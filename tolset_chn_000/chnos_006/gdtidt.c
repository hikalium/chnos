#include "core.h"

void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_SEG_DESC;
	struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) ADR_GATE_DESC;
	int i;
	struct INTERRUPT *intr = &system.io.interrupt;
	
	for(i=0;i <= LIMIT_GDT /8;i++) {
		intr->set_segment(gdt+i,0,0,0);
	}
	intr->set_segment(gdt+1,0xffffffff,0x00000000,AR_DATA32_RW);
	intr->set_segment(gdt+2,LIMIT_BOTPAK,ADR_BOTPAK,AR_CODE32_ER);
	load_gdtr(LIMIT_GDT,ADR_SEG_DESC);
	
	for(i =0;i <= LIMIT_IDT / 8;i++) {
		intr->set_gate(idt+i,0,0,0);

	}
	load_idtr(LIMIT_IDT, ADR_GATE_DESC);
	intr->set_gate(idt+0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x27, (int) asm_inthandler27, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x00, (int) asm_inthandler00, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x01, (int) asm_inthandler01, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x02, (int) asm_inthandler02, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x03, (int) asm_inthandler03, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x04, (int) asm_inthandler04, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x05, (int) asm_inthandler05, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x06, (int) asm_inthandler06, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x07, (int) asm_inthandler07, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x08, (int) asm_inthandler08, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x09, (int) asm_inthandler09, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x0a, (int) asm_inthandler0a, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x0b, (int) asm_inthandler0b, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x0c, (int) asm_inthandler0c, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x0d, (int) asm_inthandler0d, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x0e, (int) asm_inthandler0e, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x0f, (int) asm_inthandler0f, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x10, (int) asm_inthandler10, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x11, (int) asm_inthandler11, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x12, (int) asm_inthandler12, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x13, (int) asm_inthandler13, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x14, (int) asm_inthandler14, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x15, (int) asm_inthandler15, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x16, (int) asm_inthandler16, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x17, (int) asm_inthandler17, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x18, (int) asm_inthandler18, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x19, (int) asm_inthandler19, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x1a, (int) asm_inthandler1a, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x1b, (int) asm_inthandler1b, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x1c, (int) asm_inthandler1c, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x1d, (int) asm_inthandler1d, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x1e, (int) asm_inthandler1e, 2 * 8, AR_INTGATE32);
	intr->set_gate(idt+0x1f, (int) asm_inthandler1f, 2 * 8, AR_INTGATE32);

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




