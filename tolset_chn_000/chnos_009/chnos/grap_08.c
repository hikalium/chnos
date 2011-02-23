
#include "core.h"

void Draw_Put_Font_08(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font)
{
	int i;
	uchar d;
	uchar *p;
	for (i = 0; i < 16; i++) {
		p = (uchar *)(vram + (y + i) * xsize + x);
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = (uchar)c; }
		if ((d & 0x40) != 0) { p[1] = (uchar)c; }
		if ((d & 0x20) != 0) { p[2] = (uchar)c; }
		if ((d & 0x10) != 0) { p[3] = (uchar)c; }
		if ((d & 0x08) != 0) { p[4] = (uchar)c; }
		if ((d & 0x04) != 0) { p[5] = (uchar)c; }
		if ((d & 0x02) != 0) { p[6] = (uchar)c; }
		if ((d & 0x01) != 0) { p[7] = (uchar)c; }
	}
	return;
}

void Draw_Put_String_08(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s)
{
	c = RGB_32_To_08(c);
	for(; *s != 0x00; s++){
		Draw_Put_Font_08(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void Draw_Fill_Rectangle_08(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1)
{
	uint x, y;
	c = RGB_32_To_08(c);
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			((uchar *)vram)[y * xsize + x] = (uchar)c;
		}
	}
	return;
}

