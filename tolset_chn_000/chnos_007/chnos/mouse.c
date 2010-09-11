
#include "core.h"

int mdata0;

void init_mouse(int data0)
{
	mdata0 = data0;
	system.sys.mouse_decode.phase = 0;
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
	fifo32_put(&system.sys.fifo, data + mdata0);
	return;
}

int decode_mouse(int data)
{
	putfonts_asc_sht_i(system.sys.sht.desktop, 8, 200, 0xFFFFFF, 0x000000, "INT:2C IRQ:12 PS/2ﾏｳｽ");
	switch (system.sys.mouse_decode.phase){
		case 0:
			if(data == 0xfa) system.sys.mouse_decode.phase = 4;
			sendto_mouse(0xf3);
			sendto_mouse(200);
			break;
		case 1:
			if((data & 0xc8) == 0x08) {
				system.sys.mouse_decode.buf[0] = data;
				system.sys.mouse_decode.phase = 2;
			}
			break;
		case 2:
			system.sys.mouse_decode.buf[1] = data;
			system.sys.mouse_decode.phase = 3;
			break;
		case 3:
			system.sys.mouse_decode.buf[2] = data;
			system.sys.mouse_decode.phase = 1;
			system.sys.mouse_decode.btn = system.sys.mouse_decode.buf[0];
			system.sys.mouse_decode.x = system.sys.mouse_decode.buf[1];
			system.sys.mouse_decode.y = system.sys.mouse_decode.buf[2];
			if((system.sys.mouse_decode.buf[0] & 0x10) != 0) system.sys.mouse_decode.x |= 0xffffff00;
			if((system.sys.mouse_decode.buf[0] & 0x20) != 0) system.sys.mouse_decode.y |= 0xffffff00;
			system.sys.mouse_decode.y = - system.sys.mouse_decode.y;
			return 1;
		case 4:
			if(data == 0xfa) system.sys.mouse_decode.phase = 5;
			sendto_mouse(0xf3);
			sendto_mouse(100);
			break;
		case 5:
			if(data == 0xfa) system.sys.mouse_decode.phase = 6;
			sendto_mouse(0xf3);
			sendto_mouse(80);
			break;
		case 6:
			if(data == 0xfa) system.sys.mouse_decode.phase = 7;
			sendto_mouse(0xf2);
			break;
		case 7:
			if(data == 0xfa) break;
			if(data == 0x00){
				system.sys.mouse_decode.whinfo = 0x00;
				system.sys.mouse_decode.scrool = 0x00;
				system.sys.mouse_decode.phase = 1;
			} else {
				system.sys.mouse_decode.whinfo = data;
				system.sys.mouse_decode.phase = 8;
			}			
			break;
		case 8:
			if((data & 0xc8) == 0x08) {
				system.sys.mouse_decode.buf[0] = data;
				system.sys.mouse_decode.phase = 9;
				}
			break;
		case 9:
			system.sys.mouse_decode.buf[1] = data;
			system.sys.mouse_decode.phase = 10;
			break;
		case 10:
			system.sys.mouse_decode.buf[2] = data;
			system.sys.mouse_decode.btn = system.sys.mouse_decode.buf[0];
			system.sys.mouse_decode.x = system.sys.mouse_decode.buf[1];
			system.sys.mouse_decode.y = system.sys.mouse_decode.buf[2];
			if((system.sys.mouse_decode.buf[0] & 0x10) != 0) system.sys.mouse_decode.x |= 0xffffff00;
			if((system.sys.mouse_decode.buf[0] & 0x20) != 0) system.sys.mouse_decode.y |= 0xffffff00;
			system.sys.mouse_decode.y = - system.sys.mouse_decode.y;
			system.sys.mouse_decode.phase = 11;
			break;
		case 11:
			system.sys.mouse_decode.phase = 8;
			system.sys.mouse_decode.buf[3] = data;
			system.sys.mouse_decode.scrool = system.sys.mouse_decode.buf[3] & 0x0f;
			if(system.sys.mouse_decode.scrool & 0x08) {
				system.sys.mouse_decode.scrool |= 0xfffffff0;
			}
			return 1;
	}
	return -1;
}

void sendto_mouse(int data)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, data);
	return;
}

