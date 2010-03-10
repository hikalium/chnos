#include "core.h"

void CHNMain(void)
{	
	unsigned char s[24], t[7], beforet = 0xff;
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	struct FIFO32 sysfifo;
	int fifobuf[256], i;
	init_gdtidt();
	init_pic();
	init_pit();

	io_sti();

	init_scrn_i(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrny, vinfo->BitsPerPixel);
	fifo32_init(&sysfifo, 256, fifobuf);
	init_keyboard(&sysfifo, SYSFIFO_KEYB);
	pit_beep_off();

	for (;;){
	io_cli();
	if (fifo32_status(&sysfifo) == 0) {
		io_stihlt();
//		readrtc(t);
//		if (beforet != t[0]) {
//			sprintf(s, "%02X%02X.%02X.%02X %02X:%02X:%02X", t[6], t[5], t[4], t[3], t[2], t[1], t[0]);
//			boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, RGB16(20,40,30), binfo->scrnx - 200, binfo->scrny - 40, binfo->scrnx, binfo->scrny);
//			putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrnx - 200, binfo->scrny - 40, RGB16(0,0,0), s);
//			beforet = t[0];
//			}
		} else {
			i = fifo32_get(&sysfifo);
			io_sti();
			boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,240,264 , 256);	
			sprintf(s,"INT 21(IRQ-1) : PS/2 ·°ÎÞ°ÄÞ%02X",i);
			putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,240,0xffffff,s);
			
		}
	}
}



