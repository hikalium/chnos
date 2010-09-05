
#include "core.h"

void inthandler20(int *esp)
{
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	return;
}

