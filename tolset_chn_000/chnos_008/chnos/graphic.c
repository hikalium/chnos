
#include "core.h"

/*ColorTable*/
uint rgb_int2char_list [16] = {
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

void putfonts_asc_sht_i(UI_Sheet *sht, int x, int y, uint c, uint bc, const uchar *s)
{
	int l;
	l = strlen(s);
	boxfill_i(sht->buf, sht->bxsize, bc, x, y, x + l * 8, y + 16);
	putfonts_asc_i(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x + l * 8, y + 16);
	return;
}

void putfonts_asc_sht_i_no_bc(UI_Sheet *sht, int x, int y, uint c, const uchar *s)
{
	int l;
	l = strlen(s);
	putfonts_asc_i(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x + l * 8, y + 16);
	return;
}

void init_screen_i(void *desktop, void *taskbar, void *mousecursor)
{
	uchar bpp;

	bpp = system.data.info.vesa.BitsPerPixel;

	if(bpp == 8){
		init_palette();
	}
	init_desktop_i(desktop);
	init_taskbar_i(taskbar);
	init_mousecursor_i(mousecursor);

	return;
}

void init_desktop_i(void *vrami)
{
	uchar bpp;
	uint xsize, ysize;

	bpp = system.data.info.vesa.BitsPerPixel;
	xsize = system.data.info.boot.scrnx;
	ysize = system.data.info.boot.scrny;

	if(bpp == 8){
		init_desktop8(vrami, xsize, ysize);
	} else if(bpp == 16){
		init_desktop16(vrami, xsize, ysize);
	} else if(bpp == 32){
		init_desktop32(vrami, xsize, ysize);
	}
	return;	
}

void init_taskbar_i(void *vrami)
{
	uchar bpp;
	uint xsize;

	bpp = system.data.info.vesa.BitsPerPixel;
	xsize = system.data.info.boot.scrnx;

	if(bpp == 8){
		init_taskbar8(vrami, xsize);
	} else if(bpp == 16){
		init_taskbar16(vrami, xsize);
	} else if(bpp == 32){
		init_taskbar32(vrami, xsize);
	}
	return;	
}

void init_mousecursor_i(void *vrami)
{
	uchar bpp;

	bpp = system.data.info.vesa.BitsPerPixel;

	if(bpp == 8){
		init_mouse_cursor8(vrami);
	} else if(bpp == 16){
		init_mouse_cursor16(vrami);
	} else if(bpp == 32){
		init_mouse_cursor32(vrami);
	}
	return;	
}

uint mix_color(uint c0, uint c1)
{
	float r0,g0,b0,r1,g1,b1,alpha;
	uint cc;

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

	c1 = (uint) alpha;
	c1 = c1 << 8;
	c1 += (uint) r1;
	c1 = c1 << 8;
	c1 += (uint) g1;
	c1 = c1 << 8;
	c1 += (uint) b1;

	return c1;
}

void point_i(void *vrami, int x, int y, uint c, int xsize)
{
	uint cc = 0x00000000;

	if(system.data.info.vesa.BitsPerPixel == 8){
		cc = rgb_int2char(c);
	} else if(system.data.info.vesa.BitsPerPixel == 16){
		cc = rgb_int2short(c);
	} else if(system.data.info.vesa.BitsPerPixel == 32){
		cc = c;
	}
	point_bpp(vrami, x, y, cc, xsize, system.data.info.vesa.BitsPerPixel);
	return;
}

void point_bpp(void *vrami, int x, int y, uint c, int xsize, uint bpp)
{
	uchar *buf8;
	ushort *buf16;
	uint *buf32;

	if(bpp == 8){
		buf8 = vrami;
		buf8[y * xsize + x] = (uchar)c;
	} else if(bpp == 16){
		buf16 = vrami;
		buf16[y * xsize + x] = (ushort)c;
	} else if(bpp == 32){
		buf32 = vrami;
		buf32[y * xsize + x] = (uint)c;
	}

	return;
}

void boxfill_i(void *vrami, int xsize, uint c, int x0, int y0, int x1, int y1)
{
	uint cc = 0x00000000;

	if(system.data.info.vesa.BitsPerPixel == 8){
		cc = rgb_int2char(c);
	} else if(system.data.info.vesa.BitsPerPixel == 16){
		cc = rgb_int2short(c);
	} else if(system.data.info.vesa.BitsPerPixel == 32){
		cc = c;
	}
	boxfill_bpp(vrami, xsize, cc, x0, y0, x1, y1, system.data.info.vesa.BitsPerPixel);
	return;
}

void boxfill_bpp(void *vrami, int xsize, uint c, int x0, int y0, int x1, int y1, uint bpp)
{
	if(bpp == 8){
		boxfill8(vrami, xsize, c, x0, y0, x1, y1);	
	} else if(bpp == 16){
		boxfill16(vrami, xsize, c, x0, y0, x1, y1);
	} else if(bpp == 32){
		boxfill32(vrami, xsize, c, x0, y0, x1, y1);
	}

	return;
}

void putfonts_asc_i(void *vrami, int xsize, int x, int y, uint c, const uchar *s)
{
	if(system.data.info.vesa.BitsPerPixel == 8){
		uchar c8 = rgb_int2char(c);
		for (; *s != 0x00; s++) {
			putfont8(vrami, xsize, x, y, c8, hankaku + *s * 16);
			x += 8;
		}
		return;
	} else if(system.data.info.vesa.BitsPerPixel == 16){
		ushort c16 = rgb_int2short(c);
		for (; *s != 0x00; s++) {
			putfont16(vrami, xsize, x, y, c16, hankaku + *s * 16);
			x += 8;
		}
		return;
	} else if(system.data.info.vesa.BitsPerPixel == 32){
		for (; *s != 0x00; s++) {
			putfont32(vrami, xsize, x, y, c, hankaku + *s * 16);
			x += 8;
		}
	}

	return;
}

void putblock_i(void *vrami, int vxsize, int pxsize, int pysize, int px0, int py0, void *buf, int bxsize)
{
	if(system.data.info.vesa.BitsPerPixel == 8){
		putblock8_8(vrami, vxsize, pxsize, pysize, px0, py0, buf, bxsize);
	} else if(system.data.info.vesa.BitsPerPixel == 16){
		putblock16_16(vrami, vxsize, pxsize, pysize, px0, py0, buf, bxsize);
	} else if(system.data.info.vesa.BitsPerPixel == 32){
		putblock32_32(vrami, vxsize, pxsize, pysize, px0, py0, buf, bxsize);
	}

	return;
}

void putblock_i_convert(void *to, int xsize, int px0, int py0, int px1, int py1, void *from, int tobpp, int frombpp)
{
	int x, y;

	if(tobpp == 32){
		if(frombpp == 8){
			for(y = py0; y < py1; y++){
				for(x = px0; x < px1; x++){
					((uint *)to)[(xsize * y) + x] = rgb_int2char_list[((uchar *)from)[(xsize * y) + x]];
				}
			}
		}
	}

	return;
}

void line_i(void *vrami, int xsize, int x0, int y0, int x1, int y1, uint c)
{
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if(dx < 0) dx = -dx;
	if(dy < 0) dy = -dy;
	if(dx >= dy){
		len = dx + 1;
		if(x0 > x1) dx = -1024;
		else dx = 1024;
		if(y0 <= y1) dy = ((y1 - y0 + 1) << 10) / len;
		else dy = ((y1 - y0 - 1) << 10) / len;
	} else{
		len = dy + 1;
		if(y0 > y1) dy = -1024;
		else dy = 1024;
		if(x0 <= x1) dx = ((x1 - x0 + 1) << 10) / len;
		else dx = ((x1 - x0 - 1) << 10) / len;
	}
	for(i = 0; i < len; i++){
		point_i(vrami, x >> 10, y >> 10, c, xsize);
		x += dx;
		y += dy;
	}

	return;
}

void line_bpp(void *vrami, int xsize, int x0, int y0, int x1, int y1, uint c, uint bpp)
{
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if(dx < 0) dx = -dx;
	if(dy < 0) dy = -dy;
	if(dx >= dy){
		len = dx + 1;
		if(x0 > x1) dx = -1024;
		else dx = 1024;
		if(y0 <= y1) dy = ((y1 - y0 + 1) << 10) / len;
		else dy = ((y1 - y0 - 1) << 10) / len;
	} else{
		len = dy + 1;
		if(y0 > y1) dy = -1024;
		else dy = 1024;
		if(x0 <= x1) dx = ((x1 - x0 + 1) << 10) / len;
		else dx = ((x1 - x0 - 1) << 10) / len;
	}
	for(i = 0; i < len; i++){
		point_bpp(vrami, x >> 10, y >> 10, c, xsize, bpp);
		x += dx;
		y += dy;
	}

	return;
}

void draw_hexagon_i(void *vrami, int xsize, int a, int x, int y, uint c)
{
	int n, m;

	if(a < 0) a = -a;
	m = a >> 1;
	n = (1773 * m) >> 10;


	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);
	line_i(vrami, xsize, x + n, y - m, x + n, y + m, c);
	line_i(vrami, xsize, x + n, y + m, x    , y + a, c);
	line_i(vrami, xsize, x    , y + a, x - n, y + m, c);
	line_i(vrami, xsize, x - n, y + m, x - n, y - m, c);
	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	return;
}

