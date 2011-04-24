
#include "core.h"

void Draw_Put_Font_32(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font)
{
	int i;
	uchar d;
	uint *p;
	for (i = 0; i < 16; i++) {
		p = (uint *)vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = (uint)c; }
		if ((d & 0x40) != 0) { p[1] = (uint)c; }
		if ((d & 0x20) != 0) { p[2] = (uint)c; }
		if ((d & 0x10) != 0) { p[3] = (uint)c; }
		if ((d & 0x08) != 0) { p[4] = (uint)c; }
		if ((d & 0x04) != 0) { p[5] = (uint)c; }
		if ((d & 0x02) != 0) { p[6] = (uint)c; }
		if ((d & 0x01) != 0) { p[7] = (uint)c; }
	}
	return;
}

void Draw_Put_String_32(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s)
{
	for(; *s != 0x00; s++){
		if(x > xsize - 8){
			break;
		}
		Draw_Put_Font_32(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void Draw_Fill_Rectangle_32(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1)
{
	uint x, y;
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			((uint *)vram)[y * xsize + x] = c;
		}
	}
	return;
}

void Draw_Slide_Line_32(void *vram, uint xsize, uint ysize, uint vxsize, uint px, uint py)
{
	uint x, y;

	for(y = py; y < ysize + py - 16; y++){
		for(x = px; x < xsize + px; x++){
			((uint *)vram)[y * vxsize + x] = ((uint *)vram)[(y + 16) * vxsize + x];
		}
	}
	return;
}
