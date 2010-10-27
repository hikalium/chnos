
/*include files*/
#include <stdio.h>
#include <stdarg.h>

/*new object types*/
typedef enum _bool { false, true} bool;
typedef enum _state_alloc { none, initialized, allocated, configured, inuse} state_alloc;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

/*definefunctions*/
#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))

/*settings*/

#define system	(*sys_main_str_buf)

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

#define INV_COL32	0xFFFFFFFF
#define INV_COL16	0x1192
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
#define WIN_COL8_ACTIVE		0x000084
#define WIN_COL8_INACTIVE	0x848484
#define WIN_COL16_ACTIVE	0x60F8f0
#define WIN_COL16_INACTIVE	0xBCFCF8
#define WIN_COL32_ACTIVE	0x93D9FF
#define WIN_COL32_INACTIVE	0xD0EFFF

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
#define KEYCMD_LED	0xed

#define SYS_FIFOSIZE	256
#define SYS_FIFO_SIG_TIMERC	0x0001
#define SYS_FIFO_START_KEYB	0x100		/*256~511=keycode*/
#define SYS_FIFO_START_MOUSE	0x200		/*512~767=mouse*/

#define CONSOLE_FIFO_BUF_SIZE	128
#define CONSOLE_FIFO_CURSOR_START	2
#define CONSOLE_FIFO_CURSOR_STOP	3
#define CONSOLE_COLOR_BACKGROUND	0x000000
#define CONSOLE_COLOR_CHAR	0xFFFFFF
#define CONSOLE_CMDLINE_BUF_SIZE	128


#define KEYCMD_FIFOSIZE	32

#define DATA_BYTE	0xFF

#define MAX_TASKS	1000
#define TASK_GDT_START	3
#define MAX_LEVEL_TASKS	100
#define MAX_LEVELS	10

/*structures*/

struct FILEINFO {
	uchar name[8];
	uchar ext[3];
	uchar type;
	uchar reserve;
	uchar VFAT_createTimeMs;
	ushort VFAT_createTime;
	ushort VFAT_createDate;
	ushort VFAT_accessDate;
	ushort VFAT_clusterHighWord;
	ushort time;
	ushort date;
	ushort clustno;
	uint size;
};

struct POSITION_2D {
	int x, y;
};

struct KEYINFO {
	char c;
	int keycode;
	bool make;
	bool alphabet;
}; 

struct TIMER {
	struct TIMER *next_timer;
	uint timeout;
	struct FIFO32 *fifo;
	uint data;
	state_alloc flags;
};

struct TIMERCTL {
	uint count, next_count;
	struct TIMER timer[MAX_TIMER];
	struct TIMER *timers;
};

struct WINDOWINFO {
	char title[32];
	struct SHEET32 *win;
	void *buf;
	int winxsize, winysize, xsize, ysize;
	struct POSITION_2D position;
	struct POSITION_2D origin;
	state_alloc flags;
	bool active;
};

struct WINCTL {
	struct WINDOWINFO winfos[MAX_WINDOWS];
};

struct MEM_FREEINFO {
	uint addr, size;
};

struct MEMMAN {
	int frees,maxfrees,lostsize,losts;
	struct MEM_FREEINFO free[MEMMAN_FREES];
};

struct SHEET32 {
	void *buf;
	uint col_inv;
	int bxsize, bysize, vx0, vy0, height;
	state_alloc flags; 
};

struct FIFO32 {
	uint *buf;
	int p, q, size, free, flags;
	struct TASK *task;
};

struct SHTCTL {
	uint *map;
	void *vram;
	int xsize,ysize,top;
	struct SHEET32 *sheets[MAX_SHEETS];
	struct SHEET32 sheets0[MAX_SHEETS];
};

struct MOUSE_DECODE {
	uint buf[4],scrool;
	int x,y,btn,whinfo;
	uchar phase; 

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
	int level, priority;
	struct FIFO32 fifo;
	struct TSS32 tss;
};

struct TASKLEVEL {
	int running_tasks;
	int task_now;
	struct TASK *tasks[MAX_LEVEL_TASKS];
};

