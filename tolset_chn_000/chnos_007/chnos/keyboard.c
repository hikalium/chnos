
#include "core.h"

int keydata0;

char keytable0[0x80] = {
	0  , 0  , '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0  , 0  ,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0  , 0  , 'A', 'S', 
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0  , 0  , ']', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/', 0  , '*', 0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,      
	0  , 0  , 0  , 0  , 0  , 0  , 0  , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  
};

char keytable1[0x80] = {
	0  , 0  , '!',0x22, '#', '$', '%', '&',0x27, '(', ')', '~', '=', '~', 0  , 0  ,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0  , 0  , 'A', 'S', 
	'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0  , 0  , '}', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0  , '*', 0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,      
	0  , 0  , 0  , 0  , 0  , 0  , 0  , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , '_', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , '|', 0  , 0  
};

char *keynames[0x80] = {
	"[Reserved]                     ",
	"<ESC>                          ",
	"1(Full-Key)                    ",
	"2(Full-Key)                    ",
	"3(Full-Key)                    ",
	"4(Full-Key)                    ",
	"5(Full-Key)                    ",
	"6(Full-Key)                    ",
	"7(Full-Key)                    ",
	"8(Full-Key)                    ",
	"9(Full-Key)                    ",
	"0(Full-Key)                    ",
	"-(Full-Key)                    ",
	"^(Full-Key)(=(English))        ",
	"<BackSpace>                    ",
	"<Tab>                          ",
/*10*/	"Q                              ",
	"W                              ",
	"E                              ",
	"R                              ",
	"T                              ",
	"Y                              ",
	"U                              ",
	"I                              ",
	"O                              ",
	"P                              ",
	"@([(English))                  ",
	"[(](English))                  ",
	"<Enter>(Full-Key)              ",
	"<Ctrl>(Left)                   ",
	"A                              ",
	"S                              ",
/*20*/	"D                              ",
	"F                              ",
	"G                              ",
	"H                              ",
	"J                              ",
	"K                              ",
	"L                              ",
	";                              ",
	":('(English))                  ",
	"<Zenkaku-Hankaku>(`(English))  ",
	"<Shift>(Left)                  ",
	"](<Back-Slash>(English))       ",
	"Z                              ",
	"X                              ",
	"C                              ",
	"V                              ",
/*30*/	"B                              ",
	"N                              ",
	"M                              ",
	",                              ",
	".                              ",
	"/                              ",
	"<Shift>(Right)                 ",
	"*(Ten-Key)                     ",
	"<Alt>(Left)                    ",
	"<Space>                        ",
	"<CapsLock>                     ",
	"<F1>                           ",
	"<F2>                           ",
	"<F3>                           ",
	"<F4>                           ",
	"<F5>                           ",
/*40*/	"<F6>                           ",
	"<F7>                           ",
	"<F8>                           ",
	"<F9>                           ",
	"<F10>                          ",
	"<NumLock>                      ",
	"<ScrollLock>                   ",
	"7(Ten-Key)                     ",
	"8(Ten-Key)                     ",
	"9(Ten-Key)                     ",
	"-(Ten-Key)                     ",
	"4(Ten-Key)                     ",
	"5(Ten-Key)                     ",
	"6(Ten-Key)                     ",
	"+(Ten-Key)                     ",
	"1(Ten-Key)                     ",
/*50*/	"2(Ten-Key)                     ",
	"3(Ten-Key)                     ",
	"0(Ten-Key)                     ",
	".(Ten-Key)                     ",
	"<SysRequest>                   ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"<F11>                          ",
	"<F12>                          ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
/*60*/	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
/*70*/	"Hiragana                       ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"_                              ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"[Reserved]                     ",
	"Hennkann                       ",
	"[Reserved]                     ",
	"Mu-Hennkann                    ",
	"[Reserved]                     ",
	"Yen or Back-Slash              ",
	"[Reserved]                     ",
	"[Reserved]                     "
};

int key_shift = 0, key_leds;

void init_keyboard(int data0)
{
	/* 書き込み先のFIFOバッファを記憶 */
	keydata0 = data0;
	/* キーボードコントローラの初期化 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(KEYB_DATA, KBC_MODE);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0xfd);

	key_leds = (system.info.boot.leds >> 4) & 7;
	fifo32_put(&system.sys.keycmd, KEYCMD_LED);
	fifo32_put(&system.sys.keycmd, key_leds);

	return;
}

void inthandler21(int *esp)
{
	int data;
	data = io_in8(KEYB_DATA);
	io_out8(PIC0_OCW2, 0x61);	/* IRQ-01受付完了をPICに通知 。0x60+番号。*/
	fifo32_put(&system.sys.fifo, data + keydata0);
	return;
}

int decode_key(UI_KeyInfo *info, int data)
{
	if(data == 0xfa){
		system.sys.keycmd_wait = -1;
	} else if(data == 0xfe){
		wait_KBC_sendready();
		io_out8(KEYB_DATA, system.sys.keycmd_wait);
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
		if(data == 0x2a) key_shift &= ~1;/*LShift on*/
		else if(data == 0x36) key_shift &= ~2;/*Rshift on*/
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
	return 0;
}

void keylock(int led)
{
	key_leds ^= led;
	fifo32_put(&system.sys.keycmd, KEYCMD_LED);
	fifo32_put(&system.sys.keycmd, key_leds);
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

