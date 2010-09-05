
#include "core.h"

int xsize;

void init_pic(void)
{
	xsize = system.info.boot.scrnx;
	/*
	IRR�F�C���^���v�g���N�G�X�g���W�X�^ 
		.IRQ�s���̏�Ԃ�\���B�|�[�g��ǂ񂾂Ƃ��ɁAIRR�ɂȂ邩ISR�ɂȂ邩�́AOCW3�őI������B
		.1�ɂȂ��Ă���r�b�g�́A���ݗv�������Ă���i�������͏������́j���荞�݁B
	ISR�F�C���T�[�r�X���W�X�^ 
		.���ݏ������̊��荞�݂��ǂ�ł��邩�������B�|�[�g��ǂ񂾂Ƃ��ɁAIRR�ɂȂ邩ISR�ɂȂ邩�́AOCW3�őI������B
		.1�ɂȂ��Ă���r�b�g�́A���ݏ������̊��荞�݁B�������Ƃ����̂́ACPU�ɑ΂���INT���߂𔭍s�������AEOI�i���荞�ݏI�����߁j���󂯎���Ă��Ȃ����荞�݁B
	IMR�F�C���^���v�g�}�X�N���W�X�^ 
		.���ꂪ1�ɂȂ��Ă��銄�荞�݂́AIRR��1�ɂȂ��Ă��Ă��A�������Ȃ��B 
	*/

	io_out8(PIC0_IMR, 0xff);	/*���荞�ݑS�������i�}�X�^�j*/
	io_out8(PIC1_IMR, 0xff);	/*���荞�ݑS�������i�X���[�u�j*/

	io_out8(PIC0_ICW1, 0x11);	/*�G�b�W�g���K���[�h�ɐݒ�i�}�X�^�j*/
	io_out8(PIC0_ICW2, 0x20);	/*���荞�ݔԍ����A20~27�ɐݒ�B�i�}�X�^�j*/
	io_out8(PIC0_ICW3, 1 << 2);	/*00000100 �܂�A�X���[�u��IRQ2�ɂȂ����Ă܂���Ƃ������ƁB*/
	io_out8(PIC0_ICW4, 0x01);	/*�m���o�b�t�@���[�h�i�}�X�^�j*/

	io_out8(PIC1_ICW1, 0x11);	/*�G�b�W�g���K���[�h�ɐݒ�i�X���[�u�j*/
	io_out8(PIC1_ICW2, 0x28);	/*���荞�ݔԍ����A28~2f�ɐݒ�B�i�X���[�u�j*/
	io_out8(PIC1_ICW3, 2);	/*�����̓}�X�^��IRQ2�ԂɂȂ����Ă܂��Ƃ������ƁB*/
	io_out8(PIC1_ICW4, 0x01);	/*�m���o�b�t�@���[�h�i�X���[�u�j*/

	io_out8(PIC0_IMR, 0xfb);	/*11111011�܂�AIRQ2�ԁi�X���[�u�j�������B���Ƃ͖����B�i�}�X�^�j*/	
	io_out8(PIC1_IMR, 0xff);	/*11111111�܂�A�S�Ė���*/
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0x7f);	/*IRQ-07�΍�*/

	return;
}

void inthandler27(int *esp)
{
	io_out8(PIC0_OCW2, 0x67);	/* IRQ-07��t������PIC�ɒʒm �B0x60+�ԍ��B*/
	return;
}


/*CPU Exceptions*/

