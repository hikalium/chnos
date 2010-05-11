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
#define LIMIT_IDT	0x000007ff
#define ADR_GDT		0x00270000
#define LIMIT_GDT	0x0000ffff
#define ADR_BOTPAK	0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_LDT		0x0082
#define AR_TSS32	0x0089
#define AR_INTGATE32	0x008e

#define PIC0_ICW1	0x0020
#define PIC0_OCW2	0x0020
#define PIC0_IMR	0x0021
#define PIC0_ICW2	0x0021
#define PIC0_ICW3	0x0021
#define PIC0_ICW4	0x0021
#define PIC1_ICW1	0x00a0
#define PIC1_OCW2	0x00a0
#define PIC1_IMR	0x00a1
#define PIC1_ICW2	0x00a1
#define PIC1_ICW3	0x00a1
#define PIC1_ICW4	0x00a1

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

#define KEYB_DATA	0x0060
#define PORT_KEYSTA	0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE	0x60
#define KBC_MODE	0x47
#define PORT_KEYCMD	0x0064
#define KEYCMD_SENDTO_MOUSE	0xd4
#define MOUSECMD_ENABLE	0xf4

#define EFLAGS_AC_BIT	0x00040000
#define CR0_CACHE_DISABLE	0x60000000

#define MEMMAN_FREES	4090	
#define MEMMAN_ADDR	0x003c0000

#define MAX_SHEETS	1024

#define MAX_WINDOWS	256

#define MAX_TIMER	500

#define VOID_TIMER	0
#define ALLOC_TIMER	1
#define USING_TIMER	2

#define SHT_FLAGS_VOID	0
#define SHT_FLAGS_USE	1

#define WIN_FLAGS_VOID	0
#define WIN_FLAGS_USE	1

#define SYSFIFO_TIMERC	0x0001
#define SYSFIFO_KEYB	0x100			/*256~511=keycode*/
#define SYSFIFO_MOUSE	0x200			/*512~767=mouse*/

#define INV_COL32	0xFFFFFFFF
#define INV_COL16	0xFFFF
#define INV_COL8	0xFF
#define VOID_INV_COL32	0xFEFFFFFF

#define DESKTOP_COL8	COL8_C6C6C6
#define TASKBAR_COL8	COL8_0000FF

#define DESKTOP_COL16	RGB16(17,33,17)		/*初期値：10,20,10*/
#define TASKBAR_COL16	RGB16(20,40,30)		/*初期値：20,40,30*/

#define DESKTOP_COL32	0xC6C6C6
#define TASKBAR_COL32	0x0000FF

#define TASKBAR_HEIGHT	40

#define INT_MONITOR_LONG	320	

#define FIFO32_PUT_OVER 0x0001

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



/*構造体宣言*/
struct TIMER {
	struct TIMER *next_timer;
	unsigned int timeout, flags;
	struct FIFO32 *fifo;
	unsigned int data;
};

struct TIMERCTL {
	unsigned int count, next_count, using;
	struct TIMER timer[MAX_TIMER];
	struct TIMER *timers;
};

struct WINDOWINFO {
	unsigned char title[32];
	unsigned int *buf_c;
	struct SHEET32 *head, *center, *sideL, *sideR, *bottom; 
	unsigned int *buf_head, *buf_L, *buf_R, *buf_bottom; 
	int xsize,ysize,bxsize,bysize,px,py;
	unsigned int flags;
};

struct WINCTL {
	struct WINDOWINFO winfos[MAX_WINDOWS];
};

struct MEM_FREEINFO {
	unsigned int addr, size;
};

struct MEMMAN {
	int frees,maxfrees,lostsize,losts;
	struct MEM_FREEINFO free[MEMMAN_FREES];
};

struct SHEET32 {
	unsigned int *buf,col_inv;
	int bxsize,bysize,vx0,vy0,height,flags;
};

struct FIFO32 {
	unsigned int *buf;
	int p, q, size, free, flags;
};

