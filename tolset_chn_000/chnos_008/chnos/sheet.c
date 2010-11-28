
#include "core.h"

UI_SheetControl *ctl;
void (*sheet_refreshsub)(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void (*sheet_refreshmap)(int vx0, int vy0, int vx1, int vy1, int h0);

void init_sheets(void *vram, int xsize, int ysize, uchar bits)
{
	int i;

	switch(bits){
		case 8:
			sheet_refreshsub = sheet_refreshsub8;
			sheet_refreshmap = sheet_refreshmap8;
			break;
		case 16:
			sheet_refreshsub = sheet_refreshsub16;
			sheet_refreshmap = sheet_refreshmap16;
			break;
		case 32:
			sheet_refreshsub = sheet_refreshsub32;
			sheet_refreshmap = sheet_refreshmap32;
			break;
	}

	ctl = (UI_SheetControl *)sys_memman_alloc(sizeof(UI_SheetControl));
	if(ctl == 0) goto err;
	ctl->map = (uint *)sys_memman_alloc(xsize * ysize * 4);
	if(ctl->map == 0){
		sys_memman_free(ctl, sizeof(UI_SheetControl));
		goto err;
	}
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1;
	for(i = 0;i < MAX_SHEETS;i++){
		ctl->sheets0[i].flags = initialized;
	}
err:
	return;
}

UI_Sheet *sheet_alloc(void)
{
	UI_Sheet *sht;
	int i;

	for(i = 0;i < MAX_SHEETS;i++){
		if(ctl->sheets0[i].flags == initialized){
			sht = &ctl->sheets0[i];
			sht->flags = allocated;
			sht->height = -1;
			return sht;
		}
	}
	return 0;
}

void sheet_setbuf(UI_Sheet *sht, void *buf,int xsize, int ysize, uint col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

void sheet_updown(UI_Sheet *sht, int height)
{
	int h, old = sht->height;

	if(height > ctl->top + 1) height = ctl->top + 1;
	if (height < -1) height = -1;
	sht->height = height;

	if (old > height){	/*ˆÈ‘O‚æ‚è‚à’á‚­‚È‚éê‡*/
		if (height >= 0){
			for (h = old; h > height; h--){
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
			sheet_refreshmap(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			sheet_refreshsub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
		} else {	/*”ñ•\Ž¦‚É‚È‚éê‡*/
			if(ctl->top > old){
				for(h = old; h < ctl->top; h++){
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--;
			sheet_refreshmap(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			sheet_refreshsub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1);
		}
	} else if(old < height) {	/*ˆÈ‘O‚æ‚è‚à‚‚­‚È‚éê‡*/
		if(old >= 0){
			for(h = old; h < height; h++){
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
				}
				ctl->sheets[height]= sht;
			} else {
				for(h = ctl->top; h >= height; h--){
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h;			
			}
			ctl->sheets[height] = sht;
			ctl->top++;
		}
		sheet_refreshmap(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		sheet_refreshsub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height);
	}

	return;
}

void sheet_refresh(UI_Sheet *sht, int bx0, int by0, int bx1, int by1)
{
	if(sht->height >= 0){
		sheet_refreshsub(sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1 , sht->height, sht->height);
	}

	return;
}

void sheet_refresh_full_alpha(UI_Sheet *sht)
{
	sheet_refreshmap(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, sht->height);
	sheet_refresh_full(sht);

	return;
}

void sheet_refresh_full(UI_Sheet *sht)
{
	sheet_refresh(sht, 0, 0, sht->bxsize, sht->bysize);

	return;
}

void sheet_slide(UI_Sheet *sht, int vx0, int vy0)
{
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;

	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if(sht->height >= 0) {
		sheet_refreshmap(old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize,0);
		sheet_refreshmap(vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize,sht->height);
		sheet_refreshsub(old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize,0,sht->height - 1);
		sheet_refreshsub(vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize,sht->height, sht->height);
	}

	return;
}

void sheet_free(UI_Sheet *sht)
{
	if(sht->height >= 0) sheet_updown(sht, -1);
	sht->flags = initialized;

	return;
}

void sheet_refreshsub32(int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	uint *map = ctl->map,sid;
	uint *buf, *vram = (uint *)ctl->vram;
	UI_Sheet *sht;

	if(vx0 < 0) vx0 = 0;
	if(vy0 < 0) vy0 = 0;
	if(vx1 > ctl->xsize) vx1 = ctl->xsize;
	if(vy1 > ctl->ysize) vy1 = ctl->ysize;
	for(h = 0; h <= ctl->top; h++){
		sht = ctl->sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets0;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sht->bxsize) bx1 = sht->bxsize;
		if(by1 > sht->bysize) by1 = sht->bysize;
		for(by = by0; by < by1; by++){
			vy = sht->vy0 + by;
			for(bx = bx0; bx < bx1; bx++){
				vx = sht->vx0 + bx;
				if(map[vy * ctl->xsize + vx] == sid) vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
			}
		}
	}

	return;
}

void sheet_refreshsub16(int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	uint *map = ctl->map,sid;
	ushort *buf, *vram = (ushort *)ctl->vram;
	UI_Sheet *sht;

	if(vx0 < 0) vx0 = 0;
	if(vy0 < 0) vy0 = 0;
	if(vx1 > ctl->xsize) vx1 = ctl->xsize;
	if(vy1 > ctl->ysize) vy1 = ctl->ysize;
	for(h = 0; h <= ctl->top; h++){
		sht = ctl->sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets0;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sht->bxsize) bx1 = sht->bxsize;
		if(by1 > sht->bysize) by1 = sht->bysize;
		for(by = by0; by < by1; by++){
			vy = sht->vy0 + by;
			for(bx = bx0; bx < bx1; bx++){
				vx = sht->vx0 + bx;
				if(map[vy * ctl->xsize + vx] == sid) vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
			}
		}
	}

	return;
}

void sheet_refreshsub8(int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	uint *map = ctl->map,sid;
	uchar *buf, *vram = (uchar *)ctl->vram;
	UI_Sheet *sht;

	if(vx0 < 0) vx0 = 0;
	if(vy0 < 0) vy0 = 0;
	if(vx1 > ctl->xsize) vx1 = ctl->xsize;
	if(vy1 > ctl->ysize) vy1 = ctl->ysize;
	for(h = 0; h <= ctl->top; h++){
		sht = ctl->sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets0;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sht->bxsize) bx1 = sht->bxsize;
		if(by1 > sht->bysize) by1 = sht->bysize;
		for(by = by0; by < by1; by++){
			vy = sht->vy0 + by;
			for(bx = bx0; bx < bx1; bx++){
				vx = sht->vx0 + bx;
				if(map[vy * ctl->xsize + vx] == sid) vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
			}
		}
	}

	return;
}

void sheet_refreshmap32(int vx0, int vy0, int vx1, int vy1, int h0)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	uint sid, *map = ctl->map;
	uint *buf;
	UI_Sheet *sht;

	if(vx0 < 0) vx0 = 0;
	if(vy0 < 0) vy0 = 0;
	if(vx1 > ctl->xsize) vx1 = ctl->xsize;
	if(vy1 > ctl->ysize) vy1 = ctl->ysize;
	for(h = 0; h <= ctl->top; h++){
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0;
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sht->bxsize) bx1 = sht->bxsize;
		if(by1 > sht->bysize) by1 = sht->bysize;
		for(by = by0; by < by1; by++){
			vy = sht->vy0 + by;
			for(bx = bx0; bx < bx1; bx++){
				vx = sht->vx0 + bx;
				if(buf[by * sht->bxsize + bx] != INV_COL32) map[vy * ctl->xsize + vx] = sid;
			}
		}
	}

	return;
}

void sheet_refreshmap16(int vx0, int vy0, int vx1, int vy1, int h0)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	uint sid, *map = ctl->map;
	ushort *buf;
	UI_Sheet *sht;

	if(vx0 < 0) vx0 = 0;
	if(vy0 < 0) vy0 = 0;
	if(vx1 > ctl->xsize) vx1 = ctl->xsize;
	if(vy1 > ctl->ysize) vy1 = ctl->ysize;
	for(h = 0; h <= ctl->top; h++){
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0;
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sht->bxsize) bx1 = sht->bxsize;
		if(by1 > sht->bysize) by1 = sht->bysize;
		for(by = by0; by < by1; by++){
			vy = sht->vy0 + by;
			for(bx = bx0; bx < bx1; bx++){
				vx = sht->vx0 + bx;
				if(buf[by * sht->bxsize + bx] != INV_COL16) map[vy * ctl->xsize + vx] = sid;
			}
		}
	}

	return;
}

void sheet_refreshmap8(int vx0, int vy0, int vx1, int vy1, int h0)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	uint sid, *map = ctl->map;
	uchar *buf;
	UI_Sheet *sht;

	if(vx0 < 0) vx0 = 0;
	if(vy0 < 0) vy0 = 0;
	if(vx1 > ctl->xsize) vx1 = ctl->xsize;
	if(vy1 > ctl->ysize) vy1 = ctl->ysize;
	for(h = 0; h <= ctl->top; h++){
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0;
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sht->bxsize) bx1 = sht->bxsize;
		if(by1 > sht->bysize) by1 = sht->bysize;
		for(by = by0; by < by1; by++){
			vy = sht->vy0 + by;
			for(bx = bx0; bx < bx1; bx++){
				vx = sht->vx0 + bx;
				if(buf[by * sht->bxsize + bx] != INV_COL8) map[vy * ctl->xsize + vx] = sid;
			}
		}
	}

	return;
}
