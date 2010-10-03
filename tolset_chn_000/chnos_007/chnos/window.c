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

struct WINDOWINFO *make_window(unsigned char *title, int xsize, int ysize, int px, int py, int height, bool active)
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
	int x, y, i = 0;
	char c;
	unsigned int color = 0;
	struct WINDOWINFO *winfo = window_alloc();

	if(winfo == 0) goto err;
	for (; *title != 0x00; title++) {
		winfo->title[i] = *title;		
		i++;
	}
	winfo->title[i] = 0x00;
	winfo->active = active;
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

	sheet_setbuf(winfo->win, winfo->buf, winfo->winxsize, winfo->winysize,INV_COL32);	

	if(system.sys.bpp == 8){
		if(active) color = WIN_COL8_ACTIVE;
		else color = WIN_COL8_INACTIVE;
	} else if(system.sys.bpp == 16){
		if(active) color = WIN_COL16_ACTIVE;
		else color = WIN_COL16_INACTIVE;
	} else if(system.sys.bpp == 32){
		if(active) color = WIN_COL32_ACTIVE;
		else color = WIN_COL32_INACTIVE;
	}

	boxfill_i(winfo->buf, winfo->winxsize, 0xFFFFFF, winfo->origin.x, winfo->origin.y, winfo->origin.x + xsize, winfo->origin.y + ysize);
	boxfill_i(winfo->buf, winfo->winxsize, color, 0, 0, winfo->winxsize, 24);	
	boxfill_i(winfo->buf, winfo->winxsize, color, 0, 0, 3, winfo->winysize);
	boxfill_i(winfo->buf, winfo->winxsize, color, winfo->winxsize - 4, 0, winfo->winxsize, winfo->winysize);
	boxfill_i(winfo->buf, winfo->winxsize, color, 0, winfo->winysize - 4, winfo->winxsize, winfo->winysize);

	putfonts_asc_sht_i(winfo->win, 4, 4, 0xffffff, color, winfo->title);

	for(y = 0; y < 16; y++){
		for(x = 0; x < 40; x++){
			c = closebtn[y][x];
			if(c == '@')		color = 0xFF0000;
			else if(c == '$')	color = 0xFF0000;
			else if(c == 'Q')	color = 0xFF0000;
			else if(c == 'J')	color = 0xFFFFFF;
			else if(c == 'O')	color = 0xFFFFFF;
			point_i(winfo->buf, winfo->winxsize - 44 + x, y, color, winfo->winxsize);
		}
	}

	sheet_slide(winfo->win, px, py);
	sheet_updown(winfo->win, height);	
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



