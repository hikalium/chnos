
#include "core.h"

uint counter;

void CHNMain(void)
{
	IO_CLI();
	Initialise_SerialPort();
	Initialise_GlobalDescriptorTable();
	Initialise_InterruptDescriptorTable();
	Initialise_ProgrammableInterruptController();
	Initialise_ProgrammableIntervalTimer();
	IO_STI();

	for (;;) {
		IO_CLI();

		IO_STIHLT();
	}
}

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

void Initialise_ProgrammableInterruptController(void)
{
	/*
	IRR：インタラプトリクエストレジスタ 
		.IRQピンの状態を表す。ポートを読んだときに、IRRになるかISRになるかは、OCW3で選択する。
		.1になっているビットは、現在要求が来ている（もしくは処理中の）割り込み。
	ISR：インサービスレジスタ 
		.現在処理中の割り込みがどれであるかを示す。ポートを読んだときに、IRRになるかISRになるかは、OCW3で選択する。
		.1になっているビットは、現在処理中の割り込み。処理中というのは、CPUに対してINT命令を発行したが、EOI（割り込み終了命令）を受け取っていない割り込み。
	IMR：インタラプトマスクレジスタ 
		.これが1になっている割り込みは、IRRが1になっていても、反応しない。 
	*/

	IO_Out8(PIC0_IMR, 0xff);	/*割り込み全部無視（マスタ）*/
	IO_Out8(PIC1_IMR, 0xff);	/*割り込み全部無視（スレーブ）*/

	IO_Out8(PIC0_ICW1, 0x11);	/*エッジトリガモードに設定（マスタ）*/
	IO_Out8(PIC0_ICW2, 0x20);	/*割り込み番号を、20~27に設定。（マスタ）*/
	IO_Out8(PIC0_ICW3, 1 << 2);	/*00000100 つまり、スレーブはIRQ2につながってますよということ。*/
	IO_Out8(PIC0_ICW4, 0x01);	/*ノンバッファモード（マスタ）*/

	IO_Out8(PIC1_ICW1, 0x11);	/*エッジトリガモードに設定（スレーブ）*/
	IO_Out8(PIC1_ICW2, 0x28);	/*割り込み番号を、28~2fに設定。（スレーブ）*/
	IO_Out8(PIC1_ICW3, 2);		/*自分はマスタのIRQ2番につながってますということ。*/
	IO_Out8(PIC1_ICW4, 0x01);	/*ノンバッファモード（スレーブ）*/

	IO_Out8(PIC0_IMR, 0xfb);	/*11111011つまり、IRQ2番（スレーブ）だけ許可。あとは無視。（マスタ）*/	
	IO_Out8(PIC1_IMR, 0xff);	/*11111111つまり、全て無視*/

	Set_GateDescriptor((IO_GateDescriptor *)ADR_IDT + 0x27, (int)asm_InterruptHandler27, 2 * 8, AR_INTGATE32);
	IO_Out8(PIC0_IMR, IO_In8(PIC0_IMR) & 0x7f);	/*IRQ-07対策*/

	return;
}

void InterruptHandler27(int *esp)
{
	IO_Out8(PIC0_OCW2, 0x67);	/* IRQ-07受付完了をPICに通知 。0x60+番号。*/
	return;
}

