
#include "core.h"

int xsize;

void init_pic(void)
{
	xsize = system.info.boot.scrnx;
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
	"Exception 0x00:Divided by zero.",
	"Exception 0x01:Reserved.",
	"Exception 0x02:Nonmaskable interrupt.",
	"Exception 0x03:Breakpoint.",
	"Exception 0x04:Overflow.",
	"Exception 0x05:Outside BOUND.",
	"Exception 0x06:Invalid opcode.",
	"Exception 0x07:Disable Device.",
	"Exception 0x08:Double fault.",
	"Exception 0x09:Coprocessor Segment Overrun.",
	"Exception 0x0a:Invalid task status segment.",
	"Exception 0x0b:Segment absent.",
	"Exception 0x0c:Stack Segment Fault.",
	"Exception 0x0d:General Protection Exception.",
	"Exception 0x0e:Page fault.",
	"Exception 0x0f:Reserved.",
	"Exception 0x10:Floating point error.",
	"Exception 0x11:Alignment Check.",
	"Exception 0x12:Machine Check.",
	"Exception 0x13:SIMD floating-point exception.",
	"Exception 0x14:Reserved.",
	"Exception 0x15:Reserved.",
	"Exception 0x16:Reserved.",
	"Exception 0x17:Reserved.",
	"Exception 0x18:Reserved.",
	"Exception 0x19:Reserved.",
	"Exception 0x1a:Reserved.",
	"Exception 0x1b:Reserved.",
	"Exception 0x1c:Reserved.",
	"Exception 0x1d:Reserved.",
	"Exception 0x1e:Reserved.",
	"Exception 0x1f:Reserved."
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

void cpu_exception_alert(int exception, int *esp)
{
	int i;
	char s[32];
	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 0, 0xFFFFFF, 0x000000, (const unsigned char *)cpu_exceptions[exception]);
	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 16, 0xFFFFFF, 0x000000, "***registers info***");

	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 32, 0xFFFFFF, 0x000000, "#PUSHAD by _asm_inthandler");
	for(i = 0; i <= 7; i++){
	sprintf(s, "%s:0x%08X", cpu_exception_infos[i], esp[i]);
	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 16 * (i + 3), 0xFFFFFF, 0x000000, s);
	}

	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 176, 0xFFFFFF, 0x000000, "#PUSH by _asm_inthandler");
	for(i = 8; i <= 9; i++){
	sprintf(s, "%s:0x%08X", cpu_exception_infos[i], esp[i]);
	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 16 * (i + 4), 0xFFFFFF, 0x000000, s);
	}

	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 224, 0xFFFFFF, 0x000000, "#PUSH by CPU");
	for(i = 10; i <= 15; i++){
	sprintf(s, "%s:0x%08X", cpu_exception_infos[i], esp[i]);
	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 16 * (i + 5), 0xFFFFFF, 0x000000, s);
	}
	for(;;){
		io_hlt();
	}
}

void inthandler00(int *esp)
{
	cpu_exception_alert(0x00, esp);
}

void inthandler01(int *esp)
{
	cpu_exception_alert(0x01, esp);
}

void inthandler02(int *esp)
{
	cpu_exception_alert(0x02, esp);
}

void inthandler03(int *esp)
{
	cpu_exception_alert(0x03, esp);
}

void inthandler04(int *esp)
{
	cpu_exception_alert(0x04, esp);
}

void inthandler05(int *esp)
{
	cpu_exception_alert(0x05, esp);
}

void inthandler06(int *esp)
{
	cpu_exception_alert(0x06, esp);
}

void inthandler07(int *esp)
{
	cpu_exception_alert(0x07, esp);
}

void inthandler08(int *esp)
{
	cpu_exception_alert(0x08, esp);
}

void inthandler09(int *esp)
{
	cpu_exception_alert(0x09, esp);
}

void inthandler0a(int *esp)
{
	cpu_exception_alert(0x0a, esp);
}

void inthandler0b(int *esp)
{
	cpu_exception_alert(0x0b, esp);
}

void inthandler0c(int *esp)
{
	cpu_exception_alert(0x0c, esp);
}

void inthandler0d(int *esp)
{
	cpu_exception_alert(0x0d, esp);
}

void inthandler0e(int *esp)
{
	cpu_exception_alert(0x0e, esp);
}

void inthandler0f(int *esp)
{
	cpu_exception_alert(0x0f, esp);
}

void inthandler10(int *esp)
{
	cpu_exception_alert(0x10, esp);
}

void inthandler11(int *esp)
{
	cpu_exception_alert(0x11, esp);
}

void inthandler12(int *esp)
{
	cpu_exception_alert(0x12, esp);
}

void inthandler13(int *esp)
{
	cpu_exception_alert(0x13, esp);
}

void inthandler14(int *esp)
{
	cpu_exception_alert(0x14, esp);
}

void inthandler15(int *esp)
{
	cpu_exception_alert(0x15, esp);
}

void inthandler16(int *esp)
{
	cpu_exception_alert(0x16, esp);
}

void inthandler17(int *esp)
{
	cpu_exception_alert(0x17, esp);
}

void inthandler18(int *esp)
{
	cpu_exception_alert(0x18, esp);
}

void inthandler19(int *esp)
{
	cpu_exception_alert(0x19, esp);
}

void inthandler1a(int *esp)
{
	cpu_exception_alert(0x1a, esp);
}

void inthandler1b(int *esp)
{
	cpu_exception_alert(0x1b, esp);
}

void inthandler1c(int *esp)
{
	cpu_exception_alert(0x1c, esp);
}

void inthandler1d(int *esp)
{
	cpu_exception_alert(0x1d, esp);
}

void inthandler1e(int *esp)
{
	cpu_exception_alert(0x1e, esp);
}

void inthandler1f(int *esp)
{
	cpu_exception_alert(0x1f, esp);
}

