
#include "core.h"

#define COM1_RX		0x03f8
#define COM1_TX		0x03f8
#define COM1_BAUD_LSB	0x03f8
#define COM1_BAUD_MSB	0x03f9
#define COM1_INTR_ENBL	0x03f9
#define COM1_INTR_ID	0x03fa
#define COM1_CTRL_FIFO	0x03fa
#define COM1_CTRL_LINE	0x03fb
#define COM1_CTRL_MODEM	0x03fc
#define COM1_STA_LINE	0x03fd
#define COM1_STA_MODEM	0x03fe

void init_serial(void)
{
	io_out8(COM1_CTRL_LINE, 0x80);	//�{�[���[�g�ݒ�J�n
	io_out8(COM1_BAUD_LSB, 0x06);	//0x06 = 19.2bps
	io_out8(COM1_CTRL_LINE, 0x03);	//�{�[���[�g�ݒ�I���A����M�f�[�^8bit
	io_out8(COM1_CTRL_MODEM, 0x0b);	//���荞�ݗL���ARTS�ADTR�s�����A�N�e�B�u��
//	io_out8(COM1_INTR_ENBL, 0x04);	//���C���X�e�[�^�X���荞��
	io_out8(COM1_INTR_ENBL, 0x00);

	return;
}

void send_serial(uchar *s)
{
	for (; *s != 0x00; s++) {
		io_out8(COM1_TX, *s);
	}

	return;
}
