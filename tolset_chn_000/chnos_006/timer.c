#include "core.h"

struct TIMERCTL *tctl;

void inthandler20(int *esp)
{
	tctl->count++;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	return;
}

void init_pit(struct TIMERCTL *timctl)
{
	tctl = timctl;
	tctl->count = 0;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0xfe);
	return;
}