struct TASKCTL {
	int level_now;
	bool change_lv_next;
	struct TASKLEVEL level[MAX_LEVELS];
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
	ushort	ModeAttributes;
	uchar	WinAAttributes;
	uchar	WinBAttributes;
	ushort	WinGranularity;
	ushort	WinSize;
	ushort	WinASegment;
	ushort	WinBSegment;
	uint	WinFuncPtr;
	ushort	BytesPerScanLine;
	ushort	XResolution;
	ushort	YResolution;
	uchar	XCharSize;
	uchar	YCharSize;
	uchar	NumberOfPlanes;
	uchar	BitsPerPixel;
	uchar	NumberOfBanks;
	uchar	MemoryModel;
	uchar	BankSize;
	uchar	NumberOfImagePages;
	uchar	Reserved;
	uchar	RedMaskSize;
	uchar	RedFieldPosition;
	uchar	GreenMaskSize;
	uchar	GreenFieldPosition;
	uchar	BlueMaskSize;
	uchar	BlueFieldPosition;
	uchar	RsvdMaskSize;
	uchar	RsvdFieldPodition;
	uchar	DirectColorModeInfo;
	uint*	PhysBasePtr;
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
			uint (*free_total)(void);
			void *(*alloc)(uint size);
			int (*free)(void * addr, uint size);
			uint (*test)(uint start, uint end);
			uint (*test_sub)(uint start, uint end);
			struct SYS_MEMORY_ORG {
				void (*init)(struct MEMMAN *man);
				uint (*free_total)(struct MEMMAN *man);
				void *(*alloc)(struct MEMMAN *man, uint size);
				int (*free)(struct MEMMAN *man, void *addr, uint size);
				void *(*alloc_4k)(struct MEMMAN *man, uint size);
				int (*free_4k)(struct MEMMAN *man, void *addr, uint size);
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
		void (*putfonts_sht)(struct SHEET32 *sht, int x, int y, uint c, uint bc, const uchar *s);
		void (*init_screen)(void *desktop, void *taskbar, void *mousecursor);
		void (*point)(void *vrami, int x, int y, uint c, int xsize);
		void (*boxfill)(void *vrami, int xsize, uint c, int x0, int y0, int x1, int y1);
		void (*putfonts)(void *vrami, int xsize, int x, int y, uint c, const uchar *s);		
		struct SYS_COLOR {
			uint (*mix)(uint c0, uint c1);
			uchar (*int2char) (uint c32);
			ushort (*int2short) (uint c32);
			void (*pattern)(void *vrami, int xsize, int ysize);		
		} color;
		struct SYS_SHEET {
			void (*init)(void *vram, int xsize, int ysize, uchar bits);
			struct SHEET32 *(*alloc)(void);
			void (*set)(struct SHEET32 *sht, void *buf,int xsize, int ysize, uint col_inv);
			void (*updown)(struct SHEET32 *sht,int height);
			void (*refresh)(struct SHEET32 *sht, int bx0, int by0, int bx1, int by1);
			void (*slide)(struct SHEET32 *sht, int vx0, int vy0);
			void (*free)(struct SHEET32 *sht);
		} sht;
	} draw;
	struct SYS_DATA {
		struct SYS_FIFO {
			void (*init)(struct FIFO32 *fifo, int size, uint *buf, struct TASK *task);
			int (*put)(struct FIFO32 *fifo, uint data);
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
	struct SYS_FILE {
		struct FILEINFO *list;
		ushort *fat;
	} file;
	struct SYS_SYS {
		struct SYS_SYS_SHT {
			struct SHEET32 *core;
			struct SHEET32 *desktop;
			struct SHEET32 *mouse;
			struct SHEET32 *taskbar;
			uint *core_buf;
			uint *desktop_buf;
			uint mouse_buf[24][24];
			uint *taskbar_buf;
		} sht;
		struct SYS_SYS_TIMER {
			struct TIMER *t500;
			struct TIMER *taskswitch;
		} timer;
		struct SYS_SYS_TASK {
			struct TASK *idle;
			struct TASK *main;
		} task;
		struct SYS_SYS_CONSOLE {
			int org_xsize;
			int org_ysize;
			int org_xchars;
			int org_ychars;
		} cons;
		struct MEMMAN memman;
		struct TIMERCTL timctl;
		uint memtotal;
		uint *vram;
		uint xsize;
		uint ysize;
		uchar bpp;
		struct SEGMENT_DESCRIPTOR *gdt;
		struct GATE_DESCRIPTOR *idt;
		struct FIFO32 fifo;
		struct FIFO32 keycmd;
		uint fifo_buf[SYS_FIFOSIZE];
		uint keycmd_buf[KEYCMD_FIFOSIZE];
		int keycmd_wait;
		struct KEYINFO keyinfo;
		struct MOUSE_DECODE mouse_decode;
	} sys;
};

/*externs*/

extern struct SYSTEM *sys_main_str_buf;
extern char cursor[24][24];

/*typedef structures*/

/*functions*/

/*api.c*/
void hrb_api(uint edi, uint esi, uint ebp, uint esp, uint ebx, uint edx, uint ecx, uint eax);

/*file.c*/
void decode_fat(ushort *fat, bool backup);
void load_file(uint finfo_no, uchar *buf);
uint search_file(char *name);

/*console.c*/
void console_main(struct WINDOWINFO *win);
void cons_check_newline(struct WINDOWINFO *win, struct POSITION_2D *p, struct POSITION_2D *prompt);
void cons_put_prompt(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor);
void cons_new_line(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor);
void cons_slide_line(struct WINDOWINFO *win);
void cons_put_str(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, uchar *str);
void cons_reset_cmdline(uchar *cmdline, uint *cmdlines, bool *cmdline_overflow);
void cons_command_start(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, uchar *cmdline, uint *cmdlines, bool *cmdline_overflow);
void cons_new_line_no_prompt(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor);
void cons_put_char(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, uchar c);

/*mtask.c*/
void task_init(void);
struct TASK *task_alloc(void);
void task_run(struct TASK *task, int level, int priority);
void task_switch(void);
void task_sleep(struct TASK *task);
void task_arguments(struct TASK *task, int args, ...);
struct TASK *task_now(void);
void task_add(struct TASK *task);
void task_remove(struct TASK *task);
void task_switchsub(void);
void task_idle(void);

/*io.c*/
void readrtc(uchar *t);
void init_serial(void);
void send_serial(uchar *s);
void fdc_motor_on(uchar d);
void fdc_motor_off(uchar d);
void reset_cpu(void);

/*timer.c*/
void inthandler20(int *esp);
void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, uint data);
void timer_settime(struct TIMER *timer, uint timeout);

/*keyboard.c*/
void init_keyboard(int data0);
void inthandler21(int *esp);
int decode_key(struct KEYINFO *info, int data);
void keylock(int led);
void wait_KBC_sendready(void);

/*mouse.c*/
void init_mouse(int data0);
void inthandler2c(int *esp);
void sendto_mouse(int data);
int decode_mouse(int data);

/*int.c*/
void cpu_exception_alert(int exception, int *esp);
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
struct WINDOWINFO *make_window(uchar *title, int xsize, int ysize, int px, int py, int height, bool active);
void change_window(struct WINDOWINFO *winfo, uchar *title, bool active);
void change_window_title(struct WINDOWINFO *winfo, uchar *title);
void change_window_active(struct WINDOWINFO *winfo, bool active);
void slide_window(struct WINDOWINFO *winfo, int px, int py);
void refresh_window(struct WINDOWINFO *winfo);
void refresh_window_alpha(struct WINDOWINFO *winfo);
void boxfill_win(struct WINDOWINFO *winfo, uint c, int x0, int y0, int x1, int y1);
void putfonts_win(struct WINDOWINFO *winfo, int x, int y, uint c, uint bc, const uchar *s);
void scrool_win(struct WINDOWINFO *winfo);
void line_win(struct WINDOWINFO *winfo, int x0, int y0, int x1, int y1, uint c);
void draw_hexagon_win(struct WINDOWINFO *winfo, int a, int x, int y, uint c);

/*fifo.c*/
void fifo32_init(struct FIFO32 *fifo, int size, uint *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, uint data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

/*memory.c*/
void sys_memman_init(void);
uint sys_memman_free_total(void);
void *sys_memman_alloc(uint size);
int sys_memman_free(void *addr, uint size);
uint memtest(uint start, uint end);
void memman_init(struct MEMMAN *man);
uint memman_free_total(struct MEMMAN *man);
void *memman_alloc(struct MEMMAN *man, uint size);
int memman_free(struct MEMMAN *man, void *addr, uint size);
void *memman_alloc_4k(struct MEMMAN *man, uint size);
int memman_free_4k(struct MEMMAN *man, void *addr, uint size);

/*sheet.c*/
void init_sheets(void *vram, int xsize, int ysize, uchar bits);
struct SHEET32 *sheet_alloc(void);
void sheet_setbuf(struct SHEET32 *sht, void *buf,int xsize, int ysize, uint col_inv);
void sheet_updown(struct SHEET32 *sht, int height);
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
void putfonts_asc_sht_i(struct SHEET32 *sht, int x, int y, uint c, uint bc, const uchar *s);
void init_screen_i(void *desktop, void *taskbar, void *mousecursor);
void init_desktop_i(void *vrami);
void init_taskbar_i(void *vrami);
void init_mousecursor_i(void *vrami);
uint mix_color(uint c0, uint c1);
void point_i(void *vrami, int x, int y, uint c, int xsize);
void boxfill_i(void *vrami, int xsize, uint c, int x0, int y0, int x1, int y1);
void putfonts_asc_i(void *vrami, int xsize, int x, int y, uint c, const uchar *s);
void putblock_i(void *vram, int vxsize, int pxsize, int pysize, int px0, int py0, void *buf, int bxsize);
void line_i(void *vrami, int xsize, int x0, int y0, int x1, int y1, uint c);
void draw_hexagon_i(void *vrami, int xsize, int a, int x, int y, uint c);
void draw_chnos_logo(void *vrami, int xsize, int a, int x, int y);
uchar rgb_int2char (uint c32);
ushort rgb_int2short (uint c32);
void col_pat(void *vrami, int xsize, int ysize);

/*8bits*/
void boxfill8(uchar *vram, int xsize, uchar c, int x0, int y0, int x1, int y1);
void init_desktop8(uchar *vram);
void init_taskbar8(uchar *vram);
void putfont8(uchar *vram, int xsize, int x, int y, uchar c, uchar *font);
void putfonts8_asc(uchar *vram, int xsize, int x, int y, uchar c, const uchar *s);
void init_mouse_cursor8(uchar *mouse);
void putblock8_8(uchar *vram, int vxsize, int pxsize, int pysize, int px0, int py0, uchar *buf, int bxsize);
void init_palette(void);
void set_palette(int start, int end, uchar *rgb);

/*16bits*/
void boxfill16(ushort *vram, int xsize, ushort c, int x0, int y0, int x1, int y1);
void init_desktop16(ushort *vram);
void init_taskbar16(ushort *vram);
void putfont16(ushort *vram, int xsize, int x, int y, ushort c, uchar *font);
void putfonts16_asc(ushort *vram, int xsize, int x, int y, ushort c, const uchar *s);
void init_mouse_cursor16(ushort *mouse);
void putblock16_16(ushort *vram, int vxsize, int pxsize,int pysize, int px0, int py0, ushort *buf, int bxsize);

/*32bits*/
void boxfill32(uint *vram, int xsize, uint c, int x0, int y0, int x1, int y1);
void init_desktop32(uint *vram);
void putfont32(uint *vram, int xsize, int x, int y, uint c, uchar *font);
void putfonts32_asc(uint *vram, int xsize, int x, int y, uint c, const uchar *s);
void init_mouse_cursor32(uint *mouse);
void putblock32_32(uint *vram, int vxsize, int pxsize,int pysize, int px0, int py0, uint *buf, int bxsize);

/*gdtidt.c*/
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, uint limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
void init_taskbar32(uint *vram);


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
uint memtest_sub(uint start, uint end);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app(void);
void asm_hrb_api(void);

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
