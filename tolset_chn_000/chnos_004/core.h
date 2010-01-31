/*関数的定義*/

#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))


/*単なる数値の定義*/

#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG	0x00100000


/* asmhead.nas */

struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; /* ブートセクタはどこまでディスクを読んだのか */
	char leds; /* ブート時のキーボードのLEDの状態 */
	char vmode; /* ビデオモード  何ビットカラーか */
	char reserve;
	short scrnx, scrny; /* 画面解像度 */
	char *vram;
};

struct VESAINFO {/*0xe00--->512byte*/
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
	unsigned short *PhysBasePtr;
};


/*bootpack.c*/
void boxfill16(unsigned short *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1);
void init_scrn(unsigned short *vram, int xsize, int ysize);
void putfonts16_asc(unsigned short *vram, int xsize, int x, int y, unsigned short c, unsigned char *s);
void putfont16(unsigned short *vram, int xsize, int x, int y, unsigned short c, char *font);
void init_mouse_cursor16(short *mouse, unsigned short bc);
void putblock16_16(unsigned short *vram, int vxsize, int pxsize,int pysize, int px0, int py0, short *buf, int bxsize);
void readrtc(unsigned char *t);


/* naskfunc.nas */
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