/*CPU Exceptions*/
char *cpu_exceptions[0x20] = {
	"Exception 0x00:\n\rDivided by zero.",
	"Exception 0x01:\n\rReserved.",
	"Exception 0x02:\n\rNonmaskable interrupt.",
	"Exception 0x03:\n\rBreakpoint.",
	"Exception 0x04:\n\rOverflow.",
	"Exception 0x05:\n\rOutside BOUND.",
	"Exception 0x06:\n\rInvalid opcode.",
	"Exception 0x07:\n\rDisable Device.",
	"Exception 0x08:\n\rDouble fault.",
	"Exception 0x09:\n\rCoprocessor Segment Overrun.",
	"Exception 0x0a:\n\rInvalid task status segment.",
	"Exception 0x0b:\n\rSegment absent.",
	"Exception 0x0c:\n\rStack Segment Fault.",
	"Exception 0x0d:\n\rGeneral Protection Exception.",
	"Exception 0x0e:\n\rPage fault.",
	"Exception 0x0f:\n\rReserved.",
	"Exception 0x10:\n\rFloating point error.",
	"Exception 0x11:\n\rAlignment Check.",
	"Exception 0x12:\n\rMachine Check.",
	"Exception 0x13:\n\rSIMD floating-point exception.",
	"Exception 0x14:\n\rReserved.",
	"Exception 0x15:\n\rReserved.",
	"Exception 0x16:\n\rReserved.",
	"Exception 0x17:\n\rReserved.",
	"Exception 0x18:\n\rReserved.",
	"Exception 0x19:\n\rReserved.",
	"Exception 0x1a:\n\rReserved.",
	"Exception 0x1b:\n\rReserved.",
	"Exception 0x1c:\n\rReserved.",
	"Exception 0x1d:\n\rReserved.",
	"Exception 0x1e:\n\rReserved.",
	"Exception 0x1f:\n\rReserved."
};

char *cpu_exception_infos[16] = {
	"EDI      ",
	"ESI      ",
	"EBP      ",
	"ESP      ",
	"EBX      ",
	"EDX      ",
	"ECX      ",
	"EAX      ",
	"DS       ",
	"ES       ",
	"ERRORCODE",
	"EIP      ",
	"CS       ",
	"EFLAGS   ",
	"APP's ESP",
	"APP's SS "
};

void CPU_Exception_Abort(int exception, int *esp)
{
	int i;
	uchar s[32];

	sprintf(s, "%s\n\r", (uchar *)cpu_exceptions[exception]);
	Send_SerialPort(s);

	Send_SerialPort("#PUSHAD by _asm_CPU_ExceptionHandler\n\r");
	for(i = 0; i <= 7; i++){
		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		Send_SerialPort(s);
	}

	Send_SerialPort("#PUSH by _asm_CPU_ExceptionHandler\n\r");
	for(i = 8; i <= 9; i++){
		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		Send_SerialPort(s);
	}

	Send_SerialPort("#PUSH by CPU\n\r");
	for(i = 10; i <= 15; i++){
		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		Send_SerialPort(s);
	}

	Send_SerialPort("#Control Registers\n\r");
	sprintf(s, "CR0 = 0x%08X\n\r", Load_CR0());
	Send_SerialPort(s);
	sprintf(s, "CR2 = 0x%08X\n\r", Load_CR2());
	Send_SerialPort(s);
	sprintf(s, "CR3 = 0x%08X\n\r", Load_CR3());
	Send_SerialPort(s);

	for(;;){
		IO_HLT();
	}
}

void CPU_ExceptionHandler00(int *esp)
{
	CPU_Exception_Abort(0x00, esp);
}

void CPU_ExceptionHandler01(int *esp)
{
	CPU_Exception_Abort(0x01, esp);
}

void CPU_ExceptionHandler02(int *esp)
{
	CPU_Exception_Abort(0x02, esp);
}

void CPU_ExceptionHandler03(int *esp)
{
	CPU_Exception_Abort(0x03, esp);
}

void CPU_ExceptionHandler04(int *esp)
{
	CPU_Exception_Abort(0x04, esp);
}

void CPU_ExceptionHandler05(int *esp)
{
	CPU_Exception_Abort(0x05, esp);
}

void CPU_ExceptionHandler06(int *esp)
{
	CPU_Exception_Abort(0x06, esp);
}

void CPU_ExceptionHandler07(int *esp)
{
	CPU_Exception_Abort(0x07, esp);
}

void CPU_ExceptionHandler08(int *esp)
{
	CPU_Exception_Abort(0x08, esp);
}

void CPU_ExceptionHandler09(int *esp)
{
	CPU_Exception_Abort(0x09, esp);
}

void CPU_ExceptionHandler0a(int *esp)
{
	CPU_Exception_Abort(0x0a, esp);
}

