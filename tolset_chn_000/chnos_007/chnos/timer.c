
#include "core.h"

void inthandler20(int *esp)
{
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00��t������PIC�ɒʒm �B0x60+�ԍ��B*/
	return;
}

