
#include "core.h"

UI_TimerControl timerctrl;

void Initialise_ProgrammableIntervalTimer(void)
{
	UI_Timer *watch;

	IO_Out8(PIT_CTRL, 0x34);
	IO_Out8(PIT_CNT0, 0x9c);
	IO_Out8(PIT_CNT0, 0x2e);
	Set_GateDescriptor((IO_GateDescriptor *)ADR_IDT + 0x20, (int) asm_InterruptHandler20, 2 * 8, AR_INTGATE32);
	IO_Out8(PIC0_IMR, IO_In8(PIC0_IMR) & 0xfe);
	timerctrl.count = 0;
	watch = Timer_Get(0, 0);
	Timer_Set(watch, 0xFFFFFFFF, once);
	watch->state = inuse;
	timerctrl.next = watch;
	return;
}

void InterruptHandler20(int *esp)
{
	UI_Timer *old;

	IO_Out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	timerctrl.count++;

	if(timerctrl.count == timerctrl.next->timeout){
		if(timerctrl.next->fifo != 0){
			FIFO32_Put(timerctrl.next->fifo, timerctrl.next->data);
		}
		old = timerctrl.next;
		timerctrl.next = timerctrl.next->next;
		old->state = configured;
		if(old->mode == interval){
			Timer_Run(old);
		}
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

	timer = System_MemoryControl_Allocate(sizeof(UI_Timer));
	if(timer == 0){
		return 0;
	}
	timer->next = 0;
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
	UI_Timer **before;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	timer->timeout = timer->count + timerctrl.count;
	before = &timerctrl.next;
	for(;;){
		if((*before)->timeout > timer->timeout){
			timer->next = *before;
			*before = timer;
			timer->state = inuse;
			break;
		} else if((*before)->timeout == timer->timeout){
			timer->timeout--;
			timer->next = *before;
			*before = timer;
			timer->state = inuse;
			break;
		}
	}
	IO_Store_EFlags(eflags);
	return;
}
