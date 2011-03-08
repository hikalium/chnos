
#include "core.h"

uint RGB_32_To_08_Table[16] = {
	0x000000,
	0xff0000,
	0x00ff00,
	0xffff00,
	0x0000ff,
	0xff00ff,
	0x00ffff,
	0xffffff,
	0xc6c6c6,
	0x840000,
	0x008400,
	0x848400,
	0x000084,
	0x840084,
	0x008484,
	0x848484
};

void (*Draw_Put_String)(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s);
void (*Draw_Fill_Rectangle)(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void (*Draw_Slide_Line)(void *vram, uint xsize, uint ysize, uint vxsize, uint px, uint py);

void Initialise_Graphic(uint bpp)
{
	if(bpp == 8){
		Draw_Initialise_Palette();
		Draw_Put_String = Draw_Put_String_08;
		Draw_Fill_Rectangle = Draw_Fill_Rectangle_08;
		Draw_Slide_Line = Draw_Slide_Line_08;
	} else if(bpp == 16){
		Draw_Put_String = Draw_Put_String_16;
		Draw_Fill_Rectangle = Draw_Fill_Rectangle_16;
		Draw_Slide_Line = Draw_Slide_Line_16;
	} else if(bpp == 32){
		Draw_Put_String = Draw_Put_String_32;
		Draw_Fill_Rectangle = Draw_Fill_Rectangle_32;
		Draw_Slide_Line = Draw_Slide_Line_32;
	}
	return;
}

uchar RGB_32_To_08(uint c32)
{
	uchar i ;
	for(i = 0;i < 15; i++) {
		if(RGB_32_To_08_Table[i] == c32) {
			return i;
		}
	}
	return 8; //���݂��Ȃ��F�͊D�F�ɁB
}

ushort RGB_32_To_16(uint c32)
{
	ushort c16;
	uchar c[4];

	c[3] = (c32 << 24) >> 24;
	c[2] = (c32 << 16) >> 24;
	c[1] = (c32 <<  8) >> 24;
	c[0] = (c32 >> 24);

	c16 = ((c[1] >> 3) << 11 | (c[2] >> 2) << 5 | (c[3] >> 3));
	
	return c16;
}

