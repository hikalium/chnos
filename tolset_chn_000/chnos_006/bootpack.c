#include "core.h"

void CHNMain(void)
{	
	unsigned char s[24], t[7], beforet = 0xff;
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	struct FIFO32 sysfifo;
	int fifobuf[256], i,time_tick;
	init_gdtidt();
	init_pic();
	init_pit(&time_tick);

	io_sti();

	init_scrn_i(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrny, vinfo->BitsPerPixel);
	fifo32_init(&sysfifo, 256, fifobuf);
	init_keyboard(&sysfifo, SYSFIFO_KEYB);
	pit_beep_off();

	for (;;){
	io_cli();
	if(fifo32_status(&sysfifo) == 0) {
		io_stihlt();
	} else {
		i = fifo32_get(&sysfifo);
	}
	if( 256 <= i && i <=511) {
			i -= SYSFIFO_KEYB;
			io_sti();
			boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,240,264 , 256);	
			sprintf(s,"INT 21(IRQ-1) : PS/2 ·°ÎÞ°ÄÞ%02X",i);
			putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,240,0xffffff,s);
			
		}
	}
}



