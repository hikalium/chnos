
#include "core.h"

uchar keytable0[0x80] = {
	0  , 0  , '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^',0x08,0x09,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[',0x0a, 0  , 'A', 'S', 
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0  , 0  , ']', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/', 0  , '*', 0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,      
	0  , 0  , 0  , 0  , 0  , 0  , 0  , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  
};

uchar keytable1[0x80] = {
	0  , 0  , '!',0x22, '#', '$', '%', '&',0x27, '(', ')', '~', '=', '~',0x08,0x09,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{',0x0a, 0  , 'A', 'S', 
	'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0  , 0  , '}', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0  , '*', 0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,      
	0  , 0  , 0  , 0  , 0  , 0  , 0  , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , '_', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , '|', 0  , 0  
};

uint key_shift = 0, key_leds;
uint offset_data_k;
DATA_FIFO *sendto_k, *keycmd_k;
int *keycmd_wait_k;

void Initialise_Keyboard(DATA_FIFO *sendto, DATA_FIFO *keycmd, uint offset, uint leds, int *keycmd_wait)
{
	sendto_k = sendto;
	keycmd_k = keycmd;
	offset_data_k = offset;
	keycmd_wait_k = keycmd_wait;

	Keyboard_Controller_Wait_SendReady();
	IO_Out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	Keyboard_Controller_Wait_SendReady();
	IO_Out8(KEYB_DATA, KBC_MODE);

	System_GateDescriptor_Set(0x21, (uint)asm_InterruptHandler21, 0x02, AR_INTGATE32);
	IO_Out8(PIC0_IMR, IO_In8(PIC0_IMR) & 0xfd);

	key_leds = (leds >> 4) & 7;
	FIFO32_Put(keycmd_k, KEYCMD_LED);
	FIFO32_Put(keycmd_k, key_leds);

	Keyboard_Controller_Wait_SendReady();
	IO_Out8(PORT_KEYCMD, KEYCMD_ENABLE);
	Keyboard_Controller_Wait_SendReady();
	IO_Out8(PORT_KEYCMD, 0xae);
	Keyboard_Controller_Wait_SendReady();
	IO_Out8(PORT_KEYCMD, 0xa8);

	return;
}

void InterruptHandler21(int *esp)
{
	int data;

	data = IO_In8(KEYB_DATA);
	FIFO32_Put(sendto_k, data + offset_data_k);
	IO_Out8(PIC0_OCW2, 0x61);	/* IRQ-01��t������PIC�ɒʒm�B0x60+�ԍ��B*/
	return;
}

void Keyboard_Decode(UI_KeyInfo *info, uint data)
{
	if(data == 0xfa){
		*keycmd_wait_k = -1;
	} else if(data == 0xfe){
		Keyboard_Controller_Wait_SendReady();
		IO_Out8(KEYB_DATA, *keycmd_wait_k);
	}

	if(data >= 0x00 && data <= 0x7f){
		info->make = true;
		if(data == 0x2a) key_shift |= 1;/*LShift on*/
		else if(data == 0x36) key_shift |= 2;/*Rshift on*/
		else if(data == 0x3a) Keyboard_KeyLock(4);/*CapsLock*/
		else if(data == 0x45) Keyboard_KeyLock(2);/*NumLock*/
		else if(data == 0x46) Keyboard_KeyLock(1);/*ScrollLock*/
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

void Keyboard_KeyLock(uint led)
{
	if(key_leds == (key_leds ^ led)) key_leds &= ~led;
	else key_leds ^= led;
	FIFO32_Put(keycmd_k, KEYCMD_LED);
	FIFO32_Put(keycmd_k, key_leds);
	return;
}

void Keyboard_Controller_Wait_SendReady(void)
{
	/* �L�[�{�[�h�R���g���[�����f�[�^���M�\�ɂȂ�̂�҂� */
	for (;;) {
		if ((IO_In8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

uint Keyboard_Get_KeyShift(void)
{
	return key_shift;
}
