#include "core.h"

struct WINCTL *wctl;

void init_windows(void)
{
	int i;
	wctl = (struct WINCTL *)memman_alloc_4k(&system.sys.memman, sizeof(struct WINCTL));
	if(wctl == 0) goto err;
	for(i = 0;i < MAX_WINDOWS;i++){
		wctl->winfos[i].flags = initialized;
	}
err:
	return;


}

struct WINDOWINFO *window_alloc(void)
{
	struct WINDOWINFO *win;
	int i;
	for(i = 0;i < MAX_WINDOWS;i++){
		if(wctl->winfos[i].flags == initialized){
			win = &wctl->winfos[i];
			win->flags = allocated;
			return win;
		}
	}
	return 0;
}

struct WINDOWINFO *make_window32(unsigned char *title, int xsize, int ysize, int px, int py, int height)
{
	static char closebtn[16][40] = {
		"OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQJJQQQQQQQQJJQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQJJJQQQQQQJJJQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQQJJJQQQQJJJQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQQQJJJQQJJJQQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQQQQJJJJJJQQQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQQQQQJJJJQQQQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQQQQQJJJJQQQQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQQQQJJJJJJQQQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQQQJJJQQJJJQQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQQJJJQQQQJJJQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQJJJQQQQQQJJJQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQJJQQQQQQQQJJQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ$@",
		"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c;
	unsigned int col32 = 0;
	unsigned short col16 = 0;
	unsigned char col8 = 0;
	struct WINDOWINFO *winfo = window_alloc();

	if(winfo == 0) goto err;
	int i = 0;
	for (; *title != 0x00; title++) {
		winfo->title[i] = *title;		
		i++;
	}
	winfo->title[i] = 0x00;
	winfo->winxsize = xsize + 8;
	winfo->winysize = ysize + 28;
	winfo->xsize = xsize;
	winfo->ysize = ysize;
	winfo->position.x = px;
	winfo->position.y = py;
	winfo->origin.x = 4;
	winfo->origin.y = 24;
	
	winfo->win = sheet_alloc();

	winfo->buf = (unsigned int *)memman_alloc_4k(&system.sys.memman, (winfo->winxsize * winfo->winysize) * (system.sys.bpp >> 2));
//	winfo->buf_L = (unsigned int *)memman_alloc_4k(ysize * 4 * (system.sys.bpp >> 2));
//	winfo->buf_R = (unsigned int *)memman_alloc_4k(ysize * 4 * (system.sys.bpp >> 2));
//	winfo->buf_bottom = (unsigned int *)memman_alloc_4k(4 * winfo->xsize * (system.sys.bpp >> 2));
//	winfo->buf_head = (unsigned int *)memman_alloc_4k(24 * winfo->xsize * (system.sys.bpp >> 2));

	sheet_setbuf(winfo->win, winfo->buf, winfo->winxsize, winfo->winysize,INV_COL32);	
//	sheet_setbuf(winfo->sideL, winfo->buf_L, 4, ysize,INV_COL32);
//	sheet_setbuf(winfo->sideR, winfo->buf_R, 4, ysize,INV_COL32);
//	sheet_setbuf(winfo->bottom, winfo->buf_bottom, winfo->xsize, 4,INV_COL32);
//	sheet_setbuf(winfo->head, winfo->buf_head, winfo->xsize, 24,INV_COL32);

	boxfill_i(winfo->buf, winfo->winxsize, 0xFFFFFF, winfo->origin.x, winfo->origin.y, winfo->origin.x + xsize, winfo->origin.y + ysize);
	boxfill_i(winfo->buf, winfo->winxsize, WINDOW_COL32, 0, 0, winfo->winxsize, 24);	
	boxfill_i(winfo->buf, winfo->winxsize, WINDOW_COL32, 0, 0, 3, winfo->winysize);
	boxfill_i(winfo->buf, winfo->winxsize, WINDOW_COL32, winfo->winxsize - 4, 0, winfo->winxsize, winfo->winysize);
	boxfill_i(winfo->buf, winfo->winxsize, WINDOW_COL32, 0, winfo->winysize - 4, winfo->winxsize, winfo->winysize);
	putfonts_asc_sht_i(winfo->win, 4, 4, 0xffffff, WINDOW_COL32, winfo->title);

	if(system.sys.bpp == 8){
		for(y = 0; y < 16; y++){
			for(x = 0; x < 40; x++){
				c = closebtn[y][x];
				if(c == '@')		col8 = COL8_FF0000;
				else if(c == '$')	col8 = COL8_FF0000;
				else if(c == 'Q')	col8 = COL8_FF0000;
				else if(c == 'J')	col8 = COL8_FFFFFF;
				else if(c == 'O')	col8 = COL8_FFFFFF;
				point_i(winfo->buf, winfo->winxsize - 44 + x, y, col8, winfo->winxsize);
			}
		}
	}else if(system.sys.bpp == 16){
		for(y = 0; y < 16; y++){
			for(x = 0; x < 40; x++){
				c = closebtn[y][x];
				if(c == '@')		col16 = rgb_int2short(0xCA0000);
				else if(c == '$')	col16 = rgb_int2short(0xFB0000);
				else if(c == 'Q')	col16 = rgb_int2short(0xFF1A1A);
				else if(c == 'J')	col16 = rgb_int2short(0xFFFFFF);
				else if(c == 'O')	col16 = rgb_int2short(0xFFFFFF);
				point_i(winfo->buf, winfo->winxsize - 44 + x, y, col16, winfo->winxsize);
			}
		}
	}else if(system.sys.bpp == 32){
		for(y = 0; y < 16; y++){
			for(x = 0; x < 40; x++){
				c = closebtn[y][x];
				if(c == '@')		col32 = 0xCA0000;
				else if(c == '$')	col32 = 0xFB0000;
				else if(c == 'Q')	col32 = 0xFF1A1A;
				else if(c == 'J')	col32 = 0xFFFFFF;
				else if(c == 'O')	col32 = 0xFFFFFF;
				point_i(winfo->buf, winfo->winxsize - 44 + x, y, col32, winfo->winxsize);
			}
		}
	}

	sheet_slide(winfo->win, px, py);
//	sheet_slide(winfo->sideL, px,py + 24);
//	sheet_slide(winfo->sideR, px + winfo->xsize - 4,py + 24);
//	sheet_slide(winfo->bottom, px,py + winfo->ysize - 4);
//	sheet_slide(winfo->head, px,py);


	sheet_updown(winfo->win, height);	
//	sheet_updown(winfo->sideL, height);	
//	sheet_updown(winfo->sideR, height);	
//	sheet_updown(winfo->bottom, height);	
//	sheet_updown(winfo->head, height);	
	
err:
	return winfo;

}

void slide_window(struct WINDOWINFO *winfo, int px, int py)
{
	winfo->position.x = px;
	winfo->position.y = py;

	sheet_slide(winfo->win, winfo->position.x, winfo->position.y);
	return;
}

void refresh_window(struct WINDOWINFO *winfo)
{
	sheet_refresh_full(winfo->win);
	return;
}

void refresh_window_alpha(struct WINDOWINFO *winfo)
{
	sheet_refresh_full_alpha(winfo->win);
	return;
}

void boxfill_win(struct WINDOWINFO *winfo, unsigned int c, int x0, int y0, int x1, int y1)
{
	if(x0 < 0 || y0 < 0 || x1 > winfo->xsize || y1 > winfo->ysize) goto err;

	boxfill_i(winfo->buf, winfo->winxsize, c, winfo->origin.x + x0, winfo->origin.y + y0, winfo->origin.x + x1, winfo->origin.y + y1);
	sheet_refresh(winfo->win, x0 + winfo->origin.x, y0 + winfo->origin.y, x1 + winfo->origin.x, y1 + winfo->origin.y);
err:
	return;
}

void putfonts_win(struct WINDOWINFO *winfo, int x, int y, unsigned int c, unsigned int bc, const unsigned char *s)
{
	if(x < 0 || y < 0 || x > winfo->xsize || y > winfo->ysize) goto err;
	putfonts_asc_sht_i(winfo->win, x + winfo->origin.x, y + winfo->origin.y, c, bc, s);
err:
	return;
}



