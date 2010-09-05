
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

void cpu_exception_alert(int exception)
{
	putfonts_asc_sht_i(system.sys.sht.desktop, 0, 0, 0xFFFFFF, 0x000000, (const unsigned char *)cpu_exceptions[exception]);	
}

void inthandler00(int *esp)
{
	cpu_exception_alert(0x00);
}

void inthandler01(int *esp)
{
	cpu_exception_alert(0x01);
}

void inthandler02(int *esp)
{
	cpu_exception_alert(0x02);
}

void inthandler03(int *esp)
{
	cpu_exception_alert(0x03);
}

void inthandler04(int *esp)
{
	cpu_exception_alert(0x04);
}

void inthandler05(int *esp)
{
	cpu_exception_alert(0x05);
}

void inthandler06(int *esp)
{
	cpu_exception_alert(0x06);
}

void inthandler07(int *esp)
{
	cpu_exception_alert(0x07);
}

void inthandler08(int *esp)
{
	cpu_exception_alert(0x08);
}

void inthandler09(int *esp)
{
	cpu_exception_alert(0x09);
}

void inthandler0a(int *esp)
{
	cpu_exception_alert(0x0a);
}

void inthandler0b(int *esp)
{
	cpu_exception_alert(0x0b);
}

void inthandler0c(int *esp)
{
	cpu_exception_alert(0x0c);
}

void inthandler0d(int *esp)
{
	cpu_exception_alert(0x0d);
}

void inthandler0e(int *esp)
{
	cpu_exception_alert(0x0e);
}

void inthandler0f(int *esp)
{
	cpu_exception_alert(0x0f);
}

void inthandler10(int *esp)
{
	cpu_exception_alert(0x10);
}

void inthandler11(int *esp)
{
	cpu_exception_alert(0x11);
}

void inthandler12(int *esp)
{
	cpu_exception_alert(0x12);
}

void inthandler13(int *esp)
{
	cpu_exception_alert(0x13);
}

void inthandler14(int *esp)
{
	cpu_exception_alert(0x14);
}

void inthandler15(int *esp)
{
	cpu_exception_alert(0x15);
}

void inthandler16(int *esp)
{
	cpu_exception_alert(0x16);
}

void inthandler17(int *esp)
{
	cpu_exception_alert(0x17);
}

void inthandler18(int *esp)
{
	cpu_exception_alert(0x18);
}

void inthandler19(int *esp)
{
	cpu_exception_alert(0x19);
}

void inthandler1a(int *esp)
{
	cpu_exception_alert(0x1a);
}

void inthandler1b(int *esp)
{
	cpu_exception_alert(0x1b);
}

void inthandler1c(int *esp)
{
	cpu_exception_alert(0x1c);
}

void inthandler1d(int *esp)
{
	cpu_exception_alert(0x1d);
}

void inthandler1e(int *esp)
{
	cpu_exception_alert(0x1e);
}

void inthandler1f(int *esp)
{
	cpu_exception_alert(0x1f);
}

