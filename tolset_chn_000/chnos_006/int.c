#include "core.h"


void init_pic(void)
{
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



