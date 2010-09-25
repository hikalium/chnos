
/*include files*/
#include <stdio.h>
#include <stdarg.h>
/*new object types*/

typedef enum _bool { false, true } bool;
typedef enum _color_8 { black, red, green, yellow, 
			blue, pink, light_blue, white, 
			gray, brown, dark_green, gold, 
			navy_blue, purple, dark_cyan, dark_gray} color_8;
typedef enum _state_alloc { none, initialized, allocated, configured, inuse} state_alloc;
/*definefunctions*/

#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))

/*settings*/

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

#define WINDOW_COL32	0x5EC1E8

#define INV_COL32	0xFFFFFFFF
#define INV_COL16	0xFFFF
#define INV_COL8	0xFF

#define DESKTOP_COL8	COL8_C6C6C6
#define TASKBAR_COL8	COL8_0000FF

#define DESKTOP_COL16	RGB16(17,33,17)		/*初期値：10,20,10*/
#define TASKBAR_COL16	RGB16(20,40,30)		/*初期値：20,40,30*/

#define DESKTOP_COL32	0x34FF1E
#define TASKBAR_COL32	0x5EC1E8

#define TASKBAR_HEIGHT	40

#define ADR_BOOTINFO	0x00000ff0
#define ADR_VESAINFO	0x00000e00
#define ADR_DISKIMG	0x00100000

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

#define MEMMAN_FREES	4096

#define MAX_SHEETS	1024

#define MAX_WINDOWS	256

#define MAX_TIMER	512

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

#define EFLAGS_AC_BIT	0x00040000
#define CR0_CACHE_DISABLE	0x60000000

#define FIFO32_PUT_OVER 0x0001

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

#define KEYB_DATA	0x0060
#define PORT_KEYSTA	0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE	0x60
#define KBC_MODE	0x47
#define PORT_KEYCMD	0x0064
#define KEYCMD_SENDTO_MOUSE	0xd4
#define MOUSECMD_ENABLE	0xf4

#define SYS_FIFOSIZE	256
#define SYS_FIFO_SIG_TIMERC	0x0001
#define SYS_FIFO_START_KEYB	0x100		/*256~511=keycode*/
#define SYS_FIFO_START_MOUSE	0x200		/*512~767=mouse*/

#define DATA_BYTE	0xFF

#define MAX_TASKS	1000
#define TASK_GDT_START	3

/*structures*/

extern char cursor[24][24];

struct POSITION_2D {
	int x, y;
};

struct KEYINFO {
	char c;
	int keycode;
	bool make;
}; 

struct TIMER {
	struct TIMER *next_timer;
	unsigned int timeout;
	struct FIFO32 *fifo;
	unsigned int data;
	state_alloc flags;
};

struct TIMERCTL {
	unsigned int count, next_count;
	struct TIMER timer[MAX_TIMER];
	struct TIMER *timers;
};

struct WINDOWINFO {
	char title[32];
	struct SHEET32 *win;
	unsigned int *buf;
	int winxsize, winysize, xsize, ysize;
	struct POSITION_2D position;
	struct POSITION_2D origin;
	state_alloc flags;
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
	int bxsize,bysize,vx0,vy0,height;
	state_alloc flags; 
};

