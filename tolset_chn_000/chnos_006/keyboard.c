#include "core.h"

struct FIFO32 *keyfifo;
int keydata0;

char keytable1[0x54] = {
	0  , 0  , '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0  , 0  ,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '@', '[', 0  , 0  , 'a', 's', 
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ':', 0  , 0  , ']', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0  , '*', 0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,      
	0  , 0  , 0  , 0  , 0  , 0  , 0  , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.'
};

void init_keyboard(struct FIFO32 *fifo, int data0)
{
	/* �������ݐ��FIFO�o�b�t�@���L�� */
	keyfifo = fifo;
	keydata0 = data0;
	/* �L�[�{�[�h�R���g���[���̏����� */
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
	io_out8(PIC0_OCW2, 0x61);	/* IRQ-01��t������PIC�ɒʒm �B0x60+�ԍ��B*/
	fifo32_put(keyfifo,data + keydata0);
	return;
}

char keycode(int keycode)
{
	if(keycode < 0 || keycode > 0x54) return 0x00;
	return keytable1[keycode];
}
