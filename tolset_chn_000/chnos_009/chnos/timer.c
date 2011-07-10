
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
	watch->timeout = 0xFFFFFFFF;
	watch->count = 0xFFFFFFFF;
	watch->state = inuse;
	timerctrl.next = watch;

	return;
}

void InterruptHandler20(int *esp)
{
	UI_Timer *tree, *old;

	IO_Out8(PIC0_OCW2, 0x60);	/* IRQ-00��t������PIC�ɒʒm �B0x60+�ԍ��B*/
	timerctrl.count++;

	if(timerctrl.count == timerctrl.next->timeout){	//���ԂɂȂ���
		tree = timerctrl.next;
		timerctrl.next = timerctrl.next->next;
		for(;;){	//����^�C���A�E�g��T���B
			if(tree->fifo != 0){	//FIFO�̑��M�悪�L�����`�F�b�N
				FIFO32_Put(tree->fifo, tree->data);
			}
			old = tree;
			tree = old->tree;
			old->tree = 0;	//����^�C���A�E�g�Ȃ�
			old->state = configured;	//�ݒ�ς�
			if(old->mode == interval){	//�J��Ԃ��Ȃ�
				Timer_Run(old);
			}
			if(tree == 0){	//��������^�C���A�E�g�͂Ȃ�
				break;
			}
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

	timer = MemoryBlock_Allocate_System(sizeof(UI_Timer));
	MemoryBlock_Write_Description(timer, "UI_Timer");
	if(timer == 0){
		return 0;
	}
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

	eflags = IO_Load_EFlags();
	IO_CLI();

	timer->timeout = timer->count + timerctrl.count;
	target = &timerctrl.next;
	for(;;){
		if((*target)->timeout > timer->timeout){
			timer->next = *target;	//�����̎��̃^�C�}�[���w��
			*target = timer;	//������O�̃^�C�}�[�ɘA��
			timer->tree = 0;	//������root
			timer->state = inuse;	//���쒆
			break;
		}
		if((*target)->timeout == timer->timeout){
			target = &(*target)->tree;
			for(;;){
				if(*target == 0){
					break;	//�������̍Ō��T��
				}
				target = &(*target)->tree;
			}
			timer->next = *target;	//�����̑O�̃^�C�}�[���w��
			*target = timer;//������O�̃^�C�}�[�ɘA��
			timer->tree = 0;	//�������I�[
			timer->state = inuse;	//���쒆
			break;
		}
		target = &(*target)->next;
	}
	IO_Store_EFlags(eflags);
	return;
}
