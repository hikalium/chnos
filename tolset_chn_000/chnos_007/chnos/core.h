
/*include files*/

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

#define MEMMAN_FREES	4096

#define MAX_SHEETS	1024

#define MAX_WINDOWS	256

#define MAX_TIMER	512

#define EFLAGS_AC_BIT	0x00040000
#define CR0_CACHE_DISABLE	0x60000000

/*structures*/

extern char cursor[24][24];

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

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
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

struct BOOTINFO { /* 0x0ff0-0x0fff*/
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
			unsigned int (*alloc)(unsigned int size);
			int (*free)(unsigned int addr, unsigned int size);
			unsigned int (*test)(unsigned int start, unsigned int end);
			unsigned int (*test_sub)(unsigned int start, unsigned int end);
			struct SYS_MEMORY_ORG {
				void (*init)(struct MEMMAN *man);
				unsigned int (*free_total)(struct MEMMAN *man);
				unsigned int (*alloc)(struct MEMMAN *man, unsigned int size);
				int (*free)(struct MEMMAN *man, unsigned int addr, unsigned int size);
				unsigned int (*alloc_4k)(struct MEMMAN *man, unsigned int size);
				int (*free_4k)(struct MEMMAN *man, unsigned int addr, unsigned int size);
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
		void (*putfonts_sht)(struct SHEET32 *sht, int x, int y, unsigned int c, unsigned int bc, const char *s);
		void (*init_screen)(unsigned int *vrami, int xsize, int ysize, unsigned char bits, unsigned int *mousecur);
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
	} draw;
	struct SYS_INFOS {
		struct BOOTINFO	boot;
		struct VESAINFO vesa;	
	} info;
	struct SYS_APP {
		void (*start)(int eip, int cs, int esp, int ds, int *tss_esp0);
		void (*end)(void);
	} app;
	struct SYS_SYS {
		struct MEMMAN memman;
		unsigned int memtotal;
		unsigned int mousecursor[24][24];
		unsigned int *vram;
		unsigned int xsize;
		unsigned int ysize;
		unsigned char bpp;
	} sys;
};

extern struct SYSTEM system;

/*functions*/

/*system.c*/
void init_system(void);

/*memory.c*/
void sys_memman_init(void);
unsigned int sys_memman_free_total(void);
unsigned int sys_memman_alloc(unsigned int size);
int sys_memman_free(unsigned int addr, unsigned int size);
unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_free_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);

/*graphic.c grap_08.c grap_16.c grap_32.c*/

/*All*/
void putfonts_asc_sht_i(struct SHEET32 *sht, int x, int y, unsigned int c, unsigned int bc, const char *s);
void init_scrn_i(unsigned int *vrami, int xsize, int ysize, unsigned char bits, unsigned int *mousecur);
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
