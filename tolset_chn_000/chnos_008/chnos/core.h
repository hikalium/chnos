
/*include files*/
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

/*definemacros*/
#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))

/*settings*/
#define system	(*sys_main_str_buf)

#define ADR_BOOTINFO	0x00000ff0
#define ADR_VESAINFO	0x00000e00

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

/*new object types*/
typedef enum _bool { false, true} bool;
typedef enum _state_alloc { none, initialized, allocated, configured, inuse} state_alloc;
typedef enum _col_text { black, blue, green, skyblue, red, purple, brown, white} col_text;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

/*structures*/
struct BOOTINFO { 
	char cyls; 
	char leds; 
	char vmode; 
	char reserve;
	short scrnx, scrny;
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

/*typedef structures*/
typedef struct BOOTINFO			DATA_BootInfo;
typedef struct VESAINFO			DATA_VESAInfo;
typedef struct SEGMENT_DESCRIPTOR	IO_SegmentDescriptor;
typedef struct GATE_DESCRIPTOR		IO_GateDescriptor;
typedef struct MEMMAN			IO_MemoryControl;

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
	} io;
	struct SYS_DATA {
		struct SYS_DATA_INFO {
			DATA_BootInfo boot;
			DATA_VESAInfo vesa;
		} info;
	} data;
};

/*externs*/
extern struct SYSTEM *sys_main_str_buf;
extern uchar hankaku[4096];
extern char cursor[24][24];

/*functions*/
/*bootpack.c*/

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
void boxfill_i(void *vrami, int xsize, uint c, int x0, int y0, int x1, int y1);
void putfonts_asc_i(void *vrami, int xsize, int x, int y, uint c, const uchar *s);
void putblock_i(void *vrami, int vxsize, int pxsize, int pysize, int px0, int py0, void *buf, int bxsize);
void line_i(void *vrami, int xsize, int x0, int y0, int x1, int y1, uint c);
void draw_hexagon_i(void *vrami, int xsize, int a, int x, int y, uint c);
void draw_chnos_logo(void *vrami, int xsize, int a, int x, int y);
uchar rgb_int2char (uint c32);
ushort rgb_int2short (uint c32);
void col_pat(void *vrami, int xsize, int ysize);
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
void cpu_exception_abort(int exception, int *esp);
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


/* naskfunc.nas */
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
