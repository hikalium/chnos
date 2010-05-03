#include "core.h"

struct TIMERCTL *tctl;

void inthandler20(int *esp)
{
	int i, j;
	tctl->count++;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/

	if(tctl->next > tctl->count) return;
	for(i = 0; i < tctl->using; i++){
		if(tctl->timers[i]->timeout > tctl->count) break;
		tctl->timers[i]->flags = ALLOC_TIMER;
		fifo32_put(tctl->timers[i]->fifo, tctl->timers[i]->data);
	}
	tctl->using -= i;
	for(j = 0; j < tctl->using; j++){
		tctl->timers[j] = tctl->timers[i + j];
	}
	if(tctl->using > 0) tctl->next = tctl->timers[0]->timeout;
	else tctl->next = 0xffffffff;
	return;
}

void init_pit(struct TIMERCTL *timctl)
{
	int i;
	tctl = timctl;
	tctl->count = 0;
	tctl->next = 0xffffffff;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0xfe);
	for(i = 0; i < MAX_TIMER; i++) {
		tctl->timer[i].flags = VOID_TIMER;
	}
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
	int ef, i, j;
	timer->timeout = timeout + tctl->count;
	timer->flags = USING_TIMER;
	ef = io_load_eflags();
	io_cli();
	for(i = 0; i < tctl->using; i++){
		if(tctl->timers[i]->timeout >= timer->timeout) break;
	}
	for(j = tctl->using; j > i; j--){
		tctl->timers[j] = tctl->timers[j - 1];
	}
	tctl->using++;
	tctl->timers[i] = timer;
	tctl->next = tctl->timers[0]->timeout;
	io_store_eflags(ef);
	return;
}







