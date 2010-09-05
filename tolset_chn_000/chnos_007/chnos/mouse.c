
#include "core.h"

void init_mouse(void)
{
//	mfifo = fifo;
//	mdata0 = data0;
//	mdec = mdec0;
//	mdec->phase = 0;
	/* マウスの初期化 */
	sendto_mouse(MOUSECMD_ENABLE);
	io_out8(PIC1_IMR, io_in8(PIC1_IMR) & 0xef);
	return;	/*成功すれば、0xfaがマウスからやってくる。*/
}

void inthandler2c(int *esp)
{
	int data;
	data = io_in8(KEYB_DATA);
	io_out8(PIC1_OCW2, 0x64);	/* IRQ-12受付完了をPICに通知 。0x60+番号。*/
	io_out8(PIC0_OCW2, 0x62);	/*IRQ-2（スレーブ）受付完了をPICに通知*/
//	fifo32_put(mfifo,data + mdata0);
	putfonts_asc_sht_i(system.sys.sht.desktop, 8, 200, 0xFFFFFF, 0x000000, "INT:2C IRQ:12 PS/2ﾏｳｽ");

	return;
}

void sendto_mouse(int data)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, data);
	return;
}

