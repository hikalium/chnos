#include "core.h"

void CHNMain(void)
{	
	unsigned char s[24], t[7], beforet = 0xff;
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	init_gdtidt();
	init_pic();
	init_pit();
	io_sti();
	init_scrn_i(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrny, vinfo->BitsPerPixel);

	pit_beep_off();

	for (;;){
	readrtc(t);
	if (beforet != t[0]) {
	sprintf(s, "%02X%02X.%02X.%02X %02X:%02X:%02X", t[6], t[5], t[4], t[3], t[2], t[1], t[0]);
	boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, RGB16(20,40,30), binfo->scrnx - 200, binfo->scrny - 40, binfo->scrnx, binfo->scrny);
	putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrnx - 200, binfo->scrny - 40, RGB16(0,0,0), s);
	beforet = t[0];
		}

	}


}



