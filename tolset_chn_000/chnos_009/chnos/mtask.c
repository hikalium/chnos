
#include "core.h"

extern uint *ADR_Paging_Directory;

UI_TaskControl *taskctrl;

void Initialise_MultiTask(void)
{
	taskctrl = MemoryBlock_Allocate_System(sizeof(UI_TaskControl));
	MemoryBlock_Write_Description(taskctrl, "SystemTaskCtrl");

	taskctrl->main = MultiTask_Task_Get("SystemMainTask");
	taskctrl->main->tss.ldtr = 0;
	taskctrl->idle->tss.iomap = 0x4000;
	taskctrl->main->tss.cr3 = (uint)ADR_Paging_Directory;
	Load_TR(taskctrl->main->selector << 3);
	taskctrl->now = taskctrl->main;

	taskctrl->idle = MultiTask_Task_Get("SystemIdleTask");
	taskctrl->idle->tss.ldtr = 0;
	taskctrl->idle->tss.iomap = 0x4000;
	taskctrl->idle->tss.eip = (uint)&MultiTask_IdleTask;
	taskctrl->idle->tss.eflags = 0x00000202;
	taskctrl->idle->tss.esp = (uint)MemoryBlock_Allocate_System(64 * 1024) + 64 * 1024;
	taskctrl->idle->tss.es = 1 * 8;
	taskctrl->idle->tss.cs = 2 * 8;
	taskctrl->idle->tss.ss = 1 * 8;
	taskctrl->idle->tss.ds = 1 * 8;
	taskctrl->idle->tss.fs = 1 * 8;
	taskctrl->idle->tss.gs = 1 * 8;
	taskctrl->idle->tss.cr3 = (uint)ADR_Paging_Directory;

	taskctrl->ts = Timer_Get(0, 0);
	Timer_Set(taskctrl->ts, 2, once);
	Timer_TaskSwitch_Set(taskctrl->ts);
	Timer_Run(taskctrl->ts);

	return;
}

UI_Task *MultiTask_Task_Get(const uchar *description)
{
	uint i;
	UI_Task *task;

	task = MemoryBlock_Allocate_System(sizeof(UI_Task));
	MemoryBlock_Write_Description(task, "UI_Task");

	task->tss.backlink	= 0;
	task->tss.reserve00	= 0;
	task->tss.esp0		= 0;
	task->tss.ss0		= 0;
	task->tss.reserve01	= 0;
	task->tss.esp1		= 0;
	task->tss.ss1		= 0;
	task->tss.reserve02	= 0;
	task->tss.esp2		= 0;
	task->tss.ss2		= 0;
	task->tss.reserve03	= 0;
	task->tss.cr3		= 0;
	task->tss.eip		= 0;
	task->tss.eflags	= 0;
	task->tss.eax		= 0;
	task->tss.ecx		= 0;
	task->tss.edx		= 0;
	task->tss.ebx		= 0;
	task->tss.esp		= 0;
	task->tss.ebp		= 0;
	task->tss.esi		= 0;
	task->tss.edi		= 0;
	task->tss.es		= 0;
	task->tss.reserve04	= 0;
	task->tss.cs		= 0;
	task->tss.reserve05	= 0;
	task->tss.ss		= 0;
	task->tss.reserve06	= 0;
	task->tss.ds		= 0;
	task->tss.reserve07	= 0;
	task->tss.fs		= 0;
	task->tss.reserve08	= 0;
	task->tss.gs		= 0;
	task->tss.reserve09	= 0;
	task->tss.ldtr		= 0;
	task->tss.reserve10	= 0;
	task->tss.flags		= 0;
	task->tss.iomap		= 0;

	task->selector = SegmentDescriptor_Set(sizeof(IO_TaskStatusSegment) - 1, (int)&task->tss, AR_TSS32);

	for(i = 0; i < (TASK_DESCRIPTION_LENGTH - 1); i++){
		if(description[i] == 0x00){
			break;
		}
		task->description[i] = description[i];
	}
	task->description[i] = 0x00;
	return task;
}

void MultiTask_TaskSwitch(void)
{
	if(taskctrl->now == taskctrl->main){
		taskctrl->now = taskctrl->idle;
	} else{
		taskctrl->now = taskctrl->main;
	}
	Timer_Run(taskctrl->ts);
	FarJMP(0, taskctrl->now->selector << 3);
	return;
}

void MultiTask_IdleTask(void)
{
	for(;;){
		IO_HLT();
	}
}
