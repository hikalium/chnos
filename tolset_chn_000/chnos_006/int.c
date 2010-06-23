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
void inthandler00(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x00:Divided by zero.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler01(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x01:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler02(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x02:Nonmaskable interrupt.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler03(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x03:Breakpoint.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler04(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x04:Overflow.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler05(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x05:Outside BOUND.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler06(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x06:Invalid opcode.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler07(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x07:Disable Device.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler08(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x08:Double fault.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler09(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x09:Coprocessor Segment Overrun.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0a(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0a:Invalid task status segment.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0b(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0b:Segment absent.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0c(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0c:Stack Segment Fault.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0d(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0d:General Protection Exception.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0e(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0e:Page fault.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0f(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0f:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler10(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x10:Floating point error.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler11(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x11:Alignment Check.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler12(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x12:Machine Check.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler13(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x13:SIMD floating-point exception.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler14(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x14:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler15(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x15:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler16(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x16:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler17(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x17:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler18(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x18:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler19(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x19:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1a(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1a:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1b(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1b:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1c(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1c:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1d(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1d:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1e(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1e:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1f(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1f:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}


