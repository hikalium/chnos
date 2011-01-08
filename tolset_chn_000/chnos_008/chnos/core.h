
/*include files*/
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

int rand(void);
#define srand(seed)			(void) (rand_seed = (seed))
extern unsigned int rand_seed;

/*definemacros*/
#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))

/*settings*/
#define system	(*sys_main_str_buf)

#define ADR_BOOTINFO	0x00000ff0
#define ADR_VESAINFO	0x00000e00
#define ADR_DISKIMG	0x00100000

#define EFLAGS_AC_BIT	0x00040000

#define CR0_PROTECTIONENABLE	0x00000001
#define CR0_MONITORCOPROCESSOR	0x00000002
#define CR0_EMULATION		0x00000004
#define CR0_TASKSWITCH		0x00000008
#define CR0_EXTENDEDTYPE	0x00000010
#define CR0_NUMERICERROR	0x00000020
#define CR0_WRITEPROTECT	0x00010000
#define CR0_ALIGNMENTMASK	0x00040000
#define CR0_NOTWRITETHROUGH	0x20000000
#define CR0_CACHE_DISABLE	0x40000000
#define CR0_PAGING		0x80000000

#define CR0_ALL_CACHE_DISABLE	CR0_NOTWRITETHROUGH + CR0_CACHE_DISABLE

#define PG_NOTPRESENT	0x00000000
#define PG_PRESENT	0x00000001
#define PG_READONLY	0x00000000
#define PG_WRITABLE	0x00000002
#define PG_SUPERVISOR	0x00000000
#define PG_USER		0x00000004
#define PG_WRITEBACK	0x00000000
#define PG_WRITETHROUGH	0x00000008
#define PG_CACHE_ENABLE	0x00000000
#define PG_CACHE_DISABLE	0x00000010
#define PG_NOTACCESSED	0x00000000
#define PG_ACCESSED	0x00000020
#define PG_NOTWRITTEN	0x00000000
#define PG_WRITTEN	0x00000040
#define PG_4KBPAGE	0x00000000
#define PG_4MBPAGE	0x00000080
#define PG_NOTGLOBAL	0x00000000
#define	PG_GLOBAL	0x00000100

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
#define AR_APP		0x60

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

#define MEMMAN_FREES	4000

#define INV_COL32	0xFFFFFFFF
#define INV_COL16	0x1192
#define INV_COL8	0xFF

#define MAX_SHEETS	1024

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

#define MAX_TASKS	1000
#define TASK_GDT_START	3
#define MAX_LEVEL_TASKS	100
#define MAX_LEVELS	10

#define MAX_TIMER	512

#define FIFO32_PUT_OVERFLOW	0x0001

#define SYS_FIFOSIZE	256
#define KEYCMD_FIFOSIZE	128
#define KEYCTRL_FIFOSIZE	128
#define MOUSECTRL_FIFOSIZE	128

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
#define KEYCMD_LED	0xed

#define MAX_WINDOWS	256
#define WIN_COL8_ACTIVE		0x000084
#define WIN_COL8_INACTIVE	0x848484
#define WIN_COL16_ACTIVE	0x60F8f0
#define WIN_COL16_INACTIVE	0xBCFCF8
#define WIN_COL32_ACTIVE	0x93D9FF
#define WIN_COL32_INACTIVE	0xD0EFFF

#define CONSOLE_FIFO_BUF_SIZE	128
#define CONSOLE_FIFO_START_KEYB	128
#define CONSOLE_FIFO_CURSOR_START	2
#define CONSOLE_FIFO_CURSOR_STOP	3
#define CONSOLE_COLOR_BACKGROUND	0x000000
#define CONSOLE_COLOR_CHAR	0xFFFFFF
#define CONSOLE_CMDLINE_BUF_SIZE	128
#define MAX_CONSOLES	16

#define DATA_BYTE	0xFF
#define DATA_WORD	0xFFFF
#define DATA_DWORD	0xFFFFFFFF

/*new object types*/
typedef enum _bool { false, true} bool;
typedef enum _state_alloc { none, initialized, allocated, configured, inuse} state_alloc;
typedef enum _col_text { black, blue, green, skyblue, red, purple, brown, white} col_text;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

