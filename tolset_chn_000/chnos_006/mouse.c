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
	sendto_mouse(MOUSECMD_ENABLE);
	io_out8(PIC1_IMR, io_in8(PIC1_IMR) & 0xef);
	return;	/*成功すれば、0xfaがマウスからやってくる。*/
}

void sendto_mouse(int data)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, data);
	return;
}

int decode_mouse (unsigned int dat)
{
	switch (mdec->phase){
		case 0:
			if(dat == 0xfa) mdec->phase = 4;
			sendto_mouse(0xf3);
			sendto_mouse(200);
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
		case 4:
			if(dat == 0xfa) mdec->phase = 5;
			sendto_mouse(0xf3);
			sendto_mouse(100);
			break;
		case 5:
			if(dat == 0xfa) mdec->phase = 6;
			sendto_mouse(0xf3);
			sendto_mouse(80);
			break;
		case 6:
			if(dat == 0xfa) mdec->phase = 7;
			sendto_mouse(0xf2);
			break;
		case 7:
			if(dat == 0xfa) break;
			if(dat == 0x00){
				mdec->whinfo = 0x00;
				mdec->scrool = 0x00;
				mdec->phase = 1;
			} else {
				mdec->whinfo = dat;
				mdec->phase = 8;
			}			
			break;
		case 8:
			if((dat & 0xc8) == 0x08) {
				mdec->buf[0] = dat;
				mdec->phase = 9;
				}
			break;
		case 9:
			mdec->buf[1] = dat;
			mdec->phase = 10;
			break;
		case 10:
			mdec->buf[2] = dat;
			mdec->btn = mdec->buf[0];
			mdec->x = mdec->buf[1];
			mdec->y = mdec->buf[2];
			if((mdec->buf[0] & 0x10) != 0) mdec->x |= 0xffffff00;
			if((mdec->buf[0] & 0x20) != 0) mdec->y |= 0xffffff00;
			mdec->y = - mdec->y;
			mdec->phase = 11;
			break;
		case 11:
			mdec->phase = 8;
			mdec->buf[3] = dat;
			mdec->scrool = mdec->buf[3] & 0x0f;
			if(mdec->scrool & 0x08) {
				mdec->scrool |= 0xfffffff0;
			}
			return 1;
	}
	return -1;
}

