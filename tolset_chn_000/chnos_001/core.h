

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

/* asmhead.nas */
struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; /* �u�[�g�Z�N�^�͂ǂ��܂Ńf�B�X�N��ǂ񂾂̂� */
	char leds; /* �u�[�g���̃L�[�{�[�h��LED�̏�� */
	char vmode; /* �r�f�I���[�h  ���r�b�g�J���[�� */
	char reserve;
	short scrnx, scrny; /* ��ʉ𑜓x */
	char *vram;
};
#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG		0x00100000

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

