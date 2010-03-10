#include "core.h"

int time_tick_100 = 0;


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

	io_out8(PIC0_IMR, 0xf9);	/*11111001つまり、IRQ2番（スレーブ）とIRQ1番（キーボード）だけ許可。あとは無視。（マスタ）*/	
	io_out8(PIC1_IMR, 0xff);	/*割り込み全部無視（スレーブ）*/

	return;
}

void inthandler20(int *esp)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	char s[20];
	time_tick_100 ++;
	boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,256,264 , 272);	
	sprintf(s, "INT 20(IRQ-0) : PIT : %d", time_tick_100);
	putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,256,0xffffff,s);
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 。0x60+番号。*/
	return;
}


