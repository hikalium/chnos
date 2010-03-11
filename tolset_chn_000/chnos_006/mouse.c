#include "core.h"

struct FIFO32 *mfifo;
int mdata0;

void inthandler2c(int *esp)
{
	int data;
	data = io_in8(KEYB_DATA);
	io_out8(PIC1_OCW2, 0x64);	/* IRQ-12受付完了をPICに通知 。0x60+番号。*/
	io_out8(PIC0_OCW2, 0x62);	/*IRQ-2（スレーブ）受付完了をPICに通知*/
	fifo32_put(mfifo,data + mdata0);
	return;
}

void init_mouse(struct FIFO32 *fifo, int data0)
{
	mfifo = fifo;
	mdata0 = data0;
	/* マウスの初期化 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, MOUSECMD_ENABLE);
	return;	/*成功すれば、0xfaがマウスからやってくる。*/
}
