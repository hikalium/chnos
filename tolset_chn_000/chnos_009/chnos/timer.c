
#include "core.h"

UI_TimerControl timerctrl;

void Initialise_ProgrammableIntervalTimer(void)
{
	UI_Timer *watch;

	IO_Out8(PIT_CTRL, 0x34);
	IO_Out8(PIT_CNT0, 0x9c);
	IO_Out8(PIT_CNT0, 0x2e);
	GateDescriptor_Set(0x20, (uint)asm_InterruptHandler20, 0x02, AR_INTGATE32);
	IO_Out8(PIC0_IMR, IO_In8(PIC0_IMR) & 0xfe);
	timerctrl.count = 0;
	watch = Timer_Get(0, 0);
	watch->timeout = 0xFFFFFFFF;
	watch->count = 0xFFFFFFFF;
	watch->state = inuse;
	timerctrl.next = watch;
	timerctrl.ts = 0;

	return;
}

void InterruptHandler20(int *esp)
{
	UI_Timer *tree, *old;
	bool taskswitch;

	IO_Out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	timerctrl.count++;
	taskswitch = false;

	if(timerctrl.count == timerctrl.next->timeout){	//時間になった
		tree = timerctrl.next;
		timerctrl.next = timerctrl.next->next;
		for(;;){	//同一タイムアウトを探す。
			if(tree->fifo != 0){	//FIFOの送信先が有効かチェック
				FIFO32_Put(tree->fifo, tree->data);
			} else if(tree == timerctrl.ts){
				taskswitch = true;
			}
			old = tree;
			tree = old->tree;
			old->tree = 0;	//同一タイムアウトなし
			old->state = configured;	//設定済み
			if(old->mode == interval){	//繰り返すなら
				Timer_Run(old);
			}
			if(tree == 0){	//もう同一タイムアウトはない
				break;
			}
		}
	}
	if(taskswitch){
		MultiTask_TaskSwitch();
	}
	return;
}

uint Timer_Get_Tick(void)
{
	return timerctrl.count;
}

UI_Timer *Timer_Get(DATA_FIFO *fifo, uint data)
{
	UI_Timer *timer;

	timer = MemoryBlock_Allocate_System(sizeof(UI_Timer));
	MemoryBlock_Write_Description(timer, "UI_Timer");

	timer->next = 0;
	timer->tree = 0;
	timer->timeout = 0;
	timer->count = 0;
	timer->fifo = fifo;
	timer->data = data;
	timer->state = initialized;
	return timer;
}

void Timer_Set(UI_Timer *timer, uint count, timer_mode mode)
{
	timer->count = count;
	timer->mode = mode;
	timer->state = configured;
	return;
}

void Timer_Run(UI_Timer *timer)
{
	UI_Timer **target;
	uint eflags;

	if(timer->count == 0){
		return;
	}

	if(timer->state == inuse){
		return;
	}

	eflags = IO_Load_EFlags();
	IO_CLI();

	timer->timeout = timer->count + timerctrl.count;
	target = &timerctrl.next;
	for(;;){
		if((*target)->timeout > timer->timeout){
			timer->next = *target;	//自分の次のタイマーを指す
			*target = timer;	//自分を前のタイマーに連結
			timer->tree = 0;	//自分がroot
			timer->state = inuse;	//動作中
			break;
		}
		if((*target)->timeout == timer->timeout){
			target = &(*target)->tree;
			for(;;){
				if(*target == 0){
					break;	//同時刻の最後を探す
				}
				target = &(*target)->tree;
			}
			timer->next = 0;	//常に0
			*target = timer;//自分を前のタイマーに連結
			timer->tree = 0;	//自分が終端
			timer->state = inuse;	//動作中
			break;
		}
		target = &(*target)->next;
	}
	IO_Store_EFlags(eflags);
	return;
}

void Timer_Cancel(UI_Timer *timer)
{
	UI_Timer **root, **tree;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	root = &timerctrl.next;

	for(;;){
		if((*root) == 0){	/*ルートの終端*/
			return;
		}
		if((*root) == timer){	/*ルートタイマーをキャンセル*/
			if((*root)->tree == 0){	/*ツリーなし*/
				*root = timer->next;
			} else{	/*ツリーあり*/
				*root = timer->tree;
				timer->tree->next = timer->next;
			}
			goto cancel;
		} else{
			tree = &(*root)->tree;
			for(;;){
				if((*tree) == 0){	/*ツリーの終端*/
					break;
				}
				if((*tree) == timer){	/*ツリータイマーをキャンセル*/
					*tree = timer->tree;
					goto cancel;
				}
				tree = &(*tree)->tree;
			}
		}
		root = &(*root)->next;
	}

cancel:
	timer->next = 0;
	timer->tree = 0;
	timer->timeout = 0;
	timer->state = configured;

	IO_Store_EFlags(eflags);
	return;
}

void Timer_TaskSwitch_Set(UI_Timer *ts)
{
	timerctrl.ts = ts;
	return;
}
