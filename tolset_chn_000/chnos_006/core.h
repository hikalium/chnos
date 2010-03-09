/*include files*/

#include <stdio.h>

/*関数的定義*/

#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))



/*設定数値の定義*/

#define ADR_BOOTINFO	0x00000ff0
#define ADR_VESAINFO	0x00000e00
#define ADR_DISKIMG	0x00100000

#define ADR_SEG_DESC	0x00270000
#define ADR_GATE_DESC	0x0026f800

#define ADR_IDT		0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT		0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK	0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_LDT		0x0082
#define AR_TSS32		0x0089
#define AR_INTGATE32	0x008e

#define PIC0_ICW1		0x0020
#define PIC0_OCW2	0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2	0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define PIT_CTRL		0x0043
#define PIT_CNT0		0x0040

#define KEYB_DATA		0x0060

#define DESKTOP_COL8	COL8_C6C6C6
#define TASKBAR_COL8	COL8_0000FF

#define DESKTOP_COL16	RGB16(17,33,17)		/*初期値：10,20,10*/
#define TASKBAR_COL16	RGB16(20,40,30)		/*初期値：20,40,30*/

#define DESKTOP_COL32	0xC6C6C6
#define TASKBAR_COL32	0x0000FF

#define TASKBAR_HEIGHT	40	


/* asmhead.nas */

struct BOOTINFO { /* 0x0ff0-0x0fff 標準*/
	char cyls; /* ブートセクタはどこまでディスクを読んだのか */
	char leds; /* ブート時のキーボードのLEDの状態 */
	char vmode; /* ビデオモード  何ビットカラーか */
	char reserve;
	short scrnx, scrny; /* 画面解像度 */
	char *vram;
};

struct VESAINFO {/*0xe00--->512byte 標準*/
	unsigned short ModeAttributes;
	unsigned char WinAAttributes;
	unsigned char WinBAttributes;
	unsigned short WinGranularity;
	unsigned short WinSize;
	unsigned short WinASegment;
	unsigned short WinBSegment;
	unsigned int WinFuncPtr;
	unsigned short BytesPerScanLine;
	unsigned short XResolution;
	unsigned short YResolution;
	unsigned char XCharSize;
	unsigned char YCharSize;
	unsigned char NumberOfPlanes;
	unsigned char BitsPerPixel;
	unsigned char NumberOfBanks;
	unsigned char MemoryModel;
	unsigned char BankSize;
	unsigned char NumberOfImagePages;
	unsigned char Reserved;
	unsigned char RedMaskSize;
	unsigned char RedFieldPosition;
	unsigned char GreenMaskSize;
	unsigned char GreenFieldPosition;
	unsigned char BlueMaskSize;
	unsigned char BlueFieldPosition;
	unsigned char RsvdMaskSize;
	unsigned char RsvdFieldPodition;
	unsigned char DirectColorModeInfo;
	unsigned int *PhysBasePtr;
};


/*bootpack.c	ＯＳメイン*/

/*io.c		その他外部装置関係*/
void readrtc(unsigned char *t);
void init_pit(void);

/*int.c		割り込み関係ＰＩＣ等*/

void init_pic(void);
void inthandler21(int *esp);
void inthandler20(int *esp);


/*gdtidt.c		割り込み、セグメントテーブル等*/

struct SEGMENT_DESCRIPTOR { /*0x270000~0x27ffff 標準*/
	short limit_low,base_low;
	char base_mid,access_right;
	char limit_high,base_high;
};
struct GATE_DESCRIPTOR { /*0x26f800~0x26ffff 標準*/
	short offset_low,selector;
	char dw_count,access_right;
	short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

/*graphic.h	グラフィック関係*/

#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15


/*全色対応*/
unsigned short rgb_int2short (unsigned int c32);
unsigned char rgb_int2char(unsigned int c32);
void init_scrn_i(unsigned int *vram, int xsize, int ysize, unsigned char bits);
void boxfill_i(unsigned int *vrami, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
void col_pat_256safe(unsigned int *vrami, int xsize, int ysize);
void putfonts_asc_i(unsigned int *vrami, int xsize, int x, int y, unsigned int ci, unsigned char *s);

/*8bits*/
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_scrn8(unsigned char *vram, int x, int y, unsigned char *mousecur);
void putfont8(unsigned char *vram, int xsize, int x, int y, unsigned char c, unsigned char *font);
void putfonts8_asc(unsigned char *vram, int xsize, int x, int y, unsigned char c, unsigned char *s);
void init_mouse_cursor8(unsigned char *mouse, unsigned char bc);
void putblock8_8(unsigned char *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned char *buf, int bxsize);

/*16bits*/
void boxfill16(unsigned short *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1);
void init_scrn16(unsigned short *vram, int xsize, int ysize, unsigned short *mousecur);
void putfont16(unsigned short *vram, int xsize, int x, int y, unsigned short c, unsigned char *font);
void putfonts16_asc(unsigned short *vram, int xsize, int x, int y, unsigned short c, unsigned char *s);
void init_mouse_cursor16(unsigned short *mouse, unsigned short bc);
void putblock16_16(unsigned short *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned short *buf, int bxsize);

/*32bits*/
void boxfill32(unsigned int *vram, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
void init_scrn32(unsigned int *vram, int xsize, int ysize, unsigned int *mousecur);
void putfont32(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *font);
void putfonts32_asc(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *s);
void init_mouse_cursor32(unsigned int *mouse, unsigned int bc);
void putblock32_32(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize);


/* naskfunc.nas */
void pit_beep_on(void);
void pit_beep_off(void);
void clts(void);
void fnsave(int *addr);
void frstor(int *addr);
void asm_inthandler07(void);
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
int io_in32(int port);
void io_out32(int port, int data);
void io_out8(int port, int data);
int io_in16(int port);
void io_out16(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app(void);
void asm_inthandler21(void);
void asm_inthandler20(void);
