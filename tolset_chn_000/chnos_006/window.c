#include "core.h"

struct WINCTL *wctl;

void init_windows(void)
{
	int i;
	wctl = (struct WINCTL *)memman_alloc_4k(sizeof(struct WINCTL));
	if(wctl == 0) goto err;
	for(i = 0;i < MAX_WINDOWS;i++){
		wctl->winfos[i].flags = WIN_FLAGS_VOID;
	}
err:
	return;


}

struct WINDOWINFO *window_alloc(void)
{
	struct WINDOWINFO *win;
	int i;
	for(i = 0;i < MAX_WINDOWS;i++){
		if(wctl->winfos[i].flags == WIN_FLAGS_VOID){
			win = &wctl->winfos[i];
			win->flags = WIN_FLAGS_USE;
			return win;
		}
	}
	return 0;
}

struct WINDOWINFO *make_window32(unsigned int *buf, unsigned char *title, int xsize, int ysize, int px, int py, int height)
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
	unsigned int col = 0;
	struct WINDOWINFO *winfo = window_alloc();
	if(winfo == 0) goto err;
	int i = 0;
	for (; *title != 0x00; title++) {
		winfo->title[i] = *title;		
		i++;
	}
	winfo->xsize = xsize + 8;
	winfo->ysize = ysize + 28;
	winfo->bxsize = xsize;
	winfo->bysize = ysize;
	winfo->px = px;
	winfo->py = py;
	
	winfo->center = sheet_alloc();
	winfo->sideL = sheet_alloc();
	winfo->sideR = sheet_alloc();
	winfo->bottom = sheet_alloc();
	winfo->head = sheet_alloc();

	winfo->buf_c = buf;
	winfo->buf_L = (unsigned int *)memman_alloc_4k(ysize * 4 * 4);
	winfo->buf_R = (unsigned int *)memman_alloc_4k(ysize * 4 * 4);
	winfo->buf_bottom = (unsigned int *)memman_alloc_4k(4 * winfo->xsize * 4);
	winfo->buf_head = (unsigned int *)memman_alloc_4k(24 * winfo->xsize * 4);

	sheet_setbuf(winfo->center, winfo->buf_c, xsize, ysize,INV_COL32);	
	sheet_setbuf(winfo->sideL, winfo->buf_L, 4, ysize,INV_COL32);
	sheet_setbuf(winfo->sideR, winfo->buf_R, 4, ysize,INV_COL32);
	sheet_setbuf(winfo->bottom, winfo->buf_bottom, winfo->xsize, 4,INV_COL32);
	sheet_setbuf(winfo->head, winfo->buf_head, winfo->xsize, 24,INV_COL32);

	boxfill_i(winfo->buf_c, xsize, 0xFFFFFF, 0,0,xsize,ysize);
	boxfill_i(winfo->buf_L, 4, 0x00FF00, 0,0,4,ysize);	
	boxfill_i(winfo->buf_R, 4, 0x00FF00, 0,0,4,ysize);
	boxfill_i(winfo->buf_bottom, winfo->xsize, 0x00FF00, 0,0,winfo->xsize,4);
	boxfill_i(winfo->buf_head, winfo->xsize, 0x00FF00, 0,0,winfo->xsize,24);
	putfonts_asc_i(winfo->buf_head, winfo->xsize, 4,4,0xffffff,winfo->title);
	for(y = 0; y < 16; y++){
		for(x = 0; x < 40; x++){
			c = closebtn[y][x];
			if(c == '@')		col = 0xCA0000;
			else if(c == '$')	col = 0xFB0000;
			else if(c == 'Q')	col = 0xFF1A1A;
			else if(c == 'J')	col = 0xFFFFFF;
			else if(c == 'O')	col = 0xFFFFFF;
			winfo->buf_head[winfo->xsize * y + (winfo->xsize - 44 + x)] = col;
		}
	}

	sheet_slide(winfo->center, px + 4,py + 24);
	sheet_slide(winfo->sideL, px,py + 24);
	sheet_slide(winfo->sideR, px + winfo->xsize - 4,py + 24);
	sheet_slide(winfo->bottom, px,py + winfo->ysize - 4);
	sheet_slide(winfo->head, px,py);


	sheet_updown(winfo->center, height);	
	sheet_updown(winfo->sideL, height);	
	sheet_updown(winfo->sideR, height);	
	sheet_updown(winfo->bottom, height);	
	sheet_updown(winfo->head, height);	
	
err:
	return winfo;

}

void slide_window(struct WINDOWINFO *winfo, int px, int py)
{
	winfo->px = px;
	winfo->py = py;
	sheet_slide(winfo->center, px + 4,py + 24);
	sheet_slide(winfo->sideL, px,py + 24);
	sheet_slide(winfo->sideR, px + winfo->xsize - 4,py + 24);
	sheet_slide(winfo->bottom, px,py + winfo->ysize - 4);
	sheet_slide(winfo->head, px,py);
	return;
}






