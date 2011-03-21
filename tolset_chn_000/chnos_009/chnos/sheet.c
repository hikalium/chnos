
#include "core.h"

UI_Sheet_Control sheetctrl;

void Initialise_Sheet(void *vram, uint xsize, uint ysize, uint bpp)
{
	uint x, y;

	sheetctrl.mainvram = vram;
	sheetctrl.map = System_MemoryControl_Allocate(xsize * ysize * 4);
	if(sheetctrl.map == 0){
		return;
	}
	for(y = 0; y < ysize; y++){
		for(x = 0; x < xsize; x++){
			sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] = 0;
		}
	}
	sheetctrl.mainvramsize.x = xsize;
	sheetctrl.mainvramsize.y = ysize;
	sheetctrl.mainvrambpp = bpp;
	sheetctrl.next = 0;
	sheetctrl.sheets = 0;
	return;
}

UI_Sheet *Sheet_Get(uint xsize, uint ysize, uint bpp)
{
	UI_Sheet *sheet;
uchar s[128];

	sheet = System_MemoryControl_Allocate(sizeof(UI_Sheet));
	sheet->vram = System_MemoryControl_Allocate(xsize * ysize * bpp);
	sheet->position.x = 0;
	sheet->position.y = 0;
	sheet->size.x = xsize;
	sheet->size.y = ysize;
	sheet->bpp = bpp;
	sheet->next = 0;
	sheet->before = 0;

sprintf(s, "mainbpp:%d requestbpp:%d\n", sheetctrl.mainvrambpp, bpp);
Send_SerialPort(s);

	if(sheetctrl.mainvrambpp == 32){
		if(bpp == 32){
			sheet->Refresh = Sheet_Refresh_32from32;
//		} else if(bpp == 16){

//		} else if(bpp == 8){

		}
	} else if(sheetctrl.mainvrambpp == 16){
		if(bpp == 32){
			sheet->Refresh = Sheet_Refresh_16from32;
//		} else if(bpp == 16){

//		} else if(bpp == 8){

		}
	} else if(sheetctrl.mainvrambpp == 8){
		if(bpp == 32){
			sheet->Refresh = Sheet_Refresh_08from32;
//		} else if(bpp == 16){

//		} else if(bpp == 8){

		}
	}
	return sheet;
}

uint Sheet_Show(UI_Sheet *sheet, uint px, uint py, uint height)
{
	uint i;
	UI_Sheet **now;

	now = &sheetctrl.next;
	for(i = 0; i <= height; i++){
		if((*now) == 0){
			break;
		}
		if(i == height){
			break;
		}
		now = &(*now)->next;
	}
	sheet->next = *now;
	*now = sheet;
	sheetctrl.sheets++;
	sheet->position.x = px;
	sheet->position.y = py;

	Sheet_Refresh_Map(sheet, sheet->position.x, sheet->position.y, sheet->position.x + sheet->size.x - 1, sheet->position.y + sheet->size.y - 1);
	sheet->Refresh(sheet, 0, 0, sheet->size.x - 1, sheet->size.y - 1);

	return i;
}

void Sheet_Refresh_Map(UI_Sheet *sheet, uint x0, uint y0, uint x1, uint y1)
{
	UI_Sheet **before;
	uint i, x, y;
	DATA_Position2D target0, target1;

	if(x0 > sheetctrl.mainvramsize.x || y0 > sheetctrl.mainvramsize.y || x1 > sheetctrl.mainvramsize.x || y1 > sheetctrl.mainvramsize.y){
		return;
	}

	before = &sheetctrl.next;
	for(i = 0; i < sheetctrl.sheets; i++){
		for(y = y0; y < y1; y++){
			for(x = x0; x < x1; x++){
				if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == (uint)*before){
					sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] = 0;
				}
			}
		}
		if(*before == sheet){
			break;
		}
		if((*before)->next != 0){
			(*before)->next->before = *before;
		}
		before = &(*before)->next;
	}

	i++;

	for(; i > 0; i--){
		target0.x = (*before)->position.x;
		target0.y = (*before)->position.y;
		target1.x = (*before)->position.x + (*before)->size.x - 1;
		target1.y = (*before)->position.y + (*before)->size.y - 1;

		if(target0.x <= x1 && target0.y <= y1 && target1.x >= x0 && target1.y >= y0){
			if(target0.x < x0){
				target0.x = x0;
			}
			if(target0.y < y0){
				target0.y = y0;
			}
			if(target1.x > x1){
				target1.x = x1;
			}
			if(target1.y > y1){
				target1.y = y1;
			}
			for(y = target0.y; y < target1.y; y++){
				for(x = target0.x; x < target1.x; x++){
					if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == 0){
						sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] = (uint)*before;
					}
				}
			}
		}
		before = &(*before)->before;
	}
	return;
}

void Sheet_Refresh_32from32(UI_Sheet *sheet, uint px0, uint py0, uint px1, uint py1)
{
	uint x, y;

	if(px0 > sheetctrl.mainvramsize.x || py0 > sheetctrl.mainvramsize.y || px1 > sheetctrl.mainvramsize.x || py1 > sheetctrl.mainvramsize.y){
		return;
	}

	for(y = py0 + sheet->position.y; y < py1 + sheet->position.y; y++){
		for(x = px0 + sheet->position.x; x < px1 + sheet->position.x; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == (uint)sheet){
				((uint *)sheetctrl.mainvram)[(y * sheetctrl.mainvramsize.x) + x] = ((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)];
			}
		}
	}
	return;
}

void Sheet_Refresh_16from32(UI_Sheet *sheet, uint px0, uint py0, uint px1, uint py1)
{
	uint x, y;

	if(px0 > sheetctrl.mainvramsize.x || py0 > sheetctrl.mainvramsize.y || px1 > sheetctrl.mainvramsize.x || py1 > sheetctrl.mainvramsize.y){
		return;
	}

	for(y = py0 + sheet->position.y; y < py1 + sheet->position.y; y++){
		for(x = px0 + sheet->position.x; x < px1 + sheet->position.x; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == (uint)sheet){
				((ushort *)sheetctrl.mainvram)[(y * sheetctrl.mainvramsize.x) + x] = RGB_32_To_16(((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)]);
			}
		}
	}
	return;
}

void Sheet_Refresh_08from32(UI_Sheet *sheet, uint px0, uint py0, uint px1, uint py1)
{
	uint x, y;

	if(px0 > sheetctrl.mainvramsize.x || py0 > sheetctrl.mainvramsize.y || px1 > sheetctrl.mainvramsize.x || py1 > sheetctrl.mainvramsize.y){
		return;
	}

	for(y = py0 + sheet->position.y; y < py1 + sheet->position.y; y++){
		for(x = px0 + sheet->position.x; x < px1 + sheet->position.x; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == (uint)sheet){
				((uchar *)sheetctrl.mainvram)[(y * sheetctrl.mainvramsize.x) + x] = RGB_32_To_08(((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)]);
			}
		}
	}
	return;
}

