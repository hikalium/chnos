
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
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
uint io_in8(uint port);
void io_out8(uint port, uint data);
uint io_in16(uint port);
void io_out16(uint port, uint data);
uint io_in32(uint port);
void io_out32(uint port, uint data);
uint io_load_eflags(void);
void io_store_eflags(uint eflags);
uint load_cr0(void);
void store_cr0(uint cr0);
uint load_cr1(void);
void store_cr1(uint cr1);
uint load_cr2(void);
void store_cr2(uint cr2);
uint load_cr3(void);
void store_cr3(uint cr3);
uint load_cr4(void);
void store_cr4(uint cr4);

