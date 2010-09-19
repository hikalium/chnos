
#include "core.h"

void inthandler20(int *esp)
{
	struct TIMER *timer;
	system.sys.timctl.count++;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	if(system.sys.timctl.next_count > system.sys.timctl.count) return;
	timer = system.sys.timctl.timers;
	for(;;){
		if(timer->timeout > system.sys.timctl.count) break;
		timer->flags = allocated;
		fifo32_put(timer->fifo, timer->data);
		timer = timer->next_timer;
	}
	system.sys.timctl.timers = timer;
	system.sys.timctl.next_count = timer->timeout;
	return;
}

void init_pit(void)
{
	int i;
	struct TIMER *watch;
	for(i = 0; i < MAX_TIMER; i++) {
		system.sys.timctl.timer[i].flags = initialized;
	}
	watch = timer_alloc();
	watch->timeout = 0xffffffff;
	watch->flags = inuse;
	watch->next_timer = 0;
	system.sys.timctl.timers = watch;
	system.sys.timctl.next_count = 0xffffffff;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0xfe);
	system.sys.timctl.count = 0;
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for(i = 0; i < MAX_TIMER; i++){
		if(system.sys.timctl.timer[i].flags == initialized){
			system.sys.timctl.timer[i].flags = allocated;
			return &system.sys.timctl.timer[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER *timer)
{
	timer->flags = initialized;
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, unsigned int data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	int ef;
	struct TIMER *t, *s;
	timer->timeout = timeout + system.sys.timctl.count;
	timer->flags = inuse;
	ef = io_load_eflags();
	io_cli();
	t = system.sys.timctl.timers;
	if(timer->timeout <= t->timeout){
		system.sys.timctl.timers = timer;
		timer->next_timer = t;
		system.sys.timctl.next_count = timer->timeout;
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







