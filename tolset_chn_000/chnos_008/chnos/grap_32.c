
#include "core.h"

void boxfill32(uint *vram, int xsize, uint c, int x0, int y0, int x1, int y1)
{
	int x, y;

	y1 -= 1;
	x1 -= 1;

	for (y = y0;y <= y1;y++) {
		for (x = x0; x <= x1; x++)
		vram[y * xsize + x] = c;
	}

	return;
}

void init_desktop32(uint *vram, uint xsize, uint ysize)
{
	boxfill32(vram, xsize, DESKTOP_COL32, 0, 0, xsize, ysize);

	return;	
}

void init_taskbar32(uint *vram, uint xsize)
{
	boxfill32(vram, xsize, 0xFFFFFF, 0, 0, xsize, TASKBAR_HEIGHT);
	boxfill32(vram, xsize, 0xFFFFFF, 0, 0, xsize - 2, TASKBAR_HEIGHT - 2);
	boxfill32(vram, xsize, TASKBAR_COL32, 2, 2, xsize - 2, TASKBAR_HEIGHT - 2);

	return;
}

void putfont32(uint *vram, int xsize, int x, int y, uint c, uchar *font)
{
	int i;
	int *p;
	char d;

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

void putfonts32_asc(uint *vram, int xsize, int x, int y, uint c, const uchar *s)
{
	for (; *s != 0x00; s++) {
		putfont32(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}

	return;
}

void init_mouse_cursor32(uint *mouse)
{
	int x, y;

	for (y = 0; y < 24; y++) {
		for (x = 0; x < 24; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 24 + x] = 0x55000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 24 + x] = 0x55FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 24 + x] = INV_COL32;
			}
		}
	}

	return;
}

void putblock32_32(uint *vram, int vxsize, int pxsize,int pysize, int px0, int py0, uint *buf, int bxsize)
{
	int x, y;

	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}

	return;
}
