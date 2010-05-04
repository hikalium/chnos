#include "core.h"

struct TIMERCTL *tctl;

void inthandler20(int *esp)
{
	struct TIMER *timer;
	tctl->count++;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	if(tctl->next_count > tctl->count) return;
	timer = tctl->timers;
	for(;;){
		if(timer->timeout > tctl->count) break;
		timer->flags = ALLOC_TIMER;
		fifo32_put(timer->fifo, timer->data);
		timer = timer->next_timer;
	}
	tctl->timers = timer;
	tctl->next_count = timer->timeout;
	return;
}

void init_pit(struct TIMERCTL *timctl)
{
	int i;
	struct TIMER *watch;
	for(i = 0; i < MAX_TIMER; i++) {
		tctl->timer[i].flags = VOID_TIMER;
	}
	tctl = timctl;
	watch = timer_alloc();
	watch->timeout = 0xffffffff;
	watch->flags = USING_TIMER;
	watch->next_timer = 0;
	tctl->timers = watch;
	tctl->next_count = 0xffffffff;
	tctl->using = 1;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0xfe);

	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for(i = 0; i < MAX_TIMER; i++){
		if(tctl->timer[i].flags == VOID_TIMER){
			tctl->timer[i].flags = ALLOC_TIMER;
			return &tctl->timer[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER *timer)
{
	timer->flags = VOID_TIMER;
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
	timer->timeout = timeout + tctl->count;
	timer->flags = USING_TIMER;
	ef = io_load_eflags();
	io_cli();
	tctl->using++;
	t = tctl->timers;
	if(timer->timeout <= t->timeout){
		tctl->timers = timer;
		timer->next_timer = t;
		tctl->next_count = timer->timeout;
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







