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
void inthandler00(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x00:Divided by zero.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler01(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x01:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler02(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x02:Nonmaskable interrupt.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler03(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x03:Breakpoint.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler04(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x04:Overflow.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler05(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x05:Outside BOUND.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler06(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x06:Invalid opcode.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler07(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x07:Disable Device.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler08(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x08:Double fault.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler09(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x09:Coprocessor Segment Overrun.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0a(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0a:Invalid task status segment.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0b(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0b:Segment absent.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0c(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0c:Stack Segment Fault.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0d(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0d:General Protection Exception.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0e(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0e:Page fault.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler0f(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x0f:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler10(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x10:Floating point error.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler11(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x11:Alignment Check.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler12(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x12:Machine Check.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler13(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x13:SIMD floating-point exception.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler14(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x14:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler15(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x15:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler16(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x16:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler17(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x17:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler18(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x18:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler19(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x19:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1a(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1a:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1b(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1b:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1c(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1c:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1d(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1d:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1e(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1e:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}

void inthandler1f(int *esp)
{
		system.draw.putfonts(system.sht_back->buf, xsize, 0, 0, 0xFF0000, "Exception 0x1f:Reserved.");
		system.draw.sheet.refresh(system.sht_back, 0, 0, xsize, 16);
		for(;;){ system.io.hlt(); }
}