/*structures*/
struct BOOTINFO { 
	uchar cyls; 
	uchar leds; 
	uchar vmode; 
	uchar reserve;
	ushort scrnx, scrny;
	uchar *vram;
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

struct MEMMAN {
	int frees,maxfrees,lostsize,losts;
	struct MEM_FREEINFO {
		uint addr, size;
	} free[MEMMAN_FREES];
};

struct SHEET {
	void *buf;
	uint col_inv;
	int bxsize, bysize, vx0, vy0, height;
	state_alloc flags; 
};

struct SHTCTL {
	uint *map;
	void *vram;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct FIFO32 {
	uint *buf;
	uint p, q, size, free, flags;
	struct TASK *task;
};

struct TASK {
	int selector;
	state_alloc flags;
	int level, priority;
	struct FIFO32 fifo;
	struct TSS32 tss;
};

struct TASKCTL {
	int level_now;
	bool change_lv_next;
	struct TASKLEVEL {
		int running_tasks;
		int task_now;
		struct TASK *tasks[MAX_LEVEL_TASKS];
	} level[MAX_LEVELS];
	struct TASK tasks0[MAX_TASKS];
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

struct KEYINFO {
	uchar c;
	uint keycode;
	bool make;
	bool alphabet;
};

struct MOUSE_DECODE {
	uint buf[4], scrool;
	int x, y, btn, whinfo;
	uchar phase; 
};

struct POSITION_2D {
	int x, y;
};

struct WINDOWINFO {
	char title[32];
	struct SHEET *win;
	void *buf;
	int winxsize, winysize, xsize, ysize;
	struct POSITION_2D position;
	struct POSITION_2D origin;
	state_alloc flags;
	bool active;
	void *app_buf;
	uchar app_buf_bits;
	struct TASK *task;
};

struct WINCTL {
	struct WINDOWINFO winfos[MAX_WINDOWS];
};

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

struct SYS_UI_CONSOLES {
	struct TASK *task;
	struct WINDOWINFO *win;
	struct POSITION_2D prompt;
	struct POSITION_2D cursor;
	uchar *app_cs_base;
	uchar *app_ds_base;
	struct TIMER *timer;
	bool cursor_state;
	bool cursor_on;
	uint cursor_c;
};

struct _UUID {
	uchar data[16];
};

struct FORMAT_HRB {
	uint DataSegmentSize;
	uchar sign[4];
	uint DataSegmentExtSize;
	uint DefaultESP;
	uint TransferToDataSegBytes;
	uint OriginDataSection;
	uint JMPCode;
	uint EntryPoint;
	uint StartMallocAddr;
};

/*typedef structures*/
typedef struct BOOTINFO			DATA_BootInfo;
typedef struct VESAINFO			DATA_VESAInfo;
typedef struct SEGMENT_DESCRIPTOR	IO_SegmentDescriptor;
typedef struct GATE_DESCRIPTOR		IO_GateDescriptor;
typedef struct MEMMAN			IO_MemoryControl;
typedef struct SHEET			UI_Sheet;
typedef struct SHTCTL			UI_SheetControl;
typedef struct FIFO32			DATA_FIFO;
typedef struct TSS32			IO_TaskStatusSegment32;
typedef struct TASK			UI_Task;
typedef struct TASKCTL			UI_TaskControl;
typedef struct TIMER			UI_Timer;
typedef struct TIMERCTL			UI_TimerControl;
typedef struct KEYINFO			UI_KeyInfo;
typedef struct MOUSE_DECODE		UI_Mouse;
typedef struct POSITION_2D		DATA_Position2D;
typedef struct WINDOWINFO		UI_Window;
typedef struct WINCTL			UI_WindowControl;
typedef struct FILEINFO			IO_FileInfo;
typedef struct SYS_UI_CONSOLES		UI_Console;
typedef struct _UUID			UUID;
typedef struct FORMAT_HRB		FORMAT_Haribote;

/*virtual classes*/
struct SYSTEM {
	struct SYS_IO {
		struct SYS_IO_MEM {
			struct SYS_IO_MEM_PAGING {
				uint *dir;
				uint (*table)[1024];
			} paging;
			struct SYS_IO_MEM_SEGMENT {
				IO_SegmentDescriptor *gdt;
			} segment;
			uint total;
			IO_MemoryControl ctrl;
		} mem;
		struct SYS_IO_INTERRUPT {
			IO_GateDescriptor *idt;
		} interrupt;
		struct SYS_IO_KEYBOARD {
			int cmd_wait;
		} keyboard;
		struct SYS_IO_MOUSE {
			UI_Mouse decode;
		} mouse;
		struct SYS_IO_FILE {
			IO_FileInfo *list;
			ushort *fat;
		} file;
	} io;
	struct SYS_UI {
		struct SYS_UI_DRAW {
			struct SYS_UI_DRAW_SHT {
				UI_SheetControl ctrl;
				UI_Sheet *core;
				UI_Sheet *desktop;
				UI_Sheet *taskbar;
				UI_Sheet *mouse;
				void *core_buf;
				void *desktop_buf;
				void *taskbar_buf;
				void *mouse_buf;
			} sht;
		} draw;
		struct SYS_UI_TASK {
			UI_Task *idle;
			UI_Task *main;
			UI_Task *keyctrl;
			UI_Task *mousectrl;
		} task;
		struct SYS_UI_TIMER {
			UI_Timer *taskswitch;
			UI_TimerControl ctrl;
		} timer;
		struct SYS_UI_CONSOLE {
			int org_xsize;
			int org_ysize;
			int org_xchars;
			int org_ychars;
			UI_Console consoles[MAX_CONSOLES];
		} console;
		struct SYS_UI_WINDOW {
			UI_WindowControl ctrl;
		} window;
	} ui;
	struct SYS_DATA {
		struct SYS_DATA_INFO {
			DATA_BootInfo boot;
			DATA_VESAInfo vesa;
		} info;
		struct SYS_DATA_FIFO {
			DATA_FIFO main;
			uint main_buf[SYS_FIFOSIZE];
			DATA_FIFO keycmd;
			uint keycmd_buf[KEYCMD_FIFOSIZE];
			DATA_FIFO keyctrl;
			uint keyctrl_buf[MOUSECTRL_FIFOSIZE];
			DATA_FIFO mousectrl;
			uint mousectrl_buf[MOUSECTRL_FIFOSIZE];
		} fifo;
	} data;
};

/*externs*/
extern struct SYSTEM *sys_main_str_buf;
extern uchar hankaku[4096];
extern char cursor[24][24];
extern uint rgb_int2char_list[16];
extern uint key_shift;

/*functions*/
/*bootpack.c*/
void KeyBoardControlTask(void);
void MouseControlTask(void);

/*api.c*/
uint hrb_api(uint edi, uint esi, uint ebp, uint esp, uint ebx, uint edx, uint ecx, uint eax);
uint GetWindowNumber(UI_Window *win);
UI_Window *GetWindowInfo(uint n);

/*uuid.c*/
void gen_UUID(UUID *uuid);

/*file.c*/
void decode_fat(ushort *fat, bool backup);
void load_file(uint finfo_no, uchar *buf);
uint search_file(char *name);

/*console.c*/
void console_main(UI_Console *cons);
void cons_reset_cmdline(uchar *cmdline, uint *cmdlines, bool *cmdline_overflow);
void cons_command_start(UI_Console *cons, uchar *cmdline, uint *cmdlines, bool *cmdline_overflow);
void cons_command_mem(UI_Console *cons);
void cons_command_dir(UI_Console *cons);
void cons_command_fdc(UI_Console *cons, uchar *cmdline);
void cons_command_type(UI_Console *cons, uchar *cmdline);
void cons_command_cpuid(UI_Console *cons, uchar *cmdline);
void cons_command_systeminfo(UI_Console *cons, uchar *cmdline);
uint cons_app_hrb_start(UI_Console *cons, uchar *cmdline);
void cons_put_str(UI_Console *cons, uchar *str);
void cons_put_char(UI_Console *cons, uchar c);
void cons_put_prompt(UI_Console *cons);
void cons_new_line(UI_Console *cons);
void cons_new_line_no_prompt(UI_Console *cons);
void cons_slide_line(UI_Console *cons);
void cons_check_newline(UI_Console *cons);

/*window.c*/
void init_windows(void);
UI_Window *window_alloc(void);
void window_free(UI_Window *winfo);
UI_Window *make_window(uchar *title, int xsize, int ysize, int px, int py, int height, bool active);
UI_Window *make_window_app(uchar *title, int xsize, int ysize, int px, int py, int height, bool active, uint *buf, UI_Task *task);
UI_Window *make_window_app_compatible_hrb(uchar *title, int xsize, int ysize, int px, int py, int height, bool active, uint *buf, UI_Task *task);
void free_window_app(UI_Window *winfo);
void change_window(UI_Window *winfo, uchar *title, bool active);
void change_window_title(UI_Window *winfo, uchar *title);
void change_window_active(UI_Window *winfo, bool active);
void slide_window(UI_Window *winfo, int px, int py);
void refresh_window(UI_Window *winfo);
void refresh_window_alpha(UI_Window *winfo);
void boxfill_win(UI_Window *winfo, uint c, int x0, int y0, int x1, int y1);
void point_win(UI_Window *winfo, uint c, int x, int y);
void putfonts_win(UI_Window *winfo, int x, int y, uint c, uint bc, const uchar *s);
void putfonts_win_no_bc(UI_Window *winfo, int x, int y, uint c, const uchar *s);
void scrool_win(UI_Window *winfo);
void scrool_win_32(UI_Window *winfo, uint *vram);
void scrool_win_16(UI_Window *winfo, ushort *vram);
void scrool_win_8(UI_Window *winfo, uchar *vram);
void line_win(UI_Window *winfo, int x0, int y0, int x1, int y1, uint c);
void draw_hexagon_win(UI_Window *winfo, int a, int x, int y, uint c);


/*mouse.c*/
void init_mouse(uint offset);
void inthandler2c(int *esp);
int decode_mouse(uint data);
void sendto_mouse(uint data);

/*keyboard.c*/
void init_keyboard(uint offset);
void inthandler21(int *esp);
void decode_key(UI_KeyInfo *info, uint data);
void keylock(uint led);
void wait_KBC_sendready(void);

/*fifo.c*/
void fifo32_init(DATA_FIFO *fifo, uint size, uint *buf, UI_Task *task);
int fifo32_put(DATA_FIFO *fifo, uint data);
uint fifo32_get(DATA_FIFO *fifo);
uint fifo32_status(DATA_FIFO *fifo);

/*mtask.c*/
void task_init(void);
UI_Task *task_alloc(void);
void task_run(UI_Task *task, int level, int priority);
void task_switch(void);
void task_sleep(UI_Task *task);
void task_arguments(UI_Task *task, int args, ...);
UI_Task *task_now(void);
void task_add(UI_Task *task);
void task_remove(UI_Task *task);
void task_switchsub(void);
void task_idle(void);

/*timer.c*/
void inthandler20(int *esp);
void init_pit(void);
UI_Timer *timer_alloc(void);
void timer_free(UI_Timer *timer);
void timer_init(UI_Timer *timer, DATA_FIFO *fifo, uint data);
void timer_settime(UI_Timer *timer, uint timeout);
void timer_settime_millisec(UI_Timer *timer, uint timeout_ms);

/*gdtidt.c*/
void init_gdtidt(void);
void set_segmdesc(IO_SegmentDescriptor *sd, uint limit, int base, int ar);
void set_gatedesc(IO_GateDescriptor *gd, int offset, int selector, int ar);

/*graphic.c grap_08.c grap_16.c grap_32.c*/
/*All*/
void init_screen_i(void *desktop, void *taskbar, void *mousecursor);
void init_desktop_i(void *vrami);
void init_taskbar_i(void *vrami);
void init_mousecursor_i(void *vrami);
uint mix_color(uint c0, uint c1);
void point_i(void *vrami, int x, int y, uint c, int xsize);
void point_bpp(void *vrami, int x, int y, uint c, int xsize, uint bpp);
void boxfill_i(void *vrami, int xsize, uint c, int x0, int y0, int x1, int y1);
void boxfill_bpp(void *vrami, int xsize, uint c, int x0, int y0, int x1, int y1, uint bpp);
void putfonts_asc_i(void *vrami, int xsize, int x, int y, uint c, const uchar *s);
void putblock_i(void *vrami, int vxsize, int pxsize, int pysize, int px0, int py0, void *buf, int bxsize);
void putblock_i_convert(void *to, int xsize, int px0, int py0, int px1, int py1, void *from, int tobpp, int frombpp);
void line_i(void *vrami, int xsize, int x0, int y0, int x1, int y1, uint c);
void line_bpp(void *vrami, int xsize, int x0, int y0, int x1, int y1, uint c, uint bpp);
void draw_hexagon_i(void *vrami, int xsize, int a, int x, int y, uint c);
void draw_chnos_logo(void *vrami, int xsize, int a, int x, int y);
uchar rgb_int2char (uint c32);
ushort rgb_int2short (uint c32);
void col_pat(void *vrami, int xsize, int ysize);
void putfonts_asc_sht_i(UI_Sheet *sht, int x, int y, uint c, uint bc, const uchar *s);
void putfonts_asc_sht_i_no_bc(UI_Sheet *sht, int x, int y, uint c, const uchar *s);

/*08Bits*/
void boxfill8(uchar *vram, int xsize, uchar c, int x0, int y0, int x1, int y1);
void init_desktop8(uchar *vram, uint xsize, uint ysize);
void init_taskbar8(uchar *vram, uint xsize);
void putfont8(uchar *vram, int xsize, int x, int y, uchar c, uchar *font);
void putfonts8_asc(uchar *vram, int xsize, int x, int y, uchar c, const uchar *s);
void init_mouse_cursor8(uchar *mouse);
void putblock8_8(uchar *vram, int vxsize, int pxsize,int pysize, int px0, int py0, uchar *buf, int bxsize);
void init_palette(void);
void set_palette(int start, int end, uchar *rgb);
/*16Bits*/
void boxfill16(ushort *vram, int xsize, ushort c, int x0, int y0, int x1, int y1);
void init_desktop16(ushort *vram, uint xsize, uint ysize);
void init_taskbar16(ushort *vram, uint xsize);
void putfont16(ushort *vram, int xsize, int x, int y, ushort c, uchar *font);
void putfonts16_asc(ushort *vram, int xsize, int x, int y, ushort c, const uchar *s);
void init_mouse_cursor16(ushort *mouse);
void putblock16_16(ushort *vram, int vxsize, int pxsize,int pysize, int px0, int py0, ushort *buf, int bxsize);
/*32Bits*/
void boxfill32(uint *vram, int xsize, uint c, int x0, int y0, int x1, int y1);
void init_desktop32(uint *vram, uint xsize, uint ysize);
void init_taskbar32(uint *vram, uint xsize);
void putfont32(uint *vram, int xsize, int x, int y, uint c, uchar *font);
void putfonts32_asc(uint *vram, int xsize, int x, int y, uint c, const uchar *s);
void init_mouse_cursor32(uint *mouse);
void putblock32_32(uint *vram, int vxsize, int pxsize,int pysize, int px0, int py0, uint *buf, int bxsize);

/*init.c*/
void init_system(void);

/*intrpt.c*/
void init_pic(void);
void inthandler27(int *esp);
void cpu_exception_abort(int exception, int *esp);
uint cpu_exception_fault(int exception, int *esp);
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

/*io.c*/
void init_serial(void);
void send_serial(uchar *s);
uint readcmos(uchar addr);
void readrtc(uchar *t);
void fdc_motor_on(uchar d);
void fdc_motor_off(uchar d);
void reset_cpu(void);

/*memory.c*/
uint memtest(uint start, uint end);
void memman_init(IO_MemoryControl *man);
uint memman_free_total(IO_MemoryControl *man);
void *memman_alloc(IO_MemoryControl *man, uint size);
int memman_free(IO_MemoryControl *man, void *addr0, uint size);
void *memman_alloc_page(IO_MemoryControl *man);
void sys_memman_init(void);
uint sys_memman_free_total(void);
void *sys_memman_alloc(uint size);
int sys_memman_free(void *addr, uint size);
void *sys_memman_alloc_page(void);

/*paging.c*/
void init_paging(void);
void paging_set_dir(uint *dir_entry, uint *table_base, uint attribute, uint available);
void paging_set_table(uint *table_entry, uint *page_base, uint attribute, uint available);

/*sheet.c*/
void init_sheets(void *vram, int xsize, int ysize, uchar bits);
UI_Sheet *sheet_alloc(void);
void sheet_setbuf(UI_Sheet *sht, void *buf,int xsize, int ysize, uint col_inv);
void sheet_updown(UI_Sheet *sht, int height);
void sheet_refresh(UI_Sheet *sht, int bx0, int by0, int bx1, int by1);
void sheet_refresh_full_alpha(UI_Sheet *sht);
void sheet_refresh_full(UI_Sheet *sht);
void sheet_slide(UI_Sheet *sht, int vx0, int vy0);
void sheet_free(UI_Sheet *sht);
void sheet_refreshsub32(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshsub16(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshsub8(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshmap32(int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_refreshmap16(int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_refreshmap8(int vx0, int vy0, int vx1, int vy1, int h0);
int sheet_get_topheight(void);

/* naskfunc.nas */
void cpuid(void *addr, uint idaddr);
void read_tsc(uint *addr);
void pipelineflush(void);
void pit_beep_on(void);
void pit_beep_off(void);
void clts(void);
void fnsave(uint *addr);
void frstore(uint *addr);
void asm_inthandler07(void);
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
uint io_in8(uint port);
uint io_in16(uint port);
uint io_in32(uint port);
void io_out8(uint port, uint data);
void io_out16(uint port, uint data);
void io_out32(uint port, uint data);
uint io_load_eflags(void);
void io_store_eflags(uint eflags);
void load_gdtr(uint limit, uint addr);
void load_idtr(uint limit, uint addr);
uint load_cr0(void);
void store_cr0(uint cr0);
uint load_cr1(void);
void store_cr1(uint cr1);
uint load_cr2(void);
void store_cr2(uint cr2);
uint load_cr3(void);
void store_cr3(uint cr3);
void load_tr(uint tr);
uint memtest_sub(uint start, uint end);
void farjmp(uint eip, uint cs);
void farcall(uint eip, uint cs);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app(void);
void asm_hrb_api(void);
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
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
