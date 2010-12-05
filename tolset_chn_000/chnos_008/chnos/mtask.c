
#include "core.h"

UI_TaskControl *taskctl;

void task_init(void)
{
	int i;
	taskctl = (UI_TaskControl *)sys_memman_alloc(sizeof(UI_TaskControl));
	for(i = 0; i < MAX_TASKS; i++){
		taskctl->tasks0[i].flags = initialized;
		taskctl->tasks0[i].selector = (TASK_GDT_START + i) * 8;
		set_segmdesc(system.io.mem.segment.gdt + TASK_GDT_START + i, 103, (int)&taskctl->tasks0[i].tss, AR_TSS32);
	}
	for(i = 0; i < MAX_LEVELS; i++){
		taskctl->level[i].running_tasks = 0;
		taskctl->level[i].task_now = 0;
	}

	system.ui.task.idle = task_alloc();
	system.ui.task.idle->tss.esp = (int)sys_memman_alloc(64 * 1024) + 64 * 1024;
	system.ui.task.idle->tss.eip = (int)&task_idle;
	system.ui.task.idle->tss.es = 1 * 8;
	system.ui.task.idle->tss.cs = 2 * 8;
	system.ui.task.idle->tss.ss = 1 * 8;
	system.ui.task.idle->tss.ds = 1 * 8;
	system.ui.task.idle->tss.fs = 1 * 8;
	system.ui.task.idle->tss.gs = 1 * 8;
	task_run(system.ui.task.idle, MAX_LEVELS - 1, 1);

	system.ui.task.main = task_alloc();
	system.ui.task.main->flags = inuse;
	system.ui.task.main->priority = 2; /*0.02sec*/
	system.ui.task.main->level = 0;
	task_add(system.ui.task.main);
	task_switchsub();
	load_tr(system.ui.task.main->selector);
	system.ui.timer.taskswitch = timer_alloc();
	timer_settime(system.ui.timer.taskswitch, system.ui.task.main->priority);
	return;
}

UI_Task *task_alloc(void)
{
	int i;
	UI_Task *task;
	for(i = 0; i < MAX_TASKS; i++){
		if(taskctl->tasks0[i].flags == initialized){
			task = &taskctl->tasks0[i];
			task->flags = allocated;
			task->tss.eflags = 0x00000202;
			task->tss.eax = 0;
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			task->tss.ss0 = 0;
			task->tss.cr3 = 0x00400000;
			return task;
		}
	}
	return 0;
}

void task_run(UI_Task *task, int level, int priority)
{
	if(level < 0){
		level = task->level;
	}
	if(priority > 0) {
		task->priority = priority;
	}
	if(task->flags == inuse && task->level != level)task_remove(task);
	if(task->flags != inuse){
		task->level = level;
		task_add(task);
	}
	taskctl->change_lv_next = true;
	return;
}

void task_switch(void)
{
	UI_Task *new_task, *now_task = taskctl->level[taskctl->level_now].tasks[taskctl->level[taskctl->level_now].task_now];

	taskctl->level[taskctl->level_now].task_now++;
	if(taskctl->level[taskctl->level_now].task_now == taskctl->level[taskctl->level_now].running_tasks) taskctl->level[taskctl->level_now].task_now = 0;
	if(taskctl->change_lv_next) task_switchsub();
	new_task = taskctl->level[taskctl->level_now].tasks[taskctl->level[taskctl->level_now].task_now];
	timer_settime(system.ui.timer.taskswitch, new_task->priority);
	if(new_task != now_task){
		farjmp(0, new_task->selector);
	}
	return;
}

void task_sleep(UI_Task *task)
{
	UI_Task *now_task;
	int eflags;

	eflags = io_load_eflags();
	io_cli();

	if(task->flags == inuse){
		now_task = task_now();
		task_remove(task);
		if(task == now_task){
			task_switchsub();
			now_task = task_now();
			farjmp(0, now_task->selector);
		}
	}

	io_store_eflags(eflags);

	return;
}

void task_arguments(UI_Task *task, int args, ...)
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

UI_Task *task_now(void)
{
	return taskctl->level[taskctl->level_now].tasks[taskctl->level[taskctl->level_now].task_now];
}

void task_add(UI_Task *task)
{
	taskctl->level[task->level].tasks[taskctl->level[task->level].running_tasks] = task;
	taskctl->level[task->level].running_tasks++;
	task->flags = inuse;
	return;	
}

void task_remove(UI_Task *task)
{
	int i;
	for(i = 0; i < taskctl->level[task->level].running_tasks; i++){
		if(taskctl->level[task->level].tasks[i] == task) break;
	}
	taskctl->level[task->level].running_tasks--;
	if(i < taskctl->level[task->level].task_now) taskctl->level[task->level].task_now--;
	if(taskctl->level[task->level].task_now >= taskctl->level[task->level].running_tasks) taskctl->level[task->level].task_now = 0;
	task->flags = allocated;
	for(; i < taskctl->level[task->level].running_tasks; i++){
		taskctl->level[task->level].tasks[i] = taskctl->level[task->level].tasks[i + 1];
	}
	return;
}

void task_switchsub(void)
{
	int i;
	for(i = 0; i < MAX_LEVELS; i++){
		if(taskctl->level[i].running_tasks > 0) break;
	}
	taskctl->level_now = i;
	taskctl->change_lv_next = false;
	return;
}

void task_idle(void)
{
	for(;;){
		io_hlt();
	}
}
