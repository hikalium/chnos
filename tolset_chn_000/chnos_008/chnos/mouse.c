
#include "core.h"

uint offset_data_m;

void init_mouse(uint offset)
{
	offset_data_m = offset;

	system.io.mouse.decode.phase = 0;
	/* マウスの初期化 */
	sendto_mouse(MOUSECMD_ENABLE);
	set_gatedesc(system.io.interrupt.idt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);
	io_out8(PIC1_IMR, io_in8(PIC1_IMR) & 0xef);

	return;
}

void inthandler2c(int *esp)
{
	int data;

	data = io_in8(KEYB_DATA);
	io_out8(PIC1_OCW2, 0x64);	/* IRQ-12受付完了をPICに通知 。0x60+番号。*/
	io_out8(PIC0_OCW2, 0x62);	/*IRQ-2（スレーブ）受付完了をPICに通知*/
	fifo32_put(&system.data.fifo.mousectrl, data + offset_data_m);
	return;
}

int decode_mouse(uint data)
{
	switch (system.io.mouse.decode.phase){
		case 0:
			if(data == 0xfa) system.io.mouse.decode.phase = 4;
			sendto_mouse(0xf3);
			sendto_mouse(200);
			break;
		case 1:
			if((data & 0xc8) == 0x08) {
				system.io.mouse.decode.buf[0] = data;
				system.io.mouse.decode.phase = 2;
			}
			break;
		case 2:
			system.io.mouse.decode.buf[1] = data;
			system.io.mouse.decode.phase = 3;
			break;
		case 3:
			system.io.mouse.decode.buf[2] = data;
			system.io.mouse.decode.phase = 1;
			system.io.mouse.decode.btn = system.io.mouse.decode.buf[0];
			system.io.mouse.decode.x = system.io.mouse.decode.buf[1];
			system.io.mouse.decode.y = system.io.mouse.decode.buf[2];
			if((system.io.mouse.decode.buf[0] & 0x10) != 0) system.io.mouse.decode.x |= 0xffffff00;
			if((system.io.mouse.decode.buf[0] & 0x20) != 0) system.io.mouse.decode.y |= 0xffffff00;
			system.io.mouse.decode.y = - system.io.mouse.decode.y;
			return 1;
		case 4:
			if(data == 0xfa) system.io.mouse.decode.phase = 5;
			sendto_mouse(0xf3);
			sendto_mouse(100);
			break;
		case 5:
			if(data == 0xfa) system.io.mouse.decode.phase = 6;
			sendto_mouse(0xf3);
			sendto_mouse(80);
			break;
		case 6:
			if(data == 0xfa) system.io.mouse.decode.phase = 7;
			sendto_mouse(0xf2);
			break;
		case 7:
			if(data == 0xfa) break;
			if(data == 0x00){
				system.io.mouse.decode.whinfo = 0x00;
				system.io.mouse.decode.scrool = 0x00;
				system.io.mouse.decode.phase = 1;
			} else {
				system.io.mouse.decode.whinfo = data;
				system.io.mouse.decode.phase = 8;
			}			
			break;
		case 8:
			if((data & 0xc8) == 0x08) {
				system.io.mouse.decode.buf[0] = data;
				system.io.mouse.decode.phase = 9;
				}
			break;
		case 9:
			system.io.mouse.decode.buf[1] = data;
			system.io.mouse.decode.phase = 10;
			break;
		case 10:
			system.io.mouse.decode.buf[2] = data;
			system.io.mouse.decode.btn = system.io.mouse.decode.buf[0];
			system.io.mouse.decode.x = system.io.mouse.decode.buf[1];
			system.io.mouse.decode.y = system.io.mouse.decode.buf[2];
			if((system.io.mouse.decode.buf[0] & 0x10) != 0) system.io.mouse.decode.x |= 0xffffff00;
			if((system.io.mouse.decode.buf[0] & 0x20) != 0) system.io.mouse.decode.y |= 0xffffff00;
			system.io.mouse.decode.y = - system.io.mouse.decode.y;
			system.io.mouse.decode.phase = 11;
			break;
		case 11:
			system.io.mouse.decode.phase = 8;
			system.io.mouse.decode.buf[3] = data;
			system.io.mouse.decode.scrool = system.io.mouse.decode.buf[3] & 0x0f;
			if(system.io.mouse.decode.scrool & 0x08) {
				system.io.mouse.decode.scrool |= 0xfffffff0;
			}
			return 1;
	}
	return 0;
}

void sendto_mouse(uint data)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, data);
	return;
}

