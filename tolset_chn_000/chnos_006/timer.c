#include "core.h"

volatile int *time_tick_100;

void inthandler20(int *esp)
{
	*time_tick_100++;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00��t������PIC�ɒʒm �B0x60+�ԍ��B*/
	return;
}

void init_pit(volatile int *time_tick)
{
	time_tick_100 = time_tick;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0xfe);
	return;
}
