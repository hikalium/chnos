#include "core.h"

struct SHTCTL *ctl;

void init_sheets(unsigned int *vram, int xsize, int ysize)
{
	int i;
	ctl = (struct SHTCTL *)memman_alloc_4k(sizeof(struct SHTCTL));
	if(ctl == 0) goto err;
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1;
	for(i = 0;i < MAX_SHEETS;i++){
		ctl->sheets0[i].flags = SHT_FLAGS_VOID;
	}
err:
	return;

}

struct SHEET32 *sheet_alloc(void)
{
	struct SHEET32 *sht;
	int i;
	for(i = 0;i < MAX_SHEETS;i++){
		if(ctl->sheets0[i].flags == SHT_FLAGS_VOID){
			sht = &ctl->sheets0[i];
			sht->flags = SHT_FLAGS_USE;
			sht->height = -1;
			return sht;
		}
	}
	return 0;
}

void sheet_setbuf(struct SHEET32 *sht,unsigned int *buf,int xsize, int ysize, unsigned int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

void sheet_updown(struct SHEET32 *sht,int height)
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
		} else {	/*”ñ•\Ž¦‚É‚È‚éê‡*/
			if(ctl->top > old){
				for(h = old; h < ctl->top; h++){
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--;
		}
		sheet_refreshsub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
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
		sheet_refreshsub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	}
	return;
}

void sheet_refresh(struct SHEET32 *sht, int bx0, int by0, int bx1, int by1)
{
	if(sht->height >= 0){
		sheet_refreshsub(sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1 );
	}
	return;
}

void sheet_slide(struct SHEET32 *sht, int vx0, int vy0)
{
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if(sht->height >= 0) {
		sheet_refreshsub(old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize);
		sheet_refreshsub(vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize);
	}
	return;
}

void sheet_free(struct SHEET32 *sht)
{
	if(sht->height >= 0) sheet_updown(sht, -1);
	sht->flags = 0;
	return;
}

void sheet_refreshsub(int vx0, int vy0, int vx1, int vy1)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned int *buf, c, *vram = ctl->vram;
	struct SHEET32 *sht;
	for(h = 0; h <= ctl->top; h++){
		sht = ctl->sheets[h];
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
				c = buf[by * sht->bxsize + bx];
				if(c != sht->col_inv) vram[vy * ctl->xsize + vx] = c;
			}
		}
	}
	return;
}

