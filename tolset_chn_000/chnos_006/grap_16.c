#include "core.h"
#include <math.h>
#include <string.h>

void boxfill16(unsigned int *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0;y <= y1;y++) {
		for (x = x0; x <= x1; x++)
		vram[y * xsize + x] = (unsigned int)c;
	}
	return;
}

void init_scrn16(unsigned int *vram, int xsize, int ysize ,unsigned int *mousecur)
{
	boxfill16(vram,xsize,DESKTOP_COL16,0,0,xsize,ysize);
	boxfill16(vram,xsize,TASKBAR_COL16,0,ysize-TASKBAR_HEIGHT,xsize,ysize);
	putfonts16_asc(vram, xsize, 8, 8, RGB16(31,62,31), "welcome to CHNOSProject! on 16bit video mode .");
	putfonts16_asc(vram, xsize, 8, 24, RGB16(31,62,31), "Ö³º¿ CHNOSÌßÛ¼Þª¸ÄÍ!");
	putfonts16_asc(vram, xsize, 8, 40, RGB16(31,62,31), "¶ÀºÄÃÞ½¶Þ ÆÎÝºÞ¶Þ ¶¹ÙÖ³Æ ÅØÏ¼À");
	init_mouse_cursor16(mousecur);
	return;

}

void putfont16(unsigned int *vram, int xsize, int x, int y, unsigned short c, unsigned char *font)
{
	int i;
	unsigned int *p;
	char d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = (unsigned int)c; }
		if ((d & 0x40) != 0) { p[1] = (unsigned int)c; }
		if ((d & 0x20) != 0) { p[2] = (unsigned int)c; }
		if ((d & 0x10) != 0) { p[3] = (unsigned int)c; }
		if ((d & 0x08) != 0) { p[4] = (unsigned int)c; }
		if ((d & 0x04) != 0) { p[5] = (unsigned int)c; }
		if ((d & 0x02) != 0) { p[6] = (unsigned int)c; }
		if ((d & 0x01) != 0) { p[7] = (unsigned int)c; }
	}
	return;
}

void putfonts16_asc(unsigned int *vram, int xsize, int x, int y, unsigned short c, unsigned char *s)
{
	extern char hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont16(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void init_mouse_cursor16(unsigned int *mouse)
{
	int x, y;

	for (y = 0; y < 24; y++) {
		for (x = 0; x < 24; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 24 + x] = (unsigned int)RGB16(0,0,0);
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 24 + x] = (unsigned int)RGB16(31,62,31);
			}
			if (cursor[y][x] == '.') {
				mouse[y * 24 + x] = (unsigned int)INV_COL16;
			}
		}
	}
	return;
}

void putblock16_16(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}