char *cpu_exceptions[0x20] = {
	"Exception 0x00:Divided by zero.",
	"Exception 0x01:Reserved.",
	"Exception 0x02:Nonmaskable interrupt.",
	"Exception 0x03:Breakpoint.",
	"Exception 0x04:Overflow.",
	"Exception 0x05:Outside BOUND.",
	"Exception 0x06:Invalid opcode.",
	"Exception 0x07:Disable Device.",
	"Exception 0x08:Double fault.",
	"Exception 0x09:Coprocessor Segment Overrun.",
	"Exception 0x0a:Invalid task status segment.",
	"Exception 0x0b:Segment absent.",
	"Exception 0x0c:Stack Segment Fault.",
	"Exception 0x0d:General Protection Exception.",
	"Exception 0x0e:Page fault.",
	"Exception 0x0f:Reserved.",
	"Exception 0x10:Floating point error.",
	"Exception 0x11:Alignment Check.",
	"Exception 0x12:Machine Check.",
	"Exception 0x13:SIMD floating-point exception.",
	"Exception 0x14:Reserved.",
	"Exception 0x15:Reserved.",
	"Exception 0x16:Reserved.",
	"Exception 0x17:Reserved.",
	"Exception 0x18:Reserved.",
	"Exception 0x19:Reserved.",
	"Exception 0x1a:Reserved.",
	"Exception 0x1b:Reserved.",
	"Exception 0x1c:Reserved.",
	"Exception 0x1d:Reserved.",
	"Exception 0x1e:Reserved.",
	"Exception 0x1f:Reserved."
};

void cpu_exception_alert(int exception)
{
	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 0, 0xFFFFFF, 0x000000, (const unsigned char *)cpu_exceptions[exception]);	
}

void inthandler00(int *esp)
{
	cpu_exception_alert(0x00);
}

void inthandler01(int *esp)
{
	cpu_exception_alert(0x01);
}

void inthandler02(int *esp)
{
	cpu_exception_alert(0x02);
}

void inthandler03(int *esp)
{
	cpu_exception_alert(0x03);
}

void inthandler04(int *esp)
{
	cpu_exception_alert(0x04);
}

void inthandler05(int *esp)
{
	cpu_exception_alert(0x05);
}

void inthandler06(int *esp)
{
	cpu_exception_alert(0x06);
}

void inthandler07(int *esp)
{
	cpu_exception_alert(0x07);
}

void inthandler08(int *esp)
{
	cpu_exception_alert(0x08);
}

void inthandler09(int *esp)
{
	cpu_exception_alert(0x09);
}

void inthandler0a(int *esp)
{
	cpu_exception_alert(0x0a);
}

void inthandler0b(int *esp)
{
	cpu_exception_alert(0x0b);
}

void inthandler0c(int *esp)
{
	cpu_exception_alert(0x0c);
}

void inthandler0d(int *esp)
{
	cpu_exception_alert(0x0d);
}

void inthandler0e(int *esp)
{
	cpu_exception_alert(0x0e);
}

void inthandler0f(int *esp)
{
	cpu_exception_alert(0x0f);
}

void inthandler10(int *esp)
{
	cpu_exception_alert(0x10);
}

void inthandler11(int *esp)
{
	cpu_exception_alert(0x11);
}

void inthandler12(int *esp)
{
	cpu_exception_alert(0x12);
}

void inthandler13(int *esp)
{
	cpu_exception_alert(0x13);
}

void inthandler14(int *esp)
{
	cpu_exception_alert(0x14);
}

void inthandler15(int *esp)
{
	cpu_exception_alert(0x15);
}

void inthandler16(int *esp)
{
	cpu_exception_alert(0x16);
}

void inthandler17(int *esp)
{
	cpu_exception_alert(0x17);
}

void inthandler18(int *esp)
{
	cpu_exception_alert(0x18);
}

void inthandler19(int *esp)
{
	cpu_exception_alert(0x19);
}

void inthandler1a(int *esp)
{
	cpu_exception_alert(0x1a);
}

void inthandler1b(int *esp)
{
	cpu_exception_alert(0x1b);
}

void inthandler1c(int *esp)
{
	cpu_exception_alert(0x1c);
}

void inthandler1d(int *esp)
{
	cpu_exception_alert(0x1d);
}

void inthandler1e(int *esp)
{
	cpu_exception_alert(0x1e);
}

void inthandler1f(int *esp)
{
	cpu_exception_alert(0x1f);
}

