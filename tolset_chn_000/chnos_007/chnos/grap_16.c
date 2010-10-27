
#include "core.h"
#include <math.h>
#include <string.h>

void boxfill16(ushort *vram, int xsize, ushort c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0;y <= y1;y++) {
		for (x = x0; x <= x1; x++)
		vram[y * xsize + x] = c;
	}
	return;
}
void init_desktop16(ushort *vram)
{
	uint xsize = system.sys.xsize;
	uint ysize = system.sys.ysize;

	boxfill16(vram,xsize,DESKTOP_COL16,0,0,xsize,ysize);
	putfonts16_asc(vram, xsize, 168, 8, RGB16(31,62,31), "welcome to CHNOSProject! on 16bit video mode .");
	putfonts16_asc(vram, xsize, 168, 24, RGB16(31,62,31), "ֳ�� CHNOS��ۼު���!");
	putfonts16_asc(vram, xsize, 168, 40, RGB16(31,62,31), "�����޽�� ��ݺ޶� ���ֳ� ��ϼ�");

	return;	
}

void init_taskbar16(ushort *vram)
{
	uint xsize = system.sys.xsize;

	boxfill16(vram, xsize, TASKBAR_COL16, 0, 0, xsize, TASKBAR_HEIGHT);
	boxfill16(vram, xsize, RGB16(31,62,31), 0, 0, xsize, 1);
	boxfill16(vram, xsize, RGB16(31,62,31), xsize-2, 0, xsize, TASKBAR_HEIGHT);

	return;
}
void putfont16(ushort *vram, int xsize, int x, int y, ushort c, uchar *font)
{
	int i;
	ushort *p;
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

void putfonts16_asc(ushort *vram, int xsize, int x, int y, ushort c, const uchar *s)
{
	extern char hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont16(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void init_mouse_cursor16(ushort *mouse)
{
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
				mouse[y * 24 + x] = INV_COL16;
			}
		}
	}
	return;
}

void putblock16_16(ushort *vram, int vxsize, int pxsize,int pysize, int px0, int py0, ushort *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}
