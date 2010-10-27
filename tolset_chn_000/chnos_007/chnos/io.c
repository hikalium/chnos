
#include "core.h"

void readrtc(uchar *t)
{
    char err;
    static uchar adr[7] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09, 0x32 };
    static uchar max[7] = { 0x60, 0x59, 0x23, 0x31, 0x12, 0x99, 0x99 };
    int i;
    for (;;) { /* 読み込みが成功するまで繰り返す */
        err = 0;
        for (i = 0; i < 7; i++) {
            io_out8(0x70, adr[i]);
            t[i] = io_in8(0x71);
        }
        for (i = 0; i < 7; i++) {
            io_out8(0x70, adr[i]);
            if (t[i] != io_in8(0x71) || (t[i] & 0x0f) > 9 || t[i] > max[i]) {
                err = 1;
            }
        }
        if (err == 0) {
            return;
        }
    }
}

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


