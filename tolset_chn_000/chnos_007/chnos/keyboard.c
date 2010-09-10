
#include "core.h"

int keydata0;

char keytable1[0x80] = {
	0  , 0  , '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0  , 0  ,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '@', '[', 0  , 0  , 'a', 's', 
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ':', 0  , 0  , ']', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0  , '*', 0  , ' ', 0  , 0  , 0  , 0  , 0  , 0  ,      
	0  , 0  , 0  , 0  , 0  , 0  , 0  , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  
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

struct KEYINFO *decode_key(struct KEYINFO *info, int data)
{
	if(data >= 0x00 && data <= 0x7f){
		info->make = true;
	} else if(data >= 0x80 && data <= 0xff){
		info->make = false;
		data -= 0x80;
	}
	info->c = keytable1[data];
	info->keycode = data;
	return info;
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

