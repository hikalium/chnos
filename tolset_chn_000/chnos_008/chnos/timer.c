
#include "core.h"

void inthandler20(int *esp)
{
	UI_Timer *timer;
	char ts = 0;

	system.ui.timer.ctrl.count++;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	if(system.ui.timer.ctrl.next_count > system.ui.timer.ctrl.count) return;
	timer = system.ui.timer.ctrl.timers;
	for(;;){
		if(timer->timeout > system.ui.timer.ctrl.count) break;
		timer->flags = allocated;
		if(timer != system.ui.timer.taskswitch){
			fifo32_put(timer->fifo, timer->data);
		} else {
			ts = 1;
		}
		timer = timer->next_timer;
	}
	system.ui.timer.ctrl.timers = timer;
	system.ui.timer.ctrl.next_count = timer->timeout;
	if(ts != 0) task_switch();
	return;
}

void init_pit(void)
{
	int i;
	UI_Timer *watch;
	for(i = 0; i < MAX_TIMER; i++) {
		system.ui.timer.ctrl.timer[i].flags = initialized;
	}
	watch = timer_alloc();
	watch->timeout = 0xffffffff;
	watch->flags = inuse;
	watch->next_timer = 0;
	system.ui.timer.ctrl.timers = watch;
	system.ui.timer.ctrl.next_count = 0xffffffff;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0xfe);
	system.ui.timer.ctrl.count = 0;
	return;
}

UI_Timer *timer_alloc(void)
{
	int i;
	for(i = 0; i < MAX_TIMER; i++){
		if(system.ui.timer.ctrl.timer[i].flags == initialized){
			system.ui.timer.ctrl.timer[i].flags = allocated;
			return &system.ui.timer.ctrl.timer[i];
		}
	}
	return 0;
}

void timer_free(UI_Timer *timer)
{
	timer->flags = initialized;
	return;
}

void timer_init(UI_Timer *timer, DATA_FIFO *fifo, uint data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(UI_Timer *timer, uint timeout)
{
	int ef;
	UI_Timer *t, *s;

	timer->timeout = timeout + system.ui.timer.ctrl.count;
	timer->flags = inuse;
	ef = io_load_eflags();
	io_cli();
	t = system.ui.timer.ctrl.timers;
	if(timer->timeout <= t->timeout){
		system.ui.timer.ctrl.timers = timer;
		timer->next_timer = t;
		system.ui.timer.ctrl.next_count = timer->timeout;
		io_store_eflags(ef);
		return;
	}
	for(;;){
		s = t;
		t = t->next_timer;
		if(timer->timeout <= t->timeout){
			s->next_timer = timer;
			timer->next_timer = t;
			io_store_eflags(ef);
			return;				
		}
	}
}







