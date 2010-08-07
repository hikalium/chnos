
#include "core.h"
#include <math.h>
#include <string.h>

void boxfill8(unsigned int *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = (unsigned int)c;
	}
	return;
}

void init_scrn8(unsigned int *vram, int xsize, int ysize, unsigned int *mousecur)
{
	boxfill8(vram,xsize,DESKTOP_COL8,0,0,xsize,ysize);
	boxfill8(vram,xsize,TASKBAR_COL8,0,ysize-TASKBAR_HEIGHT,xsize,ysize);
	putfonts8_asc(vram, xsize, 168, 8, COL8_FFFFFF, "welcome to CHNOSProject! on 8bit video mode .");
	putfonts8_asc(vram, xsize, 168, 24, COL8_FFFFFF, "Ö³º¿ CHNOSÌßÛ¼Þª¸ÄÍ!");
	putfonts8_asc(vram, xsize, 168, 40, COL8_FFFFFF, "¶ÀºÄÃÞ½¶Þ ÆÎÝºÞ¶Þ ¶¹ÙÖ³Æ ÅØÏ¼À");
	init_mouse_cursor8(mousecur);
	return;

}

void putfont8(unsigned int *vram, int xsize, int x, int y, unsigned char c, unsigned char *font)
{
	int i;
	char d /* data */;
	unsigned int *p;
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

void putfonts8_asc(unsigned int *vram, int xsize, int x, int y, unsigned char c, unsigned char *s)
{
	extern char hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void init_mouse_cursor8(unsigned int *mouse)
{
	int x, y;

	for (y = 0; y < 24; y++) {
		for (x = 0; x < 24; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 24 + x] = (unsigned int)COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 24 + x] = (unsigned int)COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 24 + x] = INV_COL;
			}
		}
	}
	return;
}

void putblock8_8(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,
		0xff, 0x00, 0x00,
		0x00, 0xff, 0x00,
		0xff, 0xff, 0x00,
		0x00, 0x00, 0xff,
		0xff, 0x00, 0xff,
		0x00, 0xff, 0xff,
		0xff, 0xff, 0xff,
		0xc6, 0xc6, 0xc6,
		0x84, 0x00, 0x00,
		0x00, 0x84, 0x00,
		0x84, 0x84, 0x00,
		0x00, 0x00, 0x84,
		0x84, 0x00, 0x84,
		0x00, 0x84, 0x84,
		0x84, 0x84, 0x84
	};
	unsigned char table2[216 * 3];
	int r, g, b;
	set_palette(0, 15, table_rgb);
	for (b = 0; b < 6; b++) {
		for (g = 0; g < 6; g++) {
			for (r = 0; r < 6; r++) {
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}
	set_palette(16, 231, table2);
	return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();
	io_cli(); 
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);
	return;
}
