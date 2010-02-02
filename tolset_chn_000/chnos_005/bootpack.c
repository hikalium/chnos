#include "core.h"

void CHNMain(void)
{	
	unsigned char s[24], t[7], beforet = 0xff;
	unsigned short mousecur[576];
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	struct VESAINFO *vinfo = (struct VESAINFO *) 0x0e00;
	init_scrn(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrny);
	init_mouse_cursor16(mousecur, DESKTOP_COL);
	putblock16_16(vinfo->PhysBasePtr, binfo->scrnx, 24, 24, binfo->scrnx/2, binfo->scrny/2, mousecur, 24);

	for (;;){
	readrtc(t);

	if (beforet != t[0]) {
	sprintf(s, "%02X%02X.%02X.%02X %02X:%02X:%02X", t[6], t[5], t[4], t[3], t[2], t[1], t[0]);
	boxfill16(vinfo->PhysBasePtr, binfo->scrnx, RGB16(20,40,30), binfo->scrnx - 200, binfo->scrny - 40, binfo->scrnx, binfo->scrny);
	putfonts16_asc(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrnx - 200, binfo->scrny - 40, RGB16(0,0,0), s);
	beforet = t[0];
		}

	}
	for(;;) {
	io_hlt();
	}


}

void readrtc(unsigned char *t)
{
    char err;
    static unsigned char adr[7] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09, 0x32 };
    static unsigned char max[7] = { 0x60, 0x59, 0x23, 0x31, 0x12, 0x99, 0x99 };
    int i;
    for (;;) { /* “Ç‚İ‚İ‚ª¬Œ÷‚·‚é‚Ü‚ÅŒJ‚è•Ô‚· */
        err = 0;
        for (i = 0; i < 7; i++) {
            io_out8(0x70, adr[i]);
            t[i] = io_in8(0x71);
        }
        for (i = 0; i < 7; i++) {
            io_out8(0x70, adr[i]);
            if (t[i] != io_in8(0x71) || (t[i] & 0x0f) > 9 || t[i] > max[i]) {
                err = 1;
            }
        }
        if (err == 0) {
            return;
        }
    }
}

