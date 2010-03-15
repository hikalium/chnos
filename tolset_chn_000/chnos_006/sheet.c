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

struct SHEET *sheet_alloc(void)
{
	struct SHEET *sht;
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

void sheet_setbuf(struct SHEET *sht,unsigned int *buf,int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}