void draw_chnos_logo(void *vrami, int xsize, int a, int x, int y)
{
	int n, m, oldx, oldy;
	uint c;

	oldx = x;
	oldy = y;

	if(a < 0) a = -a;
	m = a >> 1;
	n = (1773 * m) >> 10;
	c = 0x00FFFF;

	x = x - n;
	x = x - n;
	x = x - n;
	y = y - (3 * m);

	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);
	line_i(vrami, xsize, x + n, y + m, x    , y + a, c);
	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	x = x + n;
	x = x + n;

	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);
	line_i(vrami, xsize, x + n, y - m, x    , y    , c);
	line_i(vrami, xsize, x + n, y - m, x + n, y + m, c);
	line_i(vrami, xsize, x + n, y + m, x    , y + a, c);
	line_i(vrami, xsize, x    , y + a, x    , y    , c);
	line_i(vrami, xsize, x    , y + a, x - n, y + m, c);
	line_i(vrami, xsize, x - n, y + m, x - n, y - m, c);
	line_i(vrami, xsize, x - n, y - m, x    , y    , c);
	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	x = x + n;
	x = x + n;

	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);
	line_i(vrami, xsize, x + n, y - m, x    , y    , c);
	line_i(vrami, xsize, x + n, y - m, x + n, y + m, c);
	line_i(vrami, xsize, x + n, y + m, x    , y + a, c);
	line_i(vrami, xsize, x    , y + a, x    , y    , c);
	line_i(vrami, xsize, x    , y + a, x - n, y + m, c);
	line_i(vrami, xsize, x - n, y + m, x - n, y - m, c);
	line_i(vrami, xsize, x - n, y - m, x    , y    , c);
	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	x = x + n;
	x = x + n;

	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);
	line_i(vrami, xsize, x    , y + a, x - n, y + m, c);
	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	x = x - n;
	x = x - n;

	x = x - n;
	x = x - n;

	x = x + n;
	y = y + (3 * m);

	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);
	line_i(vrami, xsize, x + n, y - m, x    , y    , c);
	line_i(vrami, xsize, x + n, y - m, x + n, y + m, c);
	line_i(vrami, xsize, x + n, y + m, x    , y + a, c);
	line_i(vrami, xsize, x    , y + a, x    , y    , c);
	line_i(vrami, xsize, x    , y + a, x - n, y + m, c);
	line_i(vrami, xsize, x - n, y + m, x - n, y - m, c);
	line_i(vrami, xsize, x - n, y - m, x    , y    , c);
	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	x = x + n;
	y = y + (3 * m);

	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);

	x = x + n;
	x = x + n;

	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);
	line_i(vrami, xsize, x + n, y - m, x    , y    , c);
	line_i(vrami, xsize, x + n, y - m, x + n, y + m, c);
	line_i(vrami, xsize, x + n, y + m, x    , y + a, c);
	line_i(vrami, xsize, x    , y + a, x    , y    , c);
	line_i(vrami, xsize, x    , y + a, x - n, y + m, c);
	line_i(vrami, xsize, x - n, y + m, x - n, y - m, c);
	line_i(vrami, xsize, x - n, y - m, x    , y    , c);
	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	x = x - n;
	x = x - n;

	x = x - n;
	y = y - (3 * m);

	x = x - n;
	y = y + (3 * m);

	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	x = x - n;
	x = x - n;

	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);
	line_i(vrami, xsize, x + n, y - m, x    , y    , c);
	line_i(vrami, xsize, x + n, y - m, x + n, y + m, c);
	line_i(vrami, xsize, x + n, y + m, x    , y + a, c);
	line_i(vrami, xsize, x    , y + a, x    , y    , c);
	line_i(vrami, xsize, x    , y + a, x - n, y + m, c);
	line_i(vrami, xsize, x - n, y + m, x - n, y - m, c);
	line_i(vrami, xsize, x - n, y - m, x    , y    , c);
	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	x = x + n;
	x = x + n;

	line_i(vrami, xsize, x + n, y - m, x + n, y + m, c);

	x = x + n;
	y = y + (3 * m);

	line_i(vrami, xsize, x    , y - a, x + n, y - m, c);
	line_i(vrami, xsize, x + n, y - m, x    , y    , c);
	line_i(vrami, xsize, x + n, y - m, x + n, y + m, c);
	line_i(vrami, xsize, x + n, y + m, x    , y + a, c);
	line_i(vrami, xsize, x    , y + a, x    , y    , c);
	line_i(vrami, xsize, x    , y + a, x - n, y + m, c);
	line_i(vrami, xsize, x - n, y + m, x - n, y - m, c);
	line_i(vrami, xsize, x - n, y - m, x    , y    , c);
	line_i(vrami, xsize, x - n, y - m, x    , y - a, c);

	putfonts_asc_i(vrami, xsize, oldx + n + 8, oldy, 0x00FF00, "CHNOSProject");

	return;
}

uchar rgb_int2char (uint c32)
{
	uchar i ;
	for(i = 0;i < 15; i++) {
		if(rgb_int2char_list[i] == c32) {
			return i;
		}
	}
	return 8;
}

ushort rgb_int2short (uint c32)
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

void col_pat(void *vrami, int xsize, int ysize)
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

