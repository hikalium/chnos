#include "core.h"
#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))

void boxfill16(unsigned short *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1);
void init_scrn(unsigned short *vram, int xsize, int ysize);
void putfonts16_asc(unsigned short *vram, int xsize, int x, int y, unsigned short c, unsigned char *s);
void putfont16(unsigned short *vram, int xsize, int x, int y, unsigned short c, char *font);
void init_mouse_cursor16(short *mouse, unsigned short bc);
void putblock16_16(unsigned short *vram, int vxsize, int pxsize,int pysize, int px0, int py0, short *buf, int bxsize);

void CHNMain(void)
{	
	unsigned short mousecur[1024];
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	struct VESAINFO *vinfo = (struct VESAINFO *) 0x0e00;
	init_scrn(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrny);
	init_mouse_cursor16(mousecur, RGB16(5,10,5));
	putblock16_16(vinfo->PhysBasePtr, binfo->scrnx, 32, 32, binfo->scrnx/2, binfo->scrny/2, mousecur, 32);
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
	static char cursor[32][32] = {
		"***.............................",
		"*O**............................",
		"*OO**...........................",
		"*OOO**..........................",
		"*OOOO**.........................",
		"*OOOOO**........................",
		"*OOOOOO**.......................",
		"*OOOOOOO**......................",
		"*OOOOOOOO**.....................",
		"*OOOOOOOOO**....................",
		"*OOOOOOOOOO**...................",
		"*OOOOOOOOOOO**..................",
		"*OOOOOOOOOOOO**.................",
		"*OOOOOOOOOOOOO**................",
		"*OOOOOOOOOOOOOO**...............",
		"*OOOOOOOOOOOOOOO**..............",
		"*OOOOOOOOOOOOOOOO**.............",
		"*OOOOOOOOOOOOOOOOO**............",
		"*OOOOOOOOOOOOOOOOOO**...........",
		"*OOOOOOOOOOOOOOOOOOO**..........",
		"*OOOOOOOOOO************.........",
		"*OOOOOOOOOO*....................",
		"*OOOOOOOOOOO*...................",
		"*OOOOOOO*OOO*...................",
		"*OOOOOO**OOOO*..................",
		"*OOOOO**.*OOO*..................",
		"*OOOO**..*OOOO*.................",
		"*OOO**....*OOO*.................",
		"*OO**.....*OOO*.................",
		"*O**.......***..................",
		"***.............................",
		"**.............................."
	};
	int x, y;

	for (y = 0; y < 32; y++) {
		for (x = 0; x < 32; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 32 + x] = RGB16(0,0,0);
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 32 + x] = RGB16(31,62,31);
			}
			if (cursor[y][x] == '.') {
				mouse[y * 32 + x] = bc;
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

