#include "core.h"

void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_SEG_DESC;
	struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) ADR_GATE_DESC;
	int i;

	for(i=0;i <= LIMIT_GDT /8;i++) {
		set_segmdesc(gdt+i,0,0,0);
	}
	set_segmdesc(gdt+1,0xffffffff,0x00000000,AR_DATA32_RW);
	set_segmdesc(gdt+2,LIMIT_BOTPAK,ADR_BOTPAK,AR_CODE32_ER);
	/*
	set_segm��set_gate�̊e��l�����́A�A�N�Z�X���̐ݒ�B
	��ʓ񌅂́A�f�r�b�g�ƃZ�O�����g�̃��[�h�ɂ��ω��B
	�f�r�b�g=0,���[�h=32�r�b�g >>>>>40
	�f�r�b�g=1,���[�h=32�r�b�g >>>>>C0
	�f�r�b�g=0,���[�h=16�r�b�g >>>>>00
	�f�r�b�g=1,���[�h=16�r�b�g >>>>>80
	(�f�r�b�g�P�����~�b�g��4KB�y�[�W�Ƃ��ĉ��߂���B
	�[���̎��́A�o�C�g�P�ʁB)
	���ʓ񌅂́A�ȉ��̒ʂ�
	���g�p			0x00
	�V�X�e���ǂݏ����\���s�s��	0x92
	�V�X�e���ǂݎ��s�\�����s��	0x9a
	�A�v���ǂݏ����\���s�s��	0xf2
	�A�v���ǂݎ��s�\�����s��	0xfa

	�������́A���~�b�g�B
	��O�����́A�x�[�X�B
	��O�����́A���ꂼ��gdt+�ԍ��Aidt+�ԍ��B
	*/
	load_gdtr(LIMIT_GDT,ADR_SEG_DESC);
	
	for(i =0;i <= LIMIT_IDT / 8;i++) {
		set_gatedesc(idt+i,0,0,0);

	}
	load_idtr(LIMIT_IDT, ADR_GATE_DESC);
	set_gatedesc(idt+0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt+0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt+0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);
	return;

}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	if(limit > 0xfffff) {
		ar |=0x8000;  /*OR AR,0x8000 => Gbit=1*/
		limit /= 0x1000;	
	}
	sd->limit_low	= limit & 0xffff;
	sd->base_low	= base & 0xffff;
	sd->base_mid	= (base >> 16) & 0xff;
	sd->access_right	= ar & 0xff;
	sd->limit_high	= ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high	= (base >> 24) & 0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low	= offset & 0xffff;
	gd->selector	= selector;
	gd->dw_count	= (ar >> 8) & 0xff;
	gd->access_right	= ar & 0xff;
	gd->offset_high	= (offset >> 16) & 0xffffff;
	return;
}




