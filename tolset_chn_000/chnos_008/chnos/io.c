
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
	io_out8(COM1_CTRL_LINE, 0x80);	//ボーレート設定開始
	io_out8(COM1_BAUD_LSB, 0x06);	//0x06 = 19.2bps
	io_out8(COM1_CTRL_LINE, 0x03);	//ボーレート設定終了、送受信データ8bit
	io_out8(COM1_CTRL_MODEM, 0x0b);	//割り込み有効、RTS、DTRピンをアクティブ化
//	io_out8(COM1_INTR_ENBL, 0x04);	//ラインステータス割り込み
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

uint readcmos(uchar addr)
{
	io_out8(0x70, addr);
	return io_in8(0x71);
}

void readrtc(uchar *t)
{
	char err;
	static uchar adr[7] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09, 0x32 };
	static uchar max[7] = { 0x60, 0x59, 0x23, 0x31, 0x12, 0x99, 0x99 };
	int i;

	for (;;) {
		err = 0;
		for (i = 0; i < 7; i++) {
			t[i] = readcmos(adr[i]);
		}
		for (i = 0; i < 7; i++) {
			if (t[i] != readcmos(adr[i]) || (t[i] & 0x0f) > 9 || t[i] > max[i]) {
				err = 1;
			}
		}
		if (err == 0) {
			return;
		}
	}
}

void fdc_motor_on(uchar d)
{
	if(d == 0){
		io_out8(0x3f2,io_in8(0x3f2) | 0x10);
	} else if(d == 1){
		io_out8(0x3f2,io_in8(0x3f2) | 0x20);
	} else if(d == 2){
		io_out8(0x3f2,io_in8(0x3f2) | 0x40);
	} else if(d == 3){
		io_out8(0x3f2,io_in8(0x3f2) | 0x80);
	} 
	return;
}

void fdc_motor_off(uchar d)
{
	if(d == 0){
		io_out8(0x3f2,io_in8(0x3f2) & 0xef);
	} else if(d == 1){
		io_out8(0x3f2,io_in8(0x3f2) & 0xdf);
	} else if(d == 2){
		io_out8(0x3f2,io_in8(0x3f2) & 0xbf);
	} else if(d == 3){
		io_out8(0x3f2,io_in8(0x3f2) & 0x7f);
	} else if(d == 4){
		io_out8(0x3f2,io_in8(0x3f2) & 0xef);
		io_out8(0x3f2,io_in8(0x3f2) & 0xdf);
		io_out8(0x3f2,io_in8(0x3f2) & 0xbf);
		io_out8(0x3f2,io_in8(0x3f2) & 0x7f);
	}
	return;
}

void reset_cpu(void)
{
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, 0xfe);
    for (;;) { io_hlt(); }
}



