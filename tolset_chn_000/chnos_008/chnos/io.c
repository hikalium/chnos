
#include "core.h"

void init_serial(void)
{
	io_out8(0x03fb,0x80);	//ボーレート
	io_out8(0x03f8,0x06);	//0x06 = 19200, 0x01 = 115200
	io_out8(0x03fb,0x03);	//8bit
	io_out8(0x03fc,0x0b);	//pin
	io_out8(0x03f9,0x00);	//IRQデフォルト（使用しない）
	return;
}

void send_serial(uchar *s)
{
	for (; *s != 0x00; s++) {
		io_out8(0x03f8,*s);
	}

	return;
}
