
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
		if(x > xsize - 8){
			break;
		}
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

void Draw_Slide_Line_08(void *vram, uint xsize, uint ysize, uint vxsize, uint px, uint py)
{
	uint x, y;

	for(y = py; y < ysize + py - 16; y++){
		for(x = px; x < xsize + px; x++){
			((uchar *)vram)[y * vxsize + x] = ((uchar *)vram)[(y + 16) * vxsize + x];
		}
	}
	return;
}

void Draw_Initialise_Palette(void)
{
	static uchar table_rgb[16 * 3] = {
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
	uchar table2[216 * 3];
	int r, g, b;

	Draw_Set_Palette(0, 15, table_rgb);
	for (b = 0; b < 6; b++) {
		for (g = 0; g < 6; g++) {
			for (r = 0; r < 6; r++) {
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}
	Draw_Set_Palette(16, 231, table2);

	return;
}

void Draw_Set_Palette(int start, int end, uchar *rgb)
{
	int i, eflags;

	eflags = IO_Load_EFlags();
	IO_CLI(); 
	IO_Out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		IO_Out8(0x03c9, rgb[0] >> 2);
		IO_Out8(0x03c9, rgb[1] >> 2);
		IO_Out8(0x03c9, rgb[2] >> 2);
		rgb += 3;
	}
	IO_Store_EFlags(eflags);
	return;
}
