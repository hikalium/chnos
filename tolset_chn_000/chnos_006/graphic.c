#include "core.h"
#include <math.h>
#include <string.h>
/*�ϊ��e�[�u��*/


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

/*�g���S���[�h�Ή���*/
void putfonts_asc_sht_i(struct SHEET32 *sht, int x, int y, unsigned int c, unsigned int bc, const char *s)
{
	int l;
	l = strlen(s);
	boxfill_i(sht->buf, sht->bxsize, bc, x, y, x + l * 8, y + 16);
	putfonts_asc_i(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x + l * 8, y + 16);
	return;
}

void init_scrn_i(unsigned int *vrami, int xsize, int ysize, unsigned char bits, unsigned int *mousecur)
{
	if(bits == 8){
	init_palette();
	init_scrn8(vrami, xsize, ysize ,mousecur);
	} else if(bits == 16){
	init_scrn16(vrami, xsize, ysize ,mousecur);
	} else if(bits == 32){
	init_scrn32(vrami, xsize, ysize ,mousecur);
	}
	col_pat_256safe(vrami,xsize,ysize);
	return;
}

unsigned int mix_color(unsigned int c0, unsigned int c1)
{
	float r0,g0,b0,r1,g1,b1,alpha;
	unsigned int cc;
	cc = c0 << 24;
	b0 = (float)(cc >> 24);
	cc = c0 << 16;
	g0 = (float)(cc >> 24);
	cc = c0 << 8;
	r0 = (float)(cc >> 24);

	cc = c1 << 24;
	b1 = (float)(cc >> 24);
	cc = c1 << 16;
	g1 = (float)(c1 >> 24);
	cc = c1 << 8;
	r1 = (float)(c1 >> 24);

	alpha = (float)(c1 >> 24);

	r1 = r1 * (alpha / 255) + r0 * (1 - (alpha / 255));
	g1 = g1 * (alpha / 255) + g0 * (1 - (alpha / 255));
	b1 = b1 * (alpha / 255) + b0 * (1 - (alpha / 255));

	c1 = (unsigned int) alpha;
	c1 = c1 << 8;
	c1 += (unsigned int) r1;
	c1 = c1 << 8;
	c1 += (unsigned int) g1;
	c1 = c1 << 8;
	c1 += (unsigned int) b1;

	return c1;
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
	unsigned char c8 = rgb_int2char(c);
	vrami[y * xsize + x] = (unsigned int)c8;
	} else if(vinfo->BitsPerPixel == 16){
	unsigned short c16 = rgb_int2short(c);
	vrami[y * xsize + x] = (unsigned int)c16;
	} else if(vinfo->BitsPerPixel == 32){
	vrami[y * xsize + x] = c;
	}
	return;
}

void boxfill_i(unsigned int *vrami, int xsize, unsigned int c, int x0, int y0, int x1, int y1)
{
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	y1 -= 1;
	if(vinfo->BitsPerPixel == 8){
	unsigned char c8 = rgb_int2char(c);
	boxfill8(vrami, xsize, c8, x0, y0, x1, y1);	
	} else if(vinfo->BitsPerPixel == 16){
	unsigned short c16 = rgb_int2short(c);
	boxfill16(vrami, xsize, c16, x0, y0, x1, y1);
	} else if(vinfo->BitsPerPixel == 32){
	boxfill32(vrami, xsize, c, x0, y0, x1, y1);
	}
	return;
}