void CPU_ExceptionHandler0b(int *esp)
{
	CPU_Exception_Abort(0x0b, esp);
}

void CPU_ExceptionHandler0c(int *esp)
{
	CPU_Exception_Abort(0x0c, esp);
}

void CPU_ExceptionHandler0d(int *esp)
{
	CPU_Exception_Abort(0x0d, esp);
}

void CPU_ExceptionHandler0e(int *esp)
{
	CPU_Exception_Abort(0x0e, esp);
}

void CPU_ExceptionHandler0f(int *esp)
{
	CPU_Exception_Abort(0x0f, esp);
}

void CPU_ExceptionHandler10(int *esp)
{
	CPU_Exception_Abort(0x10, esp);
}

void CPU_ExceptionHandler11(int *esp)
{
	CPU_Exception_Abort(0x11, esp);
}

void CPU_ExceptionHandler12(int *esp)
{
	CPU_Exception_Abort(0x12, esp);
}

void CPU_ExceptionHandler13(int *esp)
{
	CPU_Exception_Abort(0x13, esp);
}

void CPU_ExceptionHandler14(int *esp)
{
	CPU_Exception_Abort(0x14, esp);
}

void CPU_ExceptionHandler15(int *esp)
{
	CPU_Exception_Abort(0x15, esp);
}

void CPU_ExceptionHandler16(int *esp)
{
	CPU_Exception_Abort(0x16, esp);
}

void CPU_ExceptionHandler17(int *esp)
{
	CPU_Exception_Abort(0x17, esp);
}

void CPU_ExceptionHandler18(int *esp)
{
	CPU_Exception_Abort(0x18, esp);
}

void CPU_ExceptionHandler19(int *esp)
{
	CPU_Exception_Abort(0x19, esp);
}

void CPU_ExceptionHandler1a(int *esp)
{
	CPU_Exception_Abort(0x1a, esp);
}

void CPU_ExceptionHandler1b(int *esp)
{
	CPU_Exception_Abort(0x1b, esp);
}

void CPU_ExceptionHandler1c(int *esp)
{
	CPU_Exception_Abort(0x1c, esp);
}

void CPU_ExceptionHandler1d(int *esp)
{
	CPU_Exception_Abort(0x1d, esp);
}

void CPU_ExceptionHandler1e(int *esp)
{
	CPU_Exception_Abort(0x1e, esp);
}

void CPU_ExceptionHandler1f(int *esp)
{
	CPU_Exception_Abort(0x1f, esp);
}

void Initialise_ProgrammableIntervalTimer(void)
{
	IO_Out8(PIT_CTRL, 0x34);
	IO_Out8(PIT_CNT0, 0x9c);
	IO_Out8(PIT_CNT0, 0x2e);
	Set_GateDescriptor((IO_GateDescriptor *)ADR_IDT + 0x20, (int) asm_InterruptHandler20, 2 * 8, AR_INTGATE32);
	IO_Out8(PIC0_IMR, IO_In8(PIC0_IMR) & 0xfe);
	counter = 0;
	return;
}

void InterruptHandler20(int *esp)
{
	counter++;
	IO_Out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	return;
}

void Initialise_SerialPort(void)
{
	IO_Out8(COM1_CTRL_LINE, 0x80);	//ボーレート設定開始
	IO_Out8(COM1_BAUD_LSB, 0x06);	//0x06 = 19.2bps
	IO_Out8(COM1_CTRL_LINE, 0x03);	//ボーレート設定終了、送受信データ8bit
	IO_Out8(COM1_CTRL_MODEM, 0x0b);	//割り込み有効、RTS、DTRピンをアクティブ化
//	IO_Out8(COM1_INTR_ENBL, 0x04);	//ラインステータス割り込み
	IO_Out8(COM1_INTR_ENBL, 0x00);	//割り込みなし

	return;
}

void Send_SerialPort(uchar *s)
{
	for (; *s != 0x00; s++) {
		IO_Out8(COM1_TX, *s);
	}

	return;
}

