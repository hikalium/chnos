#include "core.h"

int *time_tick_100;

void inthandler20(int *esp)
{
	*time_tick_100 ++;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	return;
}

void init_pit(int *time_tick)
{
	time_tick_100 = time_tick;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	io_out8(PIC0_IMR, 0xf8);
	return;
}
