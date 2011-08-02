
#include "core.h"

UI_TaskControl *taskctrl;

void Initialise_MultiTask(void)
{
	taskctrl = MemoryBlock_Allocate_System(sizeof(UI_TaskControl));
	MemoryBlock_Write_Description(taskctrl, "SystemTaskCtrl");

	taskctrl->next = 0;

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
	MemoryBlock_Write_Description((void *)(taskctrl->idle->tss.esp - 64 * 1024), "IdleTaskStack");
	taskctrl->idle->tss.es = 1 * 8;
	taskctrl->idle->tss.cs = 2 * 8;
	taskctrl->idle->tss.ss = 1 * 8;
	taskctrl->idle->tss.ds = 1 * 8;
	taskctrl->idle->tss.fs = 1 * 8;
	taskctrl->idle->tss.gs = 1 * 8;
	taskctrl->idle->tss.cr3 = (uint)ADR_Paging_Directory;

	MultiTask_Task_Run(taskctrl->idle);
	MultiTask_Task_Run(taskctrl->main);

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

	task->selector = System_SegmentDescriptor_Set(sizeof(IO_TaskStatusSegment) - 1, (int)&task->tss, AR_TSS32);

	task->quantum = 2; /*0.02sec Default*/

	task->cputime = 0;

	task->state = initialized;

	for(i = 0; i < (TASK_DESCRIPTION_LENGTH - 1); i++){
		if(description[i] == 0x00){
			break;
		}
		task->description[i] = description[i];
	}
	task->description[i] = 0x00;
	return task;
}

void MultiTask_Task_Change_Quantum(UI_Task *task, uint quantum)
{
	if(1 <= quantum && quantum <= 25){	/*0.01-0.25sec*/
		task->quantum = quantum;
	}
	return;
}

void MultiTask_Task_Run(UI_Task *task)
{
	uint eflags;

	if(!(1 <= task->quantum && task->quantum <= 25)){
		task->quantum = 2;
	}
	if(task->state == inuse){
		return;
	} else if(task->state == configured){
		task->state = inuse;
		return;
	}
	eflags = IO_Load_EFlags();
	IO_CLI();

	task->next = taskctrl->next;
	taskctrl->next = task;
	task->state = inuse;

	IO_Store_EFlags(eflags);
	return;
}

void MultiTask_Task_Sleep(UI_Task *task)
{
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	task->state = configured;
	if(taskctrl->now == task){
		Timer_Cancel(taskctrl->ts);
		MultiTask_TaskSwitch();
	}

	IO_Store_EFlags(eflags);

	return;
}

void MultiTask_Task_Remove(UI_Task *task)
{
	UI_Task **next, *newnow;
	uint eflags;
	bool ts;

	ts = false;

	eflags = IO_Load_EFlags();
	IO_CLI();

	if(taskctrl->now == task){
		ts = true;
	}

	for(newnow = taskctrl->next; newnow->next != 0; newnow = newnow->next){

	}

	for(next = &taskctrl->next; (*next)->next != 0; next = &(*next)->next){
		if((*next) == task){
			(*next) = task->next;
			task->next = 0;
			task->state = initialized;
			break;
		}
		newnow = (*next);
	}
	if(ts){
		taskctrl->now = newnow;
		MultiTask_TaskSwitch();
	}
	IO_Store_EFlags(eflags);
	return;
}

void MultiTask_Task_Arguments(UI_Task *task, int args, ...)
{
	int i;
	va_list ap;

	va_start(ap, args);
	
	task->tss.esp -= 4 * (args + 1);

	for(i = 1; i < args + 1; i++){
		*((int *)(task->tss.esp + (i * 4))) = va_arg(ap, int);
	}
	va_end(ap);
	return; 
}

void MultiTask_TaskSwitch(void)
{
	UI_Task *old, *new;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	old = taskctrl->now;

	for(new = taskctrl->now->next; ; new = new->next){
		if(new == 0){
			new = taskctrl->next;
		}
		if(new->state == inuse){	/*configured = sleep*/
			taskctrl->now = new;
			break;
		}
	}
	if(old == taskctrl->now){
		taskctrl->now = old;
		old = 0;
	}
	Timer_Set(taskctrl->ts, taskctrl->now->quantum, once);
	Timer_Run(taskctrl->ts);
	taskctrl->now->cputime += taskctrl->now->quantum;
	if(old){
		FarJMP(0, taskctrl->now->selector << 3);
	}
	IO_Store_EFlags(eflags);
	return;
}

UI_Task *MultiTask_Get_NowTask(void)
{
	return taskctrl->now;
}

void MultiTask_IdleTask(void)
{
	for(;;){
		IO_HLT();
	}
}
