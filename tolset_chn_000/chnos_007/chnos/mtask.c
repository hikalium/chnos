
#include "core.h"

struct TASKCTL *taskctl;

void task_init(void)
{
	int i;
	taskctl = (struct TASKCTL *)system.io.mem.alloc(sizeof(struct TASKCTL));
	for(i = 0; i < MAX_TASKS; i++){
		taskctl->tasks0[i].flags = initialized;
		taskctl->tasks0[i].selector = (TASK_GDT_START + i) * 8;
		set_segmdesc(system.sys.gdt + TASK_GDT_START + i, 103, (int)&taskctl->tasks0[i].tss, AR_TSS32);
	}
	system.sys.task.main = task_alloc();
	system.sys.task.main->flags = inuse;
	system.sys.task.main->priority = 2; /*0.02sec*/
	taskctl->running = 1;
	taskctl->task_now = 0;
	taskctl->tasks[0] = system.sys.task.main;
	load_tr(system.sys.task.main->selector);
	system.sys.timer.taskswitch = timer_alloc();
	timer_settime(system.sys.timer.taskswitch, system.sys.task.main->priority);
	return;
}

struct TASK *task_alloc(void)
{
	int i;
	struct TASK *task;
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
			return task;
		}
	}
	return 0;
}

void task_run(struct TASK *task, int priority)
{
	if(priority > 0) {
		task->priority = priority;
	}
	if(task->flags != inuse){
		task->flags = inuse;
		taskctl->tasks[taskctl->running] = task;
		taskctl->running++;
	}
	return;
}

void task_switch(void)
{
	taskctl->task_now++;
	if(taskctl->task_now == taskctl->running) taskctl->task_now = 0;
	timer_settime(system.sys.timer.taskswitch, taskctl->tasks[taskctl->task_now]->priority);
	if(taskctl->running >= 2){
		farjmp(0, taskctl->tasks[taskctl->task_now]->selector);
	}
	return;
}

void task_sleep(struct TASK *task)
{
	int i;
	bool ts = false;
	if(task->flags == inuse){
		if(task == taskctl->tasks[taskctl->task_now]) ts = true;
		for(i = 0; i < taskctl->running; i++){
			if(taskctl->tasks[i] == task) break;
		}
		taskctl->running--;
		if(i < taskctl->task_now) taskctl->task_now--;
		for(; i < taskctl->running; i++){
			taskctl->tasks[i] = taskctl->tasks[i + 1];
		}
		task->flags = allocated;
		if(ts){
			if(taskctl->task_now >= taskctl->running) taskctl->task_now = 0;
			farjmp(0, taskctl->tasks[taskctl->task_now]->selector);
		}
	}
	return;
}

void task_arguments(struct TASK *task, int args, ...)
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
