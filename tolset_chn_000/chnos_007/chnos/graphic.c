
#include "core.h"
#include <math.h>
#include <string.h>
/*ColorTable*/


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
char cursor[24][24] = {
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

void putfonts_asc_sht_i(struct SHEET32 *sht, int x, int y, unsigned int c, unsigned int bc, const unsigned char *s)
{
	int l;
	l = strlen(s);
	boxfill_i(sht->buf, sht->bxsize, bc, x, y, x + l * 8, y + 16);
	putfonts_asc_i(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x + l * 8, y + 16);
	return;
}

void init_screen_i(unsigned int *desktop, unsigned int *taskbar, unsigned int *mousecursor)
{
	if(system.sys.bpp == 8){
		init_palette();
	}

	init_desktop_i(desktop);
	init_taskbar_i(taskbar);
	init_mousecursor_i(mousecursor);

	col_pat(desktop,system.sys.xsize, system.sys.ysize);
}

void init_desktop_i(unsigned int *vrami)
{
	if(system.sys.bpp == 8){
		init_desktop8(vrami);
	} else if(system.sys.bpp == 16){
		init_desktop16(vrami);
	} else if(system.sys.bpp == 32){
		init_desktop32(vrami);
	}
	return;	
}

void init_taskbar_i(unsigned int *vrami)
{
	if(system.sys.bpp == 8){
		init_taskbar8(vrami);
	} else if(system.sys.bpp == 16){
		init_taskbar16(vrami);
	} else if(system.sys.bpp == 32){
		init_taskbar32(vrami);
	}
	return;	
}

void init_mousecursor_i(unsigned int *vrami)
{
	if(system.sys.bpp == 8){
		init_mouse_cursor8(vrami);
	} else if(system.sys.bpp == 16){
		init_mouse_cursor16(vrami);
	} else if(system.sys.bpp == 32){
		init_mouse_cursor32(vrami);
	}
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
{
	if(system.info.vesa.BitsPerPixel == 8){
		unsigned char c8 = rgb_int2char(c);
		vrami[y * xsize + x] = (unsigned int)c8;
	} else if(system.info.vesa.BitsPerPixel == 16){
		unsigned short c16 = rgb_int2short(c);
		vrami[y * xsize + x] = (unsigned int)c16;
	} else if(system.info.vesa.BitsPerPixel == 32){
		vrami[y * xsize + x] = c;
	}
	return;
}

void boxfill_i(unsigned int *vrami, int xsize, unsigned int c, int x0, int y0, int x1, int y1)
{
	y1 -= 1;
	if(system.info.vesa.BitsPerPixel == 8){
		unsigned char c8 = rgb_int2char(c);
		boxfill8(vrami, xsize, c8, x0, y0, x1, y1);	
	} else if(system.info.vesa.BitsPerPixel == 16){
		unsigned short c16 = rgb_int2short(c);
		boxfill16(vrami, xsize, c16, x0, y0, x1, y1);
	} else if(system.info.vesa.BitsPerPixel == 32){
		boxfill32(vrami, xsize, c, x0, y0, x1, y1);
	}
	return;
}

void putfonts_asc_i(unsigned int *vrami, int xsize, int x, int y, unsigned int ci, const unsigned char *s)
{
	extern char hankaku[4096];
	if(system.info.vesa.BitsPerPixel == 8){
		unsigned char c8 = rgb_int2char(ci);
		for (; *s != 0x00; s++) {
			putfont8(vrami, xsize, x, y, c8, hankaku + *s * 16);
			x += 8;
		}
		return;
	} else if(system.info.vesa.BitsPerPixel == 16){
		unsigned short c16 = rgb_int2short(ci);
		for (; *s != 0x00; s++) {
			putfont16(vrami, xsize, x, y, c16, hankaku + *s * 16);
			x += 8;
		}
		return;
	} else if(system.info.vesa.BitsPerPixel == 32){
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

void col_pat(unsigned int *vrami, int xsize, int ysize)
{
	int x,y;
	x=0;
	y=0;
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

