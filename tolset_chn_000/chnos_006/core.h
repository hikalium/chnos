/*include files*/

#include <stdio.h>

/*�֐��I��`*/

#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))


/*�ݒ萔�l�̒�`*/

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

#define SHT_FLAGS_VOID	0
#define SHT_FLAGS_USE	1

#define WIN_FLAGS_VOID	0
#define WIN_FLAGS_USE	1

#define SYSFIFO_KEYB	0x100			/*256~511=keycode*/
#define SYSFIFO_MOUSE	0x200			/*512~767=mouse*/

#define INV_COL32	0xFFFFFFFF
#define VOID_INV_COL32	0xFEFFFFFF

#define DESKTOP_COL8	COL8_C6C6C6
#define TASKBAR_COL8	COL8_0000FF

#define DESKTOP_COL16	RGB16(17,33,17)		/*�����l�F10,20,10*/
#define TASKBAR_COL16	RGB16(20,40,30)		/*�����l�F20,40,30*/

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



/*�\���̐錾*/
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
	unsigned int *vram;
	int xsize,ysize,top;
	struct SHEET32 *sheets[MAX_SHEETS];
	struct SHEET32 sheets0[MAX_SHEETS];
};

struct MOUSE_DECODE {
	unsigned int buf[3];
	int x,y,btn;
	unsigned char phase; 

};

struct BOOTINFO { /* 0x0ff0-0x0fff �W��*/
	char cyls; /* �u�[�g�Z�N�^�͂ǂ��܂Ńf�B�X�N��ǂ񂾂̂� */
	char leds; /* �u�[�g���̃L�[�{�[�h��LED�̏�� */
	char vmode; /* �r�f�I���[�h  ���r�b�g�J���[�� */
	char reserve;
	short scrnx, scrny; /* ��ʉ𑜓x */
	char *vram;
};

struct VESAINFO {/*0xe00--->512byte �W��*/
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

struct SEGMENT_DESCRIPTOR { /*0x270000~0x27ffff �W��*/
	short limit_low,base_low;
	char base_mid,access_right;
	char limit_high,base_high;
};
struct GATE_DESCRIPTOR { /*0x26f800~0x26ffff �W��*/
	short offset_low,selector;
	char dw_count,access_right;
	short offset_high;
};

struct WINDOW {
	void (*init)(void);
};

struct MEMORY {
	void (*init)(struct MEMMAN *man);
};

struct SYSTEM {
	struct WINDOW window;
	struct MEMORY memory;
};

/*�֐��錾*/


/*bootpack.c	�n�r���C��*/

/*system.c	�֐��|�C���^�֌W�Asystem�\���̂̊Ǘ�*/
void init_system(struct SYSTEM *system);

/*io.c		���̑��O�����u�֌W*/

void readrtc(unsigned char *t);
void wait_KBC_sendready(void);

/*int.c		���荞�݊֌W�o�h�b��*/

void init_pic(void);

/*memory.c	�������Ǘ��֌W*/

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_free_total(void);
unsigned int memman_alloc(unsigned int size);
int memman_free(unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(unsigned int size);
int memman_free_4k(unsigned int addr, unsigned int size);

/*window.c	�E�B���h�E�֌W*/
void init_windows(void);
struct WINDOWINFO *window_alloc(void);
struct WINDOWINFO *make_window32(unsigned int *buf, unsigned char *title, int xsize, int ysize, int px, int py, int height);
void slide_window(struct WINDOWINFO *winfo, int px, int py);

/*sheet.c	��ʊǗ��֌W*/

void init_sheets(unsigned int *vram, int xsize, int ysize);
struct SHEET32 *sheet_alloc(void);
void sheet_setbuf(struct SHEET32 *sht,unsigned int *buf,int xsize, int ysize, unsigned int col_inv );
void sheet_updown(struct SHEET32 *sht,int height);
void sheet_refresh(struct SHEET32 *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET32 *sht, int vx0, int vy0);
void sheet_free(struct SHEET32 *sht);
void sheet_refreshsub(int vx0, int vy0, int vx1, int vy1);

/*keyboard.c	�L�[�{�[�h�֌W*/
void init_keyboard(struct FIFO32 *fifo, int data0);
void inthandler21(int *esp);

/*mouse.c	�}�E�X�֌W*/

void inthandler2c(int *esp);
void init_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DECODE *mdec0);
int decode_mouse (unsigned int dat);


/*timer.c		�^�C�}�[�֌W*/
void init_pit(volatile int *time_tick);
void inthandler20(int *esp);

/*fifo.c		FIFO�o�b�t�@�֌W*/

void fifo32_init(struct FIFO32 *fifo, int size, unsigned int *buf);
int fifo32_put(struct FIFO32 *fifo, unsigned int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

/*gdtidt.c		���荞�݁A�Z�O�����g�e�[�u����*/

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

/*graphic.h	�O���t�B�b�N�֌W*/

/*�S�F�Ή�*/

void circle_i(unsigned int *vrami, int cx, int cy, unsigned int c, int xsize, int r);
unsigned short rgb_int2short (unsigned int c32);
unsigned char rgb_int2char(unsigned int c32);
void init_scrn_i(unsigned int *vram, int xsize, int ysize, unsigned char bits, unsigned int *mousecur32);
void boxfill_i(unsigned int *vrami, int xsize, unsigned int c, int x0, int y0, int x1, int y1);
void col_pat_256safe(unsigned int *vrami, int xsize, int ysize);
void putfonts_asc_i(unsigned int *vrami, int xsize, int x, int y, unsigned int ci, unsigned char *s);
void point_i(unsigned int *vrami, int x, int y, unsigned int c32, int xsize);

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

