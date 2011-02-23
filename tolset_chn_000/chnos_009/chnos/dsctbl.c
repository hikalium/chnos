
#include "core.h"

void Initialise_GlobalDescriptorTable(void)
{
	uint i;
	IO_SegmentDescriptor *gdt = (IO_SegmentDescriptor *)ADR_GDT;

	for(i =0; i < 8192; i++){
		Set_SegmentDescriptor(gdt + i, 0, 0, 0);
	}
	Set_SegmentDescriptor(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
	Set_SegmentDescriptor(gdt + 2, 0x0007ffff, 0x00280000, AR_CODE32_ER);
	Load_GDTR(0xffff, (int)gdt);

	return;
}

void Initialise_InterruptDescriptorTable(void)
{
	uint i;
	IO_GateDescriptor *idt = (IO_GateDescriptor *)ADR_IDT;

	for(i = 0; i < 256; i++) {
		Set_GateDescriptor(idt + i, 0, 0, 0);
	}
	Load_IDTR(0x7ff, (int)idt);

	Set_GateDescriptor(idt+0x00, (int) asm_CPU_ExceptionHandler00, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x01, (int) asm_CPU_ExceptionHandler01, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x02, (int) asm_CPU_ExceptionHandler02, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x03, (int) asm_CPU_ExceptionHandler03, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x04, (int) asm_CPU_ExceptionHandler04, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x05, (int) asm_CPU_ExceptionHandler05, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x06, (int) asm_CPU_ExceptionHandler06, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x07, (int) asm_CPU_ExceptionHandler07, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x08, (int) asm_CPU_ExceptionHandler08, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x09, (int) asm_CPU_ExceptionHandler09, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x0a, (int) asm_CPU_ExceptionHandler0a, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x0b, (int) asm_CPU_ExceptionHandler0b, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x0c, (int) asm_CPU_ExceptionHandler0c, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x0d, (int) asm_CPU_ExceptionHandler0d, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x0e, (int) asm_CPU_ExceptionHandler0e, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x0f, (int) asm_CPU_ExceptionHandler0f, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x10, (int) asm_CPU_ExceptionHandler10, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x11, (int) asm_CPU_ExceptionHandler11, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x12, (int) asm_CPU_ExceptionHandler12, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x13, (int) asm_CPU_ExceptionHandler13, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x14, (int) asm_CPU_ExceptionHandler14, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x15, (int) asm_CPU_ExceptionHandler15, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x16, (int) asm_CPU_ExceptionHandler16, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x17, (int) asm_CPU_ExceptionHandler17, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x18, (int) asm_CPU_ExceptionHandler18, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x19, (int) asm_CPU_ExceptionHandler19, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x1a, (int) asm_CPU_ExceptionHandler1a, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x1b, (int) asm_CPU_ExceptionHandler1b, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x1c, (int) asm_CPU_ExceptionHandler1c, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x1d, (int) asm_CPU_ExceptionHandler1d, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x1e, (int) asm_CPU_ExceptionHandler1e, 2 * 8, AR_INTGATE32);
	Set_GateDescriptor(idt+0x1f, (int) asm_CPU_ExceptionHandler1f, 2 * 8, AR_INTGATE32);

	return;
}

void Set_SegmentDescriptor(IO_SegmentDescriptor *sd, uint limit, int base, int ar)
{
	if(limit > 0xfffff){
		ar |= 0x8000;
		limit = limit >> 12;
	}
	sd->limit_low		= limit & 0xffff;
	sd->base_low		= base & 0xffff;
	sd->base_mid		= (base >> 16) & 0xff;
	sd->access_right	= ar & 0xff;
	sd->limit_high		= ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high		= (base >> 24) & 0xff;
	return;
}

void Set_GateDescriptor(IO_GateDescriptor *gd, int offset, int selector, int ar)
{
	gd->offset_low		= offset & 0xffff;
	gd->selector		= selector;
	gd->dw_count		= (ar >> 8) & 0xff;
	gd->access_right	= ar & 0xff;
	gd->offset_high		= (offset >> 16) & 0xffff;
	return;
}
