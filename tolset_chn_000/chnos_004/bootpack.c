#include "core.h"
#include <stdio.h>

void CHNMain(void)
{	
	unsigned char s[24], t[7], beforet = 0xff;
	unsigned short mousecur[576];
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	struct VESAINFO *vinfo = (struct VESAINFO *) 0x0e00;
	init_scrn(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrny);
	init_mouse_cursor16(mousecur, RGB16(5,10,5));
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
void init_scrn(unsigned short *vram, int xsize, int ysize )
{
	boxfill16(vram,xsize,RGB16(5,10,5),0,0,xsize,ysize);
	boxfill16(vram,xsize,RGB16(20,40,30),0,ysize-40,xsize,ysize);
	boxfill16(vram,xsize,RGB16(20,40,30),xsize/2-8,ysize/2-8,xsize/2+8,ysize/2+8);
	putfonts16_asc(vram, xsize, 8, 8, RGB16(31,62,31), "welcome to CHNOSProject!");
	return;

}


void boxfill16(unsigned short *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0;y <= y1;y++) {
		for (x = x0; x <= x1; x++)
		vram[y * xsize + x] = c;
	}
	return;
}

void putfonts16_asc(unsigned short *vram, int xsize, int x, int y, unsigned short c, unsigned char *s)
{
	extern char hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont16(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void putfont16(unsigned short *vram, int xsize, int x, int y, unsigned short c, char *font)
{
	int i;
	short *p;
	char d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

void init_mouse_cursor16(short *mouse, unsigned short bc)
{
	static char cursor[24][24] = {
		"........................",
		"*O**....................",
		"*OO**...................",
		"*OOO**..................",
		"*OOOO**.................",
		"*OOOOO**................",
		"*OOOOOO**...............",
		"*OOOOOOO**..............",
		"*OOOOOOOO**.............",
		"*OOOOOOOOO**............",
		"*OOOOOOOOOO**...........",
		"*OOOOOOOOOOO**..........",
		"*OOOOOOOOOOOO**.........",
		"*OOOOOOOOOOOOO**........",
		"*OOOOOOOOOOOOOO**.......",
		"*OOOOOOOOOOOOOOO**......",
		"*OOOOOOOOOOOOOOOO**.....",
		"*OOOOOO*************....",
		"*OOOOO**................",
		"*OOOO**.................",
		"*OOO**..................",
		"*OO**...................",
		"*O**....................",
		"***.....................",
	};
	int x, y;

	for (y = 0; y < 24; y++) {
		for (x = 0; x < 24; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 24 + x] = RGB16(0,0,0);
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 24 + x] = RGB16(31,62,31);
			}
			if (cursor[y][x] == '.') {
				mouse[y * 24 + x] = bc;
			}
		}
	}
	return;
}

void putblock16_16(unsigned short *vram, int vxsize, int pxsize,int pysize, int px0, int py0, short *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

void readrtc(unsigned char *t)
{
    char err;
    static unsigned char adr[7] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09, 0x32 };
    static unsigned char max[7] = { 0x60, 0x59, 0x23, 0x31, 0x12, 0x99, 0x99 };
    int i;
    for (;;) { /* “Ç‚Ýž‚Ý‚ª¬Œ÷‚·‚é‚Ü‚ÅŒJ‚è•Ô‚· */
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

