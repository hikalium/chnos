#include "core.h"
#include <math.h>
/*変換テーブル*/


static int rgb_int2char_list [16] = {
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
static short rgb_char2short_list[16] = {
	RGB16(0,0,0),
	RGB16(31,0,0),
	RGB16(0,62,0),
	RGB16(31,62,0),
	RGB16(0,0,31),
	RGB16(31,0,31),
	RGB16(0,62,31),
	RGB16(31,62,31),
	RGB16(25,50,25),
	RGB16(17,0,0),
	RGB16(0,33,0),
	RGB16(17,33,0),
	RGB16(0,0,17),
	RGB16(17,0,17),
	RGB16(0,33,17),
	RGB16(17,33,17)
	};

/*拡張全モード対応版*/
void init_scrn_i(unsigned int *vrami, int xsize, int ysize, unsigned char bits)
{
	unsigned int mousecur32 [576];
	if(bits == 8){
	unsigned char *mousecur8 = (unsigned char *)mousecur32;
	unsigned char *vram8 = (unsigned char *)vrami;
	init_palette();
	init_scrn8(vram8, xsize, ysize ,mousecur8);
	} else if(bits == 16){
	unsigned short *mousecur16 = (unsigned short *)mousecur32;
	unsigned short *vram16 = (unsigned short *)vrami;
	init_scrn16(vram16, xsize, ysize ,mousecur16);
	} else if(bits == 32){

	init_scrn32(vrami, xsize, ysize ,mousecur32);
	}
	col_pat_256safe(vrami,xsize,ysize);
	return;
}

void circle_i(unsigned int *vrami, int cx, int cy, unsigned int c, int xsize, int r)
{
	double pi = 3.1415;
	int c_div = 100;
	double incr = 2 * pi / c_div;
	double th = 0;
	int i,x,y;
	for (i = 0;i <= c_div;i++){
		x = r * cos(th);
		y = r * sin(th);
		point_i(vrami,cx+x,cy+y,c,xsize);
		th += incr;
	}

}

void point_i(unsigned int *vrami, int x, int y, unsigned int c, int xsize)
{	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	if(vinfo->BitsPerPixel == 8){
	unsigned char *vram8 = (unsigned char *)vrami;
	unsigned char c8 = rgb_int2char(c);
	vram8[y * xsize + x] = c8;
	} else if(vinfo->BitsPerPixel == 16){
	unsigned short *vram16 = (unsigned short *)vrami;
	unsigned short c16 = rgb_int2short(c);
	vram16[y * xsize + x] = c16;
	} else if(vinfo->BitsPerPixel == 32){
	vrami[y * xsize + x] = c;
	}
	return;
}

void draw_mouse_i(unsigned int *vrami, int x, int y, int xsize)
{
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	unsigned int mousecur32 [576];
	if(vinfo->BitsPerPixel == 8){
	unsigned char *mousecur8 = (unsigned char *)mousecur32;
	unsigned char *vram8 = (unsigned char *)vrami;
	init_mouse_cursor8(mousecur8, DESKTOP_COL8);
	putblock8_8(vram8, xsize,24, 24, x, y, mousecur8, 24);
	} else if(vinfo->BitsPerPixel == 16){
	unsigned short *mousecur16 = (unsigned short *)mousecur32;
	unsigned short *vram16 = (unsigned short *)vrami;
	init_mouse_cursor16(mousecur16, DESKTOP_COL16);
	putblock16_16(vram16, xsize,24, 24, x, y, mousecur16, 24);
	} else if(vinfo->BitsPerPixel == 32){
	init_mouse_cursor32(mousecur32, DESKTOP_COL32);
	putblock32_32(vrami, xsize,24, 24, x, y, mousecur32, 24);
	}
	return;
}
void boxfill_i(unsigned int *vrami, int xsize, unsigned int c, int x0, int y0, int x1, int y1)
{
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	if(vinfo->BitsPerPixel == 8){
	unsigned char *vram8 = (unsigned char *)vrami;
	unsigned char c8 = rgb_int2char(c);
	boxfill8(vram8, xsize, c8, x0, y0, x1, y1);	
	} else if(vinfo->BitsPerPixel == 16){
	unsigned short *vram16 = (unsigned short *)vrami;
	unsigned short c16 = rgb_int2short(c);
	boxfill16(vram16, xsize, c16, x0, y0, x1, y1);
	} else if(vinfo->BitsPerPixel == 32){
	boxfill32(vrami, xsize, c, x0, y0, x1, y1);
	}
	return;
}

void putfonts_asc_i(unsigned int *vrami, int xsize, int x, int y, unsigned int ci, unsigned char *s)
{
	extern char hankaku[4096];
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	if(vinfo->BitsPerPixel == 8){
	unsigned char *vram8 = (unsigned char *)vrami;
	unsigned char c8 = rgb_int2char(ci);
	for (; *s != 0x00; s++) {
		putfont8(vram8, xsize, x, y, c8, hankaku + *s * 16);
		x += 8;
	}
	return;
	} else if(vinfo->BitsPerPixel == 16){
	unsigned short *vram16 = (unsigned short *)vrami;
	unsigned short c16 = rgb_int2short(ci);
	for (; *s != 0x00; s++) {
		putfont16(vram16, xsize, x, y, c16, hankaku + *s * 16);
		x += 8;
	}
	return;
	} else if(vinfo->BitsPerPixel == 32){
	for (; *s != 0x00; s++) {
		putfont32(vrami, xsize, x, y, ci, hankaku + *s * 16);
		x += 8;
		}
	}
	return;
}

unsigned char rgb_int2char (unsigned int c32)
{
unsigned char i ;
for(i = 0;i < 15; i++) {
	if(rgb_int2char_list[i] == c32) {
		return i;
		}
	}
return 7;
}

unsigned short rgb_int2short (unsigned int c32)
{
	unsigned short c16;
	unsigned char c8;
	c8 = rgb_int2char(c32);
	c16 = rgb_char2short_list[c8];
	return c16;
}

void col_pat_256safe(unsigned int *vrami, int xsize, int ysize)
{
	int x,y;
	x=0;
	y=80;
	boxfill_i(vrami,xsize,0x000000,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0xFF0000,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x00FF00,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0xFFFF00,x,y,x+40,y+40);

	y+=40;
	x=0;
	boxfill_i(vrami,xsize,0x0000FF,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0xFF00FF,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x00FFFF,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0xFFFFFF,x,y,x+40,y+40);

	y+=40;
	x=0;
	boxfill_i(vrami,xsize,0xC6C6C6,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x840000,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x008400,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x848400,x,y,x+40,y+40);

	y+=40;
	x=0;
	boxfill_i(vrami,xsize,0x000084,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x840084,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x008484,x,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x848484,x,y,x+40,y+40);


	return;


}


/*8bits*/
void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黒 */
		0xff, 0x00, 0x00,	/*  1:明るい赤 */
		0x00, 0xff, 0x00,	/*  2:明るい緑 */
		0xff, 0xff, 0x00,	/*  3:明るい黄色 */
		0x00, 0x00, 0xff,	/*  4:明るい青 */
		0xff, 0x00, 0xff,	/*  5:明るい紫 */
		0x00, 0xff, 0xff,	/*  6:明るい水色 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:明るい灰色 */
		0x84, 0x00, 0x00,	/*  9:暗い赤 */
		0x00, 0x84, 0x00,	/* 10:暗い緑 */
		0x84, 0x84, 0x00,	/* 11:暗い黄色 */
		0x00, 0x00, 0x84,	/* 12:暗い青 */
		0x84, 0x00, 0x84,	/* 13:暗い紫 */
		0x00, 0x84, 0x84,	/* 14:暗い水色 */
		0x84, 0x84, 0x84	/* 15:暗い灰色 */
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
	eflags = io_load_eflags();	/* 割り込み許可フラグの値を記録する */
	io_cli(); 					/* 許可フラグを0にして割り込み禁止にする */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 割り込み許可フラグを元に戻す */
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void init_scrn8(unsigned char *vram, int xsize, int ysize, unsigned char *mousecur)
{
	boxfill8(vram,xsize,DESKTOP_COL8,0,0,xsize,ysize);
	boxfill8(vram,xsize,TASKBAR_COL8,0,ysize-TASKBAR_HEIGHT,xsize,ysize);
	putfonts8_asc(vram, xsize, 8, 8, COL8_FFFFFF, "welcome to CHNOSProject! on 8bit video mode .");
	putfonts8_asc(vram, xsize, 8, 24, COL8_FFFFFF, "ﾖｳｺｿ CHNOSﾌﾟﾛｼﾞｪｸﾄﾍ!");
	putfonts8_asc(vram, xsize, 8, 40, COL8_FFFFFF, "ｶﾀｺﾄﾃﾞｽｶﾞ ﾆﾎﾝｺﾞｶﾞ ｶｹﾙﾖｳﾆ ﾅﾘﾏｼﾀ");
	init_mouse_cursor8(mousecur, DESKTOP_COL8);
	putblock8_8(vram, xsize,24, 24, xsize/2, ysize/2, mousecur, 24);
	return;
}

void putfont8(unsigned char *vram, int xsize, int x, int y, unsigned char c, unsigned char *font)
{
	int i;
	char *p, d /* data */;
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

void putfonts8_asc(unsigned char *vram, int xsize, int x, int y, unsigned char c, unsigned char *s)
{
	extern char hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void init_mouse_cursor8(unsigned char *mouse, unsigned char bc)
{
	static char cursor[24][24] = {
		"***.....................",
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
				mouse[y * 24 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 24 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 24 + x] = bc;
			}
		}
	}
	return;
}

void putblock8_8(unsigned char *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned char *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

/*16bits*/


void boxfill16(unsigned short *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0;y <= y1;y++) {
		for (x = x0; x <= x1; x++)
		vram[y * xsize + x] = c;
	}
	return;
}

void init_scrn16(unsigned short *vram, int xsize, int ysize ,unsigned short *mousecur)
{
	boxfill16(vram,xsize,DESKTOP_COL16,0,0,xsize,ysize);
	boxfill16(vram,xsize,TASKBAR_COL16,0,ysize-TASKBAR_HEIGHT,xsize,ysize);
	putfonts16_asc(vram, xsize, 8, 8, RGB16(31,62,31), "welcome to CHNOSProject! on 16bit video mode .");
	putfonts16_asc(vram, xsize, 8, 24, RGB16(31,62,31), "ﾖｳｺｿ CHNOSﾌﾟﾛｼﾞｪｸﾄﾍ!");
	putfonts16_asc(vram, xsize, 8, 40, RGB16(31,62,31), "ｶﾀｺﾄﾃﾞｽｶﾞ ﾆﾎﾝｺﾞｶﾞ ｶｹﾙﾖｳﾆ ﾅﾘﾏｼﾀ");
	init_mouse_cursor16(mousecur, DESKTOP_COL16);
	putblock16_16(vram, xsize, 24, 24, xsize/2, ysize/2, mousecur, 24);
	return;

}

void putfont16(unsigned short *vram, int xsize, int x, int y, unsigned short c, unsigned char *font)
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

void putfonts16_asc(unsigned short *vram, int xsize, int x, int y, unsigned short c, unsigned char *s)
{
	extern char hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont16(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void init_mouse_cursor16(unsigned short *mouse, unsigned short bc)
{
	static char cursor[24][24] = {
		"***.....................",
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

void putblock16_16(unsigned short *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned short *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}


/*32bits*/
void boxfill32(unsigned int *vram, int xsize, unsigned int c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0;y <= y1;y++) {
		for (x = x0; x <= x1; x++)
		vram[y * xsize + x] = c;
	}
	return;
}


void init_scrn32(unsigned int *vram, int xsize, int ysize, unsigned int *mousecur)
{
	boxfill32(vram,xsize,DESKTOP_COL32,0,0,xsize,ysize);
	boxfill32(vram,xsize,TASKBAR_COL32,0,ysize-TASKBAR_HEIGHT,xsize,ysize);
	putfonts32_asc(vram, xsize, 8, 8, 0xFFFFFF, "welcome to CHNOSProject! on 32bit video mode .");
	putfonts32_asc(vram, xsize, 8, 24, 0xFFFFFF, "ﾖｳｺｿ CHNOSﾌﾟﾛｼﾞｪｸﾄﾍ!");
	putfonts32_asc(vram, xsize, 8, 40, 0xFFFFFF, "ｶﾀｺﾄﾃﾞｽｶﾞ ﾆﾎﾝｺﾞｶﾞ ｶｹﾙﾖｳﾆ ﾅﾘﾏｼﾀ");
	init_mouse_cursor32(mousecur, DESKTOP_COL32);
	putblock32_32(vram, xsize, 24, 24, xsize/2, ysize/2, mousecur, 24);
	return;

}

void putfont32(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *font)
{
	int i;
	int *p;
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


void putfonts32_asc(unsigned int *vram, int xsize, int x, int y, unsigned int c, unsigned char *s)
{
	extern char hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont32(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void init_mouse_cursor32(unsigned int *mouse, unsigned int bc)
{
	static char cursor[24][24] = {
		"***.....................",
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
				mouse[y * 24 + x] = 0x000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 24 + x] = 0xFFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 24 + x] = bc;
			}
		}
	}
	return;
}

void putblock32_32(unsigned int *vram, int vxsize, int pxsize,int pysize, int px0, int py0, unsigned int *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}