struct FIFO32 {
	unsigned int *buf;
	int p, q, size, free, flags;
	struct TASK *task;
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

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct TASK {
	int selector;
	state_alloc flags;
	int priority;
	struct TSS32 tss;
};

struct TASKCTL {
	int running;
	int task_now;
	struct TASK *tasks[MAX_TASKS];
	struct TASK tasks0[MAX_TASKS];
};

struct SEGMENT_DESCRIPTOR { 
	short limit_low,base_low;
	char base_mid,access_right;
	char limit_high,base_high;
};

struct GATE_DESCRIPTOR { 
	short offset_low,selector;
	char dw_count,access_right;
	short offset_high;
};

struct BOOTINFO { 
	char cyls; 
	char leds; 
	char vmode; 
	char reserve;
	short scrnx, scrny;
	char *vram;
};

struct VESAINFO {/*0xe00--->512byte*/
	unsigned short	ModeAttributes;
	unsigned char	WinAAttributes;
	unsigned char	WinBAttributes;
	unsigned short	WinGranularity;
	unsigned short	WinSize;
	unsigned short	WinASegment;
	unsigned short	WinBSegment;
	unsigned int	WinFuncPtr;
	unsigned short	BytesPerScanLine;
	unsigned short	XResolution;
	unsigned short	YResolution;
	unsigned char	XCharSize;
	unsigned char	YCharSize;
	unsigned char	NumberOfPlanes;
	unsigned char	BitsPerPixel;
	unsigned char	NumberOfBanks;
	unsigned char	MemoryModel;
	unsigned char	BankSize;
	unsigned char	NumberOfImagePages;
	unsigned char	Reserved;
	unsigned char	RedMaskSize;
	unsigned char	RedFieldPosition;
	unsigned char	GreenMaskSize;
	unsigned char	GreenFieldPosition;
	unsigned char	BlueMaskSize;
	unsigned char	BlueFieldPosition;
	unsigned char	RsvdMaskSize;
	unsigned char	RsvdFieldPodition;
	unsigned char	DirectColorModeInfo;
	unsigned int*	PhysBasePtr;
};

/*virtualclasses*/

struct SYSTEM {
	struct SYS_IO {
		void (*clts)(void);
		void (*fnsave)(int *addr);
		void (*frstore)(int *addr);
		void (*hlt)(void);
		void (*cli)(void);
		void (*sti)(void);
		void (*stihlt)(void);
		int (*in8)(int port);
		int (*in16)(int port);
		int (*in32)(int port);
		void (*out8)(int port, int data);
		void (*out16)(int port, int data);
		void (*out32)(int port, int data);
		void (*farjmp)(int eip, int cs);
		void (*farcall)(int eip, int cs);
		struct SYS_MEMORY {
			void (*init)(void);
			unsigned int (*free_total)(void);
			void *(*alloc)(unsigned int size);
			int (*free)(void * addr, unsigned int size);
			unsigned int (*test)(unsigned int start, unsigned int end);
			unsigned int (*test_sub)(unsigned int start, unsigned int end);
			struct SYS_MEMORY_ORG {
				void (*init)(struct MEMMAN *man);
				unsigned int (*free_total)(struct MEMMAN *man);
				void *(*alloc)(struct MEMMAN *man, unsigned int size);
				int (*free)(struct MEMMAN *man, void *addr, unsigned int size);
				void *(*alloc_4k)(struct MEMMAN *man, unsigned int size);
				int (*free_4k)(struct MEMMAN *man, void *addr, unsigned int size);
			} org;
		} mem;
		struct SYS_TR {
			void (*load)(int tr);
		} tr;
		struct SYS_CR0 {
			int (*load)(void);
			void (*store)(int cr0);
		} cr0;
		struct SYS_GDT {
			void (*load)(int limit, int addr);		
		} gdt;
		struct SYS_IDT {
			void (*load)(int limit, int addr);
		} idt;
		struct SYS_EFLAGS {
			int (*load)(void);
			void (*store)(int eflags);
		} eflags;
		struct SYS_BEEP {
			void (*on)(void);
			void (*off)(void);
		} beep;
	} io;
	struct SYS_DRAW {
		void (*putfonts_sht)(struct SHEET32 *sht, int x, int y, unsigned int c, unsigned int bc, const unsigned char *s);
		void (*init_screen)(unsigned int *desktop, unsigned int *taskbar, unsigned int *mousecursor);
		void (*circle)(unsigned int *vrami, int cx, int cy, unsigned int c, int xsize, int r);
		void (*point)(unsigned int *vrami, int x, int y, unsigned int c, int xsize);
		void (*boxfill)(unsigned int *vrami, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
		void (*putfonts)(unsigned int *vrami, int xsize, int x, int y, unsigned int ci, const unsigned char *s);		
		struct SYS_COLOR {
			unsigned int (*mix)(unsigned int c0, unsigned int c1);
			unsigned char (*int2char) (unsigned int c32);
			unsigned short (*int2short) (unsigned int c32);
			void (*pattern)(unsigned int *vrami, int xsize, int ysize);		
		} color;
		struct SYS_SHEET {
			void (*init)(unsigned int *vram, int xsize, int ysize, unsigned char bits);
			struct SHEET32 *(*alloc)(void);
			void (*set)(struct SHEET32 *sht,unsigned int *buf,int xsize, int ysize, unsigned int col_inv );
			void (*updown)(struct SHEET32 *sht,int height);
			void (*refresh)(struct SHEET32 *sht, int bx0, int by0, int bx1, int by1);
			void (*slide)(struct SHEET32 *sht, int vx0, int vy0);
			void (*free)(struct SHEET32 *sht);
		} sht;
	} draw;
	struct SYS_DATA {
		struct SYS_FIFO {
			void (*init)(struct FIFO32 *fifo, int size, unsigned int *buf, struct TASK *task);
			int (*put)(struct FIFO32 *fifo, unsigned int data);
			int (*get)(struct FIFO32 *fifo);
			int (*status)(struct FIFO32 *fifo);
		} fifo;
	} data;
	struct SYS_INFOS {
		struct BOOTINFO	boot;
		struct VESAINFO vesa;	
	} info;
	struct SYS_APP {
		void (*start)(int eip, int cs, int esp, int ds, int *tss_esp0);
		void (*end)(void);
	} app;
	struct SYS_SYS {
		struct SYS_SYS_SHT {
			struct SHEET32 *desktop;
			struct SHEET32 *mouse;
			struct SHEET32 *taskbar;
			unsigned int *desktop_buf;
			unsigned int mouse_buf[24][24];
			unsigned int *taskbar_buf;
		} sht;
		struct SYS_SYS_TIMER {
			struct TIMER *t500;
			struct TIMER *taskswitch;
		} timer;
		struct SYS_SYS_TASK {
			struct TASK *main;
		} task;
		struct MEMMAN memman;
		struct TIMERCTL timctl;
		unsigned int memtotal;
		unsigned int *vram;
		unsigned int xsize;
		unsigned int ysize;
		unsigned char bpp;
		struct SEGMENT_DESCRIPTOR *gdt;
		struct GATE_DESCRIPTOR *idt;
		struct FIFO32 fifo;
		unsigned int fifo_buf[SYS_FIFOSIZE];
		struct KEYINFO keyinfo;
		struct MOUSE_DECODE mouse_decode;
	} sys;
};

extern struct SYSTEM system;

/*functions*/

/*mtask.c*/
void task_init(void);
struct TASK *task_alloc(void);
void task_run(struct TASK *task, int priority);
void task_switch(void);
void task_sleep(struct TASK *task);
void task_arguments(struct TASK *task, int args, ...);

/*io.c*/
void readrtc(unsigned char *t);
void init_serial(void);
void send_serial(unsigned char *s);
void fdc_motor_on(unsigned char d);
void fdc_motor_off(unsigned char d);

/*timer.c*/
void inthandler20(int *esp);
void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, unsigned int data);
void timer_settime(struct TIMER *timer, unsigned int timeout);

/*keyboard.c*/
void init_keyboard(int data0);
void inthandler21(int *esp);
int decode_key(struct KEYINFO *info, int data);
void wait_KBC_sendready(void);


/*mouse.c*/
void init_mouse(int data0);
void inthandler2c(int *esp);
void sendto_mouse(int data);
int decode_mouse(int data);

/*int.c*/
void init_pic(void);
void inthandler27(int *esp);

void inthandler00(int *esp);
void inthandler01(int *esp);
void inthandler02(int *esp);
void inthandler03(int *esp);
void inthandler04(int *esp);
void inthandler05(int *esp);
void inthandler06(int *esp);
void inthandler07(int *esp);
void inthandler08(int *esp);
void inthandler09(int *esp);
void inthandler0a(int *esp);
void inthandler0b(int *esp);
void inthandler0c(int *esp);
void inthandler0d(int *esp);
void inthandler0e(int *esp);
void inthandler0f(int *esp);
void inthandler10(int *esp);
void inthandler11(int *esp);
void inthandler12(int *esp);
void inthandler13(int *esp);
void inthandler14(int *esp);
void inthandler15(int *esp);
void inthandler16(int *esp);
void inthandler17(int *esp);
void inthandler18(int *esp);
void inthandler19(int *esp);
void inthandler1a(int *esp);
void inthandler1b(int *esp);
void inthandler1c(int *esp);
void inthandler1d(int *esp);
void inthandler1e(int *esp);
void inthandler1f(int *esp);

/*init.c*/
void init_system(void);

/*window.c*/
void init_windows(void);
struct WINDOWINFO *window_alloc(void);
struct WINDOWINFO *make_window32(unsigned char *title, int xsize, int ysize, int px, int py, int height);
void slide_window(struct WINDOWINFO *winfo, int px, int py);
void refresh_window(struct WINDOWINFO *winfo);
void refresh_window_alpha(struct WINDOWINFO *winfo);
void boxfill_win(struct WINDOWINFO *winfo, unsigned int c, int x0, int y0, int x1, int y1);
void putfonts_win(struct WINDOWINFO *winfo, int x, int y, unsigned int c, unsigned int bc, const unsigned char *s);

/*fifo.c*/
void fifo32_init(struct FIFO32 *fifo, int size, unsigned int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, unsigned int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

/*memory.c*/
void sys_memman_init(void);
unsigned int sys_memman_free_total(void);
void *sys_memman_alloc(unsigned int size);
int sys_memman_free(void *addr, unsigned int size);
unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_free_total(struct MEMMAN *man);
void *memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, void *addr, unsigned int size);
void *memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, void *addr, unsigned int size);