void putfonts_asc_i(unsigned int *vrami, int xsize, int x, int y, unsigned int ci, const unsigned char *s)
{
	extern char hankaku[4096];
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	if(vinfo->BitsPerPixel == 8){
	unsigned char c8 = rgb_int2char(ci);
	for (; *s != 0x00; s++) {
		putfont8(vrami, xsize, x, y, c8, hankaku + *s * 16);
		x += 8;
	}
	return;
	} else if(vinfo->BitsPerPixel == 16){
	unsigned short c16 = rgb_int2short(ci);
	for (; *s != 0x00; s++) {
		putfont16(vrami, xsize, x, y, c16, hankaku + *s * 16);
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
	boxfill_i(vrami,xsize,0x000000,x,y,x+20,y+40);
	boxfill_i(vrami,xsize,mix_color(0x000000,0x7fff0000),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0xFF0000,x,y,x+20,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7fff0000,0x7f00ff00),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x00FF00,x,y,x+20,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7f00ff00,0x7fffff00),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0xFFFF00,x,y,x+20,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7fffff00,0x7f0000ff),x+20,y,x+40,y+40);

	y+=40;
	x=0;
	boxfill_i(vrami,xsize,0x0000FF,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7f0000ff,0x7fff00ff),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0xFF00FF,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7fff00ff,0x7f00ffff),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x00FFFF,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7f00ffff,0x7fffffff),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0xFFFFFF,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7fffffff,0x7fc6c6c6),x+20,y,x+40,y+40);

	y+=40;
	x=0;
	boxfill_i(vrami,xsize,0xC6C6C6,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7fc6c6c6,0x7fff0000),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x840000,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7fff0000,0x7f008400),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x008400,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7f008400,0x7f848400),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x848400,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7f848400,0x7f000084),x+20,y,x+40,y+40);

	y+=40;
	x=0;
	boxfill_i(vrami,xsize,0x000084,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x000000,0x7fff0000),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x840084,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7fff0000,0x7f840084),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x008484,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x7f840084,0x7f008484),x+20,y,x+40,y+40);
	x+=40;
	boxfill_i(vrami,xsize,0x848484,x,y,x+40,y+40);
	boxfill_i(vrami,xsize,mix_color(0x07f008484,0x7f848484),x+20,y,x+40,y+40);


	return;


}


/*8bits*/
void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:�� */
		0xff, 0x00, 0x00,	/*  1:���邢�� */
		0x00, 0xff, 0x00,	/*  2:���邢�� */
		0xff, 0xff, 0x00,	/*  3:���邢���F */
		0x00, 0x00, 0xff,	/*  4:���邢�� */
		0xff, 0x00, 0xff,	/*  5:���邢�� */
		0x00, 0xff, 0xff,	/*  6:���邢���F */
		0xff, 0xff, 0xff,	/*  7:�� */
		0xc6, 0xc6, 0xc6,	/*  8:���邢�D�F */
		0x84, 0x00, 0x00,	/*  9:�Â��� */
		0x00, 0x84, 0x00,	/* 10:�Â��� */
		0x84, 0x84, 0x00,	/* 11:�Â����F */
		0x00, 0x00, 0x84,	/* 12:�Â��� */
		0x84, 0x00, 0x84,	/* 13:�Â��� */
		0x00, 0x84, 0x84,	/* 14:�Â����F */
		0x84, 0x84, 0x84	/* 15:�Â��D�F */
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
	eflags = io_load_eflags();	/* ���荞�݋��t���O�̒l���L�^���� */
	io_cli(); 					/* ���t���O��0�ɂ��Ċ��荞�݋֎~�ɂ��� */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* ���荞�݋��t���O�����ɖ߂� */
	return;
}

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
	putfonts8_asc(vram, xsize, 8, 8, COL8_FFFFFF, "welcome to CHNOSProject! on 8bit video mode .");
	putfonts8_asc(vram, xsize, 8, 24, COL8_FFFFFF, "ֳ�� CHNOS��ۼު���!");
	putfonts8_asc(vram, xsize, 8, 40, COL8_FFFFFF, "�����޽�� ��ݺ޶� ���ֳ� ��ϼ�");
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
				mouse[y * 24 + x] = (unsigned int)INV_COL8;
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

/*16bits*/


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
	putfonts16_asc(vram, xsize, 8, 24, RGB16(31,62,31), "ֳ�� CHNOS��ۼު���!");
	putfonts16_asc(vram, xsize, 8, 40, RGB16(31,62,31), "�����޽�� ��ݺ޶� ���ֳ� ��ϼ�");
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
	putfonts32_asc(vram, xsize, 8, 24, 0xFFFFFF, "ֳ�� CHNOS��ۼު���!");
	putfonts32_asc(vram, xsize, 8, 40, 0xFFFFFF, "�����޽�� ��ݺ޶� ���ֳ� ��ϼ�");
	init_mouse_cursor32(mousecur);

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

void init_mouse_cursor32(unsigned int *mouse)
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








