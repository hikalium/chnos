#include "core.h"

struct FIFO32 *mfifo;
struct MOUSE_DECODE *mdec;
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

void init_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DECODE *mdec0)
{
	mfifo = fifo;
	mdata0 = data0;
	mdec = mdec0;
	mdec->phase = 0;
	/* マウスの初期化 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, MOUSECMD_ENABLE);
	return;	/*成功すれば、0xfaがマウスからやってくる。*/
}

int decode_mouse (unsigned int dat)
{
	switch (mdec->phase){
		case 0:
			if(dat == 0xfa) mdec->phase = 1;
			break;
		case 1:
			if((dat & 0xc8) == 0x08) {
				mdec->buf[0] = dat;
				mdec->phase = 2;
				}
			break;
		case 2:
			mdec->buf[1] = dat;
			mdec->phase = 3;
			break;
		case 3:
			mdec->buf[2] = dat;
			mdec->phase = 1;
			mdec->btn = mdec->buf[0];
			mdec->x = mdec->buf[1];
			mdec->y = mdec->buf[2];
			if((mdec->buf[0] & 0x10) != 0) mdec->x |= 0xffffff00;
			if((mdec->buf[0] & 0x20) != 0) mdec->y |= 0xffffff00;
			mdec->y = - mdec->y;
			return 1;
	}
	return -1;
}

