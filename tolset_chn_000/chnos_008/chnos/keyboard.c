
#include "core.h"

uchar keytable0[0x80] = {
	0  , 0  , '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0  , 0  ,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0  , 0  , 'A', 'S', 
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0  , 0  , ']', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/', 0  , '*', 0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,      
	0  , 0  , 0  , 0  , 0  , 0  , 0  , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  
};

uchar keytable1[0x80] = {
	0  , 0  , '!',0x22, '#', '$', '%', '&',0x27, '(', ')', '~', '=', '~', 0  , 0  ,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0  , 0  , 'A', 'S', 
	'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0  , 0  , '}', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0  , '*', 0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,      
	0  , 0  , 0  , 0  , 0  , 0  , 0  , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , '_', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , '|', 0  , 0  
};

uint key_shift = 0, key_leds;
uint offset_data_k;

void init_keyboard(uint offset)
{
	offset_data_k = offset;

	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, KBC_MODE);

	set_gatedesc(system.io.interrupt.idt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0xfd);

	key_leds = (system.data.info.boot.leds >> 4) & 7;
	fifo32_put(&system.data.fifo.keycmd, KEYCMD_LED);
	fifo32_put(&system.data.fifo.keycmd, key_leds);

	return;
}

void inthandler21(int *esp)
{
	int data;
	data = io_in8(KEYB_DATA);
	io_out8(PIC0_OCW2, 0x61);	/* IRQ-01受付完了をPICに通知 。0x60+番号。*/
	fifo32_put(&system.data.fifo.keyctrl, data + offset_data_k);
	return;
}

void decode_key(UI_KeyInfo *info, uint data)
{
	if(data == 0xfa){
		system.io.keyboard.cmd_wait = -1;
	} else if(data == 0xfe){
		wait_KBC_sendready();
		io_out8(KEYB_DATA, system.io.keyboard.cmd_wait);
	}

	if(data >= 0x00 && data <= 0x7f){
		info->make = true;
		if(data == 0x2a) key_shift |= 1;/*LShift on*/
		else if(data == 0x36) key_shift |= 2;/*Rshift on*/
		else if(data == 0x3a) keylock(4);/*CapsLock*/
		else if(data == 0x45) keylock(2);/*NumLock*/
		else if(data == 0x46) keylock(1);/*ScrollLock*/
	} else if(data >= 0x80 && data <= 0xff){
		info->make = false;
		data -= 0x80;
		if(data == 0x2a) key_shift &= ~1;/*LShift off*/
		else if(data == 0x36) key_shift &= ~2;/*Rshift off*/
	}

	if(key_shift == 0){
		info->c = keytable0[data];
	} else{
		info->c = keytable1[data];
	}

	if('A' <= info->c && info->c <= 'Z'){
		if(((key_leds & 4) == 0 && key_shift == 0) || ((key_leds & 4) != 0 && key_shift != 0)) info->c += 0x20;
		info->alphabet = true;
	} else {
		info->alphabet = false;
	}

	info->keycode = data;
	return;
}

void keylock(uint led)
{
	if(key_leds == (key_leds ^ led)) key_leds &= ~led;
	else key_leds ^= led;
	fifo32_put(&system.data.fifo.keycmd, KEYCMD_LED);
	fifo32_put(&system.data.fifo.keycmd, key_leds);
	return;
}

void wait_KBC_sendready(void)
{
	/* キーボードコントローラがデータ送信可能になるのを待つ */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