/*sheet.c*/
void init_sheets(unsigned int *vram, int xsize, int ysize, unsigned char bits);
struct SHEET32 *sheet_alloc(void);
void sheet_setbuf(struct SHEET32 *sht,unsigned int *buf,int xsize, int ysize, unsigned int col_inv );
void sheet_updown(struct SHEET32 *sht,int height);
void sheet_refresh(struct SHEET32 *sht, int bx0, int by0, int bx1, int by1);
void sheet_refresh_full_alpha(struct SHEET32 *sht);
void sheet_refresh_full(struct SHEET32 *sht);
void sheet_slide(struct SHEET32 *sht, int vx0, int vy0);
void sheet_free(struct SHEET32 *sht);
void sheet_refreshsub32(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshsub16(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshsub8(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshmap32(int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_refreshmap16(int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_refreshmap8(int vx0, int vy0, int vx1, int vy1, int h0);

/*graphic.c grap_08.c grap_16.c grap_32.c*/

/*All*/
void putfonts_asc_sht_i(struct SHEET32 *sht, int x, int y, unsigned int c, unsigned int bc, const unsigned char *s);
void init_screen_i(unsigned int *desktop, unsigned int *taskbar, unsigned int *mousecursor);
void init_desktop_i(unsigned int *vrami);
void init_taskbar_i(unsigned int *vrami);
void init_mousecursor_i(unsigned int *vrami);
unsigned int mix_color(unsigned int c0, unsigned int c1);
void circle_i(unsigned int *vrami, int cx, int cy, unsigned int c, int xsize, int r);
void point_i(unsigned int *vrami, int x, int y, unsigned int c, int xsize);
void boxfill_i(unsigned int *vrami, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
void putfonts_asc_i(unsigned int *vrami, int xsize, int x, int y, unsigned int ci, const unsigned char *s);
unsigned char rgb_int2char (unsigned int c32);
unsigned short rgb_int2short (unsigned int c32);
void col_pat(unsigned int *vrami, int xsize, int ysize);

/*8bits*/
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned int *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_desktop8(unsigned int *vram);
void init_taskbar8(unsigned int *vram);
void putfont8(unsigned int *vram, int xsize, int x, int y, unsigned char c, unsigned char *font);
void putfonts8_asc(unsigned int *vram, int xsize, int x, int y, unsigned char c, unsigned char *s);
void init_mouse_cursor8(unsigned int *mouse);
void putblock8_8(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize);

/*16bits*/
void boxfill16(unsigned int *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1);
void init_desktop16(unsigned int *vram);
void init_taskbar16(unsigned int *vram);
void putfont16(unsigned int *vram, int xsize, int x, int y, unsigned short c, unsigned char *font);
void putfonts16_asc(unsigned int *vram, int xsize, int x, int y, unsigned short c, unsigned char *s);
void init_mouse_cursor16(unsigned int *mouse);
void putblock16_16(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize);

/*32bits*/
void boxfill32(unsigned int *vram, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
void init_desktop32(unsigned int *vram);
void init_taskbar32(unsigned int *vram);
void putfont32(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *font);
void putfonts32_asc(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *s);
void init_mouse_cursor32(unsigned int *mouse);
void putblock32_32(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize);

/*gdtidt.c*/
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);


/* naskfunc.nas */
void pit_beep_on(void);
void pit_beep_off(void);
void clts(void);
void fnsave(int *addr);
void frstore(int *addr);
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
void asm_inthandler00(void);
void asm_inthandler01(void);
void asm_inthandler02(void);
void asm_inthandler03(void);
void asm_inthandler04(void);
void asm_inthandler05(void);
void asm_inthandler06(void);
void asm_inthandler07(void);
void asm_inthandler08(void);
void asm_inthandler09(void);
void asm_inthandler0a(void);
void asm_inthandler0b(void);
void asm_inthandler0c(void);
void asm_inthandler0d(void);
void asm_inthandler0e(void);
void asm_inthandler0f(void);
void asm_inthandler10(void);
void asm_inthandler11(void);
void asm_inthandler12(void);
void asm_inthandler13(void);
void asm_inthandler14(void);
void asm_inthandler15(void);
void asm_inthandler16(void);
void asm_inthandler17(void);
void asm_inthandler18(void);
void asm_inthandler19(void);
void asm_inthandler1a(void);
void asm_inthandler1b(void);
void asm_inthandler1c(void);
void asm_inthandler1d(void);
void asm_inthandler1e(void);
void asm_inthandler1f(void);
