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
	set_segmとset_gateの各第四引数は、アクセス権の設定。
	上位二桁は、Ｇビットとセグメントのモードにより変化。
	Ｇビット=0,モード=32ビット >>>>>40
	Ｇビット=1,モード=32ビット >>>>>C0
	Ｇビット=0,モード=16ビット >>>>>00
	Ｇビット=1,モード=16ビット >>>>>80
	(Ｇビット１＝リミットを4KBページとして解釈する。
	ゼロの時は、バイト単位。)
	下位二桁は、以下の通り
	未使用			0x00
	システム読み書き可能実行不可	0x92
	システム読み実行可能書き不可	0x9a
	アプリ読み書き可能実行不可	0xf2
	アプリ読み実行可能書き不可	0xfa

	第二引数は、リミット。
	第三引数は、ベース。
	第三引数は、それぞれgdt+番号、idt+番号。
	*/
	load_gdtr(LIMIT_GDT,ADR_SEG_DESC);
	
	for(i =0;i <= LIMIT_IDT / 8;i++) {
		set_gatedesc(idt+i,0,0,0);

	}
	load_idtr(LIMIT_IDT, ADR_GATE_DESC);
	set_gatedesc(idt+0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);

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