struct SHTCTL {
	unsigned int *vram, *map;
	int xsize,ysize,top;
	struct SHEET32 *sheets[MAX_SHEETS];
	struct SHEET32 sheets0[MAX_SHEETS];
};

struct MOUSE_DECODE {
	unsigned int buf[4],scrool;
	int x,y,btn,whinfo;
	unsigned char phase; 

};

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

/*擬似クラス*/

struct SYSTEM {
	struct IO {
		void (*clts)(void);
		void (*fnsave)(int *addr);
		void (*frstore)(int *addr);
		void (*hlt)(void);
		void (*cli)(void);
		void (*sti)(void);
		void (*stihlt)(void);
		int (*in8)(int port);
		int (*in32)(int port);
		void (*out32)(int port, int data);
		void (*out8)(int port, int data);
		int (*in16)(int port);
		void (*out16)(int port, int data);
		void (*farcall)(int eip, int cs);
		void (*farjmp)(int eip, int cs);
		void (*readrtc)(unsigned char *t);
		struct PIC {
			void (*init)(void);
		} pic;
		struct EFLAGS { 
			int (*load)(void);
			void (*store)(int eflags);
		} eflags;
		struct GDTR {
			void (*load)(int limit, int addr);
		} gdtr;
		struct IDTR {
			void (*load)(int limit, int addr);
		} idtr;
		struct CR0 {
			int (*load)(void);
			void (*store)(int cr0);
		} cr0;
		struct TR {
			void (*load)(int tr);
		} tr;
		struct KEYBOARD {
			void (*wait)(void);
			void (*init)(struct FIFO32 *fifo, int data0);
			void (*inthandler)(int *esp);
		} keyboard;
		struct IO_TIMER {
			void (*init)(struct TIMERCTL *timctl);
			void (*inthandler)(int *esp);
		} timer;
		struct MOUSE {
			void (*inthandler)(int *esp);
			void (*init)(struct FIFO32 *fifo, int data0, struct MOUSE_DECODE *mdec0);
			int (*decode) (unsigned int dat);
		} mouse;
		struct INTERRUPT {
			void (*init)(void);
			void (*set_segment)(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
			void (*set_gate)(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
			struct HANDLER {
				void (*int27)(int *esp);
			} handler;
		} interrupt;
		struct MEMORY {
			void (*init)(struct MEMMAN *man);
			unsigned int (*test)(unsigned int start, unsigned int end);
			unsigned int (*freesize)(void);
			unsigned int (*allocb)(unsigned int size);
			int (*freeb)(unsigned int addr, unsigned int size);
			unsigned int (*alloc)(unsigned int size);
			int (*free)(unsigned int addr, unsigned int size);
		} memory;
		struct BEEP {
			void (*on)(void);
			void (*off)(void);
		} beep;
		struct SERIAL {
			void (*init)(void);
			void (*send)(unsigned char *s);
		} serial;
		struct FDC {
			void (*motor_on)(unsigned char d);
			void (*motor_off)(unsigned char d);
		} fdc;
	} io;
	struct DRAW {
		void (*circle)(unsigned int *vrami, int cx, int cy, unsigned int c, int xsize, int r);
		unsigned short (*int2short) (unsigned int c32);
		unsigned char (*int2char)(unsigned int c32);
		void (*init_scrn)(unsigned int *vram, int xsize, int ysize, unsigned char bits, unsigned int *mousecur32);
		void (*boxfill)(unsigned int *vrami, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
		void (*col_pat_256safe)(unsigned int *vrami, int xsize, int ysize);
		void (*putfonts)(unsigned int *vrami, int xsize, int x, int y, unsigned int ci, const unsigned char *s);
		void (*point)(unsigned int *vrami, int x, int y, unsigned int c32, int xsize);
		struct DRAW_BITS8 {
			void (*init)(void);
			void (*set)(int start, int end, unsigned char *rgb);
			void (*boxfill)(unsigned int *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
			void (*init_scrn)(unsigned int *vram, int xsize, int ysize, unsigned int *mousecur);
			void (*putfont)(unsigned int *vram, int xsize, int x, int y, unsigned char c, unsigned char *font);
			void (*putfonts)(unsigned int *vram, int xsize, int x, int y, unsigned char c, unsigned char *s);
			void (*mouse_cursor)(unsigned int *mouse);
			void (*putblock)(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize);
		} bits8;
		struct DRAW_BITS16 {
			void (*boxfill)(unsigned int *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1);
			void (*init_scrn)(unsigned int *vram, int xsize, int ysize ,unsigned int *mousecur);
			void (*putfont)(unsigned int *vram, int xsize, int x, int y, unsigned short c, unsigned char *font);
			void (*putfonts)(unsigned int *vram, int xsize, int x, int y, unsigned short c, unsigned char *s);
			void (*mouse_cursor)(unsigned int *mouse);
			void (*putblock)(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize);
		} bits16;
		struct DRAW_BITS32 {
			void (*boxfill)(unsigned int *vram, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
			void (*init_scrn)(unsigned int *vram, int xsize, int ysize, unsigned int *mousecur);
			void (*putfont)(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *font);
			void (*putfonts)(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *s);
			void (*mouse_cursor)(unsigned int *mouse);
			void (*putblock)(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize);
		}bits32;
		struct WINDOW {
			void (*init)(void);
			struct WINDOWINFO *(*alloc)(void);
			struct WINDOWINFO *(*make)(unsigned int *buf, unsigned char *title, int xsize, int ysize, int px, int py, int height);
			void (*slide)(struct WINDOWINFO *winfo, int px, int py);
		} window;
		struct SHEET {
			void (*init)(unsigned int *vram, int xsize, int ysize, unsigned char bits);
			struct SHEET32 *(*alloc)(void);
			void (*set)(struct SHEET32 *sht,unsigned int *buf,int xsize, int ysize, unsigned int col_inv );
			void (*updown)(struct SHEET32 *sht,int height);
			void (*refresh)(struct SHEET32 *sht, int bx0, int by0, int bx1, int by1);
			void (*slide)(struct SHEET32 *sht, int vx0, int vy0);
			void (*free)(struct SHEET32 *sht);
		} sheet;
	} draw;
	struct DATA {
		struct FIFO {
			void (*init)(struct FIFO32 *fifo, int size, unsigned int *buf);
			int (*put)(struct FIFO32 *fifo, unsigned int data);
			int (*get)(struct FIFO32 *fifo);
			int (*status)(struct FIFO32 *fifo);
		} fifo;
	} data;
	struct INFOS {
		struct VESAINFO vesa;
		struct BOOTINFO boot;
	} info;
	struct FIFO32 sysfifo;
	struct MOUSE_DECODE mouse_dec;
	int fifobuf[256];
	struct MEMMAN memman;
	struct SHEET32 *sht_back;
};

/*関数宣言*/


/*bootpack.c	ＯＳメイン*/

/*system.c	関数ポインタ関係、system構造体の管理*/
void init_system(struct SYSTEM *system);

/*io.c		その他外部装置関係*/

void readrtc(unsigned char *t);
void wait_KBC_sendready(void);
void init_serial(void);
void send_serial(unsigned char *s);
void fdc_motor_on(unsigned char d);
void fdc_motor_off(unsigned char d);

/*int.c		割り込み関係ＰＩＣ等*/

void init_pic(void);
void inthandler27(int *esp);

/*memory.c	メモリ管理関係*/

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_free_total(void);
unsigned int memman_alloc(unsigned int size);
int memman_free(unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(unsigned int size);
int memman_free_4k(unsigned int addr, unsigned int size);

/*window.c	ウィンドウ関係*/
void init_windows(void);
struct WINDOWINFO *window_alloc(void);
struct WINDOWINFO *make_window32(unsigned int *buf, unsigned char *title, int xsize, int ysize, int px, int py, int height);
void slide_window(struct WINDOWINFO *winfo, int px, int py);

/*sheet.c	画面管理関係*/

void init_sheets(unsigned int *vram, int xsize, int ysize, unsigned char bits);
struct SHEET32 *sheet_alloc(void);
void sheet_setbuf(struct SHEET32 *sht,unsigned int *buf,int xsize, int ysize, unsigned int col_inv );
void sheet_updown(struct SHEET32 *sht,int height);
void sheet_refresh(struct SHEET32 *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET32 *sht, int vx0, int vy0);
void sheet_free(struct SHEET32 *sht);
void sheet_refreshsub32(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshsub8(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshmap32(int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_refreshmap8(int vx0, int vy0, int vx1, int vy1, int h0);
/*keyboard.c	キーボード関係*/
void init_keyboard(struct FIFO32 *fifo, int data0);
void inthandler21(int *esp);
char keycode(int keycode);

/*mouse.c	マウス関係*/

void inthandler2c(int *esp);
void init_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DECODE *mdec0);
int decode_mouse (unsigned int dat);
void sendto_mouse(int data);


/*timer.c		タイマー関係*/
void init_pit(struct TIMERCTL *timctl);
void inthandler20(int *esp);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, unsigned int data);
void timer_settime(struct TIMER *timer, unsigned int timeout);

/*fifo.c		FIFOバッファ関係*/

void fifo32_init(struct FIFO32 *fifo, int size, unsigned int *buf);
int fifo32_put(struct FIFO32 *fifo, unsigned int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

/*gdtidt.c		割り込み、セグメントテーブル等*/

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

/*graphic.h	グラフィック関係*/

/*全色対応*/
void putfonts_asc_sht_i(struct SHEET32 *sht, int x, int y, unsigned int c, unsigned int bc, const char *s);
void init_scrn_i(unsigned int *vrami, int xsize, int ysize, unsigned char bits, unsigned int *mousecur);
unsigned int mix_color(unsigned int c0, unsigned int c1);
void circle_i(unsigned int *vrami, int cx, int cy, unsigned int c, int xsize, int r);
void point_i(unsigned int *vrami, int x, int y, unsigned int c, int xsize);
void boxfill_i(unsigned int *vrami, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
void putfonts_asc_i(unsigned int *vrami, int xsize, int x, int y, unsigned int ci, const unsigned char *s);
unsigned char rgb_int2char (unsigned int c32);
unsigned short rgb_int2short (unsigned int c32);
void col_pat_256safe(unsigned int *vrami, int xsize, int ysize);

/*8bits*/
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned int *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_scrn8(unsigned int *vram, int xsize, int ysize, unsigned int *mousecur);
void putfont8(unsigned int *vram, int xsize, int x, int y, unsigned char c, unsigned char *font);
void putfonts8_asc(unsigned int *vram, int xsize, int x, int y, unsigned char c, unsigned char *s);
void init_mouse_cursor8(unsigned int *mouse);
void putblock8_8(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize);

/*16bits*/
void boxfill16(unsigned int *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1);
void init_scrn16(unsigned int *vram, int xsize, int ysize ,unsigned int *mousecur);
void putfont16(unsigned int *vram, int xsize, int x, int y, unsigned short c, unsigned char *font);
void putfonts16_asc(unsigned int *vram, int xsize, int x, int y, unsigned short c, unsigned char *s);
void init_mouse_cursor16(unsigned int *mouse);
void putblock16_16(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize);

/*32bits*/
void boxfill32(unsigned int *vram, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
void init_scrn32(unsigned int *vram, int xsize, int ysize, unsigned int *mousecur);
void putfont32(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *font);
void putfonts32_asc(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *s);
void init_mouse_cursor32(unsigned int *mouse);
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
void asm_inthandler2c(void);
void asm_inthandler27(void);


