
#include "core.h"

void Initialise_GlobalDescriptorTable(void)
{
	uint i;

	for(i = 0; i < 8192; i++){
		SegmentDescriptor_Set_Absolute(i, 0, 0, 0);
	}

	SegmentDescriptor_Set_Absolute(0x01, 0xffffffff, 0x00000000, AR_DATA32_RW);
	SegmentDescriptor_Set_Absolute(0x02, LIMIT_BOOTPACK, ADR_BOOTPACK, AR_CODE32_ER);

	Load_GDTR(LIMIT_GDT, ADR_GDT);

	return;
}

void Initialise_InterruptDescriptorTable(void)
{
	uint i;

	for(i = 0; i < 256; i++) {
		GateDescriptor_Set(i, 0, 0, 0);
	}

	Load_IDTR(LIMIT_IDT, ADR_IDT);

	GateDescriptor_Set(0x00, (uint)asm_CPU_ExceptionHandler00, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x01, (uint)asm_CPU_ExceptionHandler01, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x02, (uint)asm_CPU_ExceptionHandler02, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x03, (uint)asm_CPU_ExceptionHandler03, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x04, (uint)asm_CPU_ExceptionHandler04, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x05, (uint)asm_CPU_ExceptionHandler05, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x06, (uint)asm_CPU_ExceptionHandler06, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x07, (uint)asm_CPU_ExceptionHandler07, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x08, (uint)asm_CPU_ExceptionHandler08, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x09, (uint)asm_CPU_ExceptionHandler09, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x0a, (uint)asm_CPU_ExceptionHandler0a, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x0b, (uint)asm_CPU_ExceptionHandler0b, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x0c, (uint)asm_CPU_ExceptionHandler0c, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x0d, (uint)asm_CPU_ExceptionHandler0d, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x0e, (uint)asm_CPU_ExceptionHandler0e, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x0f, (uint)asm_CPU_ExceptionHandler0f, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x10, (uint)asm_CPU_ExceptionHandler10, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x11, (uint)asm_CPU_ExceptionHandler11, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x12, (uint)asm_CPU_ExceptionHandler12, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x13, (uint)asm_CPU_ExceptionHandler13, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x14, (uint)asm_CPU_ExceptionHandler14, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x15, (uint)asm_CPU_ExceptionHandler15, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x16, (uint)asm_CPU_ExceptionHandler16, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x17, (uint)asm_CPU_ExceptionHandler17, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x18, (uint)asm_CPU_ExceptionHandler18, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x19, (uint)asm_CPU_ExceptionHandler19, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x1a, (uint)asm_CPU_ExceptionHandler1a, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x1b, (uint)asm_CPU_ExceptionHandler1b, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x1c, (uint)asm_CPU_ExceptionHandler1c, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x1d, (uint)asm_CPU_ExceptionHandler1d, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x1e, (uint)asm_CPU_ExceptionHandler1e, 0x02, AR_INTGATE32);
	GateDescriptor_Set(0x1f, (uint)asm_CPU_ExceptionHandler1f, 0x02, AR_INTGATE32);

	return;
}

void SegmentDescriptor_Set_Absolute(uint sd, uint limit, uint base, uint ar)
{
	IO_SegmentDescriptor *gdt = (IO_SegmentDescriptor *)ADR_GDT;

	if(limit > 0xfffff){
		ar |= 0x8000;
		limit = limit >> 12;
	}
	gdt[sd].limit_low	= limit & 0xffff;
	gdt[sd].base_low	= base & 0xffff;
	gdt[sd].base_mid	= (base >> 16) & 0xff;
	gdt[sd].access_right	= ar & 0xff;
	gdt[sd].limit_high	= ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	gdt[sd].base_high	= (base >> 24) & 0xff;

	return;
}

uint SegmentDescriptor_Get_Base(uint sd)
{
	IO_SegmentDescriptor *gdt = (IO_SegmentDescriptor *)ADR_GDT;

	return (gdt[sd].base_low | (gdt[sd].base_mid << 16) | (gdt[sd].base_high << 24));
}

uint SegmentDescriptor_Get_Limit(uint sd)
{
	IO_SegmentDescriptor *gdt = (IO_SegmentDescriptor *)ADR_GDT;

	if((gdt[sd].limit_high & 0x80) != 0){
		return ((((gdt[sd].limit_low | ((gdt[sd].limit_high & 0x0f) << 16))) << 12) | 0xFFF);
	}

	return (gdt[sd].limit_low | ((gdt[sd].limit_high & 0x0f) << 16));
}

uint SegmentDescriptor_Get_AccessRight(uint sd)
{
	IO_SegmentDescriptor *gdt = (IO_SegmentDescriptor *)ADR_GDT;

	return (gdt[sd].access_right | ((gdt[sd].limit_high & 0xf0) << 8)) & 0x7FFF;
}

uint SegmentDescriptor_Set(uint limit, int base, int ar)
{
	uint *retaddr;
	uint i;

	retaddr = &limit;

	for(i = 1; i < 8192; i++){
		if(SegmentDescriptor_Get_Limit(i) == 0){
			SegmentDescriptor_Set_Absolute(i, limit, base, ar);
			return i;
		}
	}

	Emergency_Out("[0x%08X]SegDesc_Set Failed.", *(retaddr - 1));
	debug("[0x%08X]SegDesc_Set Failed.", *(retaddr - 1));
	for(;;){
		IO_HLT();
	}

	return 0;
}

void GateDescriptor_Set(uint gd, uint offset, uint selector, uint ar)
{
	IO_GateDescriptor *idt = (IO_GateDescriptor *)ADR_IDT;

	idt[gd].offset_low		= offset & 0xffff;
	idt[gd].selector		= selector << 3;
	idt[gd].dw_count		= (ar >> 8) & 0xff;
	idt[gd].access_right	= ar & 0xff;
	idt[gd].offset_high		= (offset >> 16) & 0xffff;

	return;
}
