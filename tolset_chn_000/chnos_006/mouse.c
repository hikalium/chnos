#include "core.h"

struct FIFO32 *mfifo;
int mdata0;

void inthandler2c(int *esp)
{
	int data;
	data = io_in8(KEYB_DATA);
	io_out8(PIC1_OCW2, 0x64);	/* IRQ-12��t������PIC�ɒʒm �B0x60+�ԍ��B*/
	io_out8(PIC0_OCW2, 0x62);	/*IRQ-2�i�X���[�u�j��t������PIC�ɒʒm*/
	fifo32_put(mfifo,data + mdata0);
	return;
}

void init_mouse(struct FIFO32 *fifo, int data0)
{
	mfifo = fifo;
	mdata0 = data0;
	/* �}�E�X�̏����� */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, MOUSECMD_ENABLE);
	return;	/*��������΁A0xfa���}�E�X�������Ă���B*/
}
