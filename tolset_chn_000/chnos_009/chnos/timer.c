
#include "core.h"

void Initialise_ProgrammableIntervalTimer(void)
{
	IO_Out8(PIT_CTRL, 0x34);
	IO_Out8(PIT_CNT0, 0x9c);
	IO_Out8(PIT_CNT0, 0x2e);
	Set_GateDescriptor((IO_GateDescriptor *)ADR_IDT + 0x20, (int) asm_InterruptHandler20, 2 * 8, AR_INTGATE32);
	IO_Out8(PIC0_IMR, IO_In8(PIC0_IMR) & 0xfe);
	return;
}

void InterruptHandler20(int *esp)
{
	IO_Out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	return;
}
