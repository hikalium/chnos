#include "core.h"

struct FIFO32 *keyfifo;
int keydata0;

void init_keyboard(struct FIFO32 *fifo, int data0)
{
	/* 書き込み先のFIFOバッファを記憶 */
	keyfifo = fifo;
	keydata0 = data0;
	/* キーボードコントローラの初期化 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, KBC_MODE);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0xfd);
	return;
}

void inthandler21(int *esp)
{
	int data;
	data = io_in8(KEYB_DATA);
	io_out8(PIC0_OCW2, 0x61);	/* IRQ-01受付完了をPICに通知 。0x60+番号。*/
	fifo32_put(keyfifo,data + keydata0);
	return;
}

