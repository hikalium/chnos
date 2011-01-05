
#include "core.h"

void init_pic(void)
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

	io_out8(PIC0_IMR, 0xff);	/*割り込み全部無視（マスタ）*/
	io_out8(PIC1_IMR, 0xff);	/*割り込み全部無視（スレーブ）*/

	io_out8(PIC0_ICW1, 0x11);	/*エッジトリガモードに設定（マスタ）*/
	io_out8(PIC0_ICW2, 0x20);	/*割り込み番号を、20~27に設定。（マスタ）*/
	io_out8(PIC0_ICW3, 1 << 2);	/*00000100 つまり、スレーブはIRQ2につながってますよということ。*/
	io_out8(PIC0_ICW4, 0x01);	/*ノンバッファモード（マスタ）*/

	io_out8(PIC1_ICW1, 0x11);	/*エッジトリガモードに設定（スレーブ）*/
	io_out8(PIC1_ICW2, 0x28);	/*割り込み番号を、28~2fに設定。（スレーブ）*/
	io_out8(PIC1_ICW3, 2);	/*自分はマスタのIRQ2番につながってますということ。*/
	io_out8(PIC1_ICW4, 0x01);	/*ノンバッファモード（スレーブ）*/

	io_out8(PIC0_IMR, 0xfb);	/*11111011つまり、IRQ2番（スレーブ）だけ許可。あとは無視。（マスタ）*/	
	io_out8(PIC1_IMR, 0xff);	/*11111111つまり、全て無視*/

	set_gatedesc(system.io.interrupt.idt+0x27, (int)asm_inthandler27, 2 * 8, AR_INTGATE32);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0x7f);	/*IRQ-07対策*/

	return;
}

void inthandler27(int *esp)
{
	io_out8(PIC0_OCW2, 0x67);	/* IRQ-07受付完了をPICに通知 。0x60+番号。*/
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

void cpu_exception_abort(int exception, int *esp)
{
	int i;
	char s[32];

//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 0, 0xFFFFFF, (const uchar *)cpu_exceptions[exception]);
//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 16, 0xFFFFFF, "***registers info***");

	sprintf(s, "%s\n\r", (uchar *)cpu_exceptions[exception]);
	send_serial(s);

//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 32, 0xFFFFFF, "#PUSHAD by _asm_inthandler");

	send_serial("#PUSHAD by _asm_inthandler\n\r");

	for(i = 0; i <= 7; i++){
//		sprintf(s, "%s:0x%08X", cpu_exception_infos[i], esp[i]);
//		putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 16 * (i + 3), 0xFFFFFF, s);

		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		send_serial(s);
	}

//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 176, 0xFFFFFF, "#PUSH by _asm_inthandler");
	send_serial("#PUSH by _asm_inthandler\n\r");

	for(i = 8; i <= 9; i++){
//		sprintf(s, "%s:0x%08X", cpu_exception_infos[i], esp[i]);
//		putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 16 * (i + 4), 0xFFFFFF, s);

		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		send_serial(s);
	}

//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 224, 0xFFFFFF, "#PUSH by CPU");

	send_serial("#PUSH by CPU\n\r");
	for(i = 10; i <= 15; i++){
//		sprintf(s, "%s:0x%08X", cpu_exception_infos[i], esp[i]);
//		putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 16 * (i + 5), 0xFFFFFF, s);

		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		send_serial(s);
	}

	send_serial("#Control Registers\n\r");
	sprintf(s, "CR0 = 0x%08X\n\r", load_cr0());
	send_serial(s);
	sprintf(s, "CR2 = 0x%08X\n\r", load_cr2());
	send_serial(s);
	sprintf(s, "CR3 = 0x%08X\n\r", load_cr3());
	send_serial(s);

	for(;;){
		io_hlt();
	}
}

uint cpu_exception_fault(int exception, int *esp)
{
	UI_Task *task = task_now();
	UI_Console *cons;
	uchar s[64];
	uint i;

	for(i = 0; i < MAX_CONSOLES; i++){
		if(system.ui.console.consoles[i].task == task)break;
	}
	if(system.ui.console.consoles[i].task != task)cpu_exception_abort(exception, esp);
	cons = &system.ui.console.consoles[i];

	cons_put_str(cons, (uchar *)cpu_exceptions[exception]);
	sprintf(s, "\n%s:0x%08X", cpu_exception_infos[11], esp[11]);
	cons_put_str(cons, s);

	return (uint)&(task->tss.esp0);
}

void inthandler00(int *esp)
{
	cpu_exception_abort(0x00, esp);
}

void inthandler01(int *esp)
{
	cpu_exception_abort(0x01, esp);
}

void inthandler02(int *esp)
{
	cpu_exception_abort(0x02, esp);
}

void inthandler03(int *esp)
{
	cpu_exception_abort(0x03, esp);
}

void inthandler04(int *esp)
{
	cpu_exception_abort(0x04, esp);
}

void inthandler05(int *esp)
{
	cpu_exception_abort(0x05, esp);
}

void inthandler06(int *esp)
{
	cpu_exception_abort(0x06, esp);
}

void inthandler07(int *esp)
{
	cpu_exception_abort(0x07, esp);
}

void inthandler08(int *esp)
{
	cpu_exception_abort(0x08, esp);
}

void inthandler09(int *esp)
{
	cpu_exception_abort(0x09, esp);
}

void inthandler0a(int *esp)
{
	cpu_exception_abort(0x0a, esp);
}

void inthandler0b(int *esp)
{
	cpu_exception_abort(0x0b, esp);
}

void inthandler0c(int *esp)
{
	cpu_exception_fault(0x0c, esp);
}

void inthandler0d(int *esp)
{
	cpu_exception_fault(0x0d, esp);
}

void inthandler0e(int *esp)
{
	cpu_exception_abort(0x0e, esp);
}

void inthandler0f(int *esp)
{
	cpu_exception_abort(0x0f, esp);
}

void inthandler10(int *esp)
{
	cpu_exception_abort(0x10, esp);
}

void inthandler11(int *esp)
{
	cpu_exception_abort(0x11, esp);
}

void inthandler12(int *esp)
{
	cpu_exception_abort(0x12, esp);
}

void inthandler13(int *esp)
{
	cpu_exception_abort(0x13, esp);
}

void inthandler14(int *esp)
{
	cpu_exception_abort(0x14, esp);
}

void inthandler15(int *esp)
{
	cpu_exception_abort(0x15, esp);
}

void inthandler16(int *esp)
{
	cpu_exception_abort(0x16, esp);
}

void inthandler17(int *esp)
{
	cpu_exception_abort(0x17, esp);
}

void inthandler18(int *esp)
{
	cpu_exception_abort(0x18, esp);
}

void inthandler19(int *esp)
{
	cpu_exception_abort(0x19, esp);
}

void inthandler1a(int *esp)
{
	cpu_exception_abort(0x1a, esp);
}

void inthandler1b(int *esp)
{
	cpu_exception_abort(0x1b, esp);
}

void inthandler1c(int *esp)
{
	cpu_exception_abort(0x1c, esp);
}

void inthandler1d(int *esp)
{
	cpu_exception_abort(0x1d, esp);
}

void inthandler1e(int *esp)
{
	cpu_exception_abort(0x1e, esp);
}

void inthandler1f(int *esp)
{
	cpu_exception_abort(0x1f, esp);
}
