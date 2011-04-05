
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

UI_Sheet *Sheet_Get(uint xsize, uint ysize, uint bpp, uint invcol)
{
	UI_Sheet *sheet;

	if(bpp == 0){
		bpp = sheetctrl.mainvrambpp;
	}

	sheet = System_MemoryControl_Allocate(sizeof(UI_Sheet));
	sheet->vram = System_MemoryControl_Allocate(xsize * ysize * (bpp >> 3));
	sheet->position.x = 0;
	sheet->position.y = 0;
	sheet->size.x = xsize;
	sheet->size.y = ysize;
	sheet->bpp = bpp;
	sheet->invcol = invcol;
	if(sheet->invcol == 0){
		sheet->WriteMap = Sheet_Write_Map_NoInvisible;
	} else if(sheet->bpp == 32){
		sheet->WriteMap = Sheet_Write_Map_32;
	} else if(sheet->bpp == 16){
		sheet->WriteMap = Sheet_Write_Map_16;
		sheet->invcol = RGB_32_To_16(sheet->invcol);
	} else if(sheet->bpp == 8){
		sheet->WriteMap = Sheet_Write_Map_08;
		sheet->invcol = RGB_32_To_08(sheet->invcol);
	}
	sheet->next = 0;
	sheet->before = 0;

	sheet->Refresh = Sheet_Refresh_Invalid;

	return sheet;
}

uint Sheet_Show(UI_Sheet *sheet, int px, int py, uint height)
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
	sheet->visible = true;

	if(sheetctrl.mainvrambpp == 32){
		if(sheet->bpp == 32){
			sheet->Refresh = Sheet_Refresh_32from32;
//		} else if(sheet->bpp == 16){

//		} else if(sheet->bpp == 8){

		}
	} else if(sheetctrl.mainvrambpp == 16){
		if(sheet->bpp == 32){
			sheet->Refresh = Sheet_Refresh_16from32;
		} else if(sheet->bpp == 16){
			sheet->Refresh = Sheet_Refresh_16from16;
//		} else if(sheet->bpp == 8){

		}
	} else if(sheetctrl.mainvrambpp == 8){
		if(sheet->bpp == 32){
			sheet->Refresh = Sheet_Refresh_08from32;
//		} else if(sheet->bpp == 16){

		} else if(sheet->bpp == 8){
			sheet->Refresh = Sheet_Refresh_08from08;
		}
	}

	Sheet_Refresh_Map(sheet, sheet->position.x, sheet->position.y, sheet->position.x + sheet->size.x - 1, sheet->position.y + sheet->size.y - 1);
	sheet->Refresh(sheet, 0, 0, sheet->size.x - 1, sheet->size.y - 1);

	return i;
}

void Sheet_Slide(UI_Sheet *sheet, int px, int py)
{
	int movex, movey;
	DATA_Position2D target0, target1;

	movex = px - sheet->position.x;
	movey = py - sheet->position.y;

	if(movex == 0 && movey == 0){	//�����Ȃ�
		return;
	}
	if(movey == 0){	//���ɃX���C�h���邾��
		target0.y = sheet->position.y;
		target1.y = sheet->position.y + sheet->size.y - 1;
		if(movex > 0){	//�E����
			target0.x = sheet->position.x;
			target1.x = sheet->position.x + sheet->size.x - 1 + movex;
		} else{	//������
			target0.x = sheet->position.x + movex;
			target1.x = sheet->position.x + sheet->size.x - 1;
		}
	} else if(movex == 0){	//�c�ɃX���C�h���邾��
		target0.x = sheet->position.x;
		target1.x = sheet->position.x + sheet->size.x - 1;
		if(movey > 0){	//������
			target0.y = sheet->position.y;
			target1.y = sheet->position.y + sheet->size.y - 1 + movey;
		} else{	//�����
			target0.y = sheet->position.y + movey;
			target1.y = sheet->position.y + sheet->size.y - 1;
		}
	} else if(movex > 0 && movey > 0){	//�E���փX���C�h
		target0.x = sheet->position.x;
		target0.y = sheet->position.y;
		target1.x = sheet->position.x + sheet->size.x - 1 + movex;
		target1.y = sheet->position.y + sheet->size.y - 1 + movey;
	} else if(movex > 0 && movey < 0){	//�E��փX���C�h
		target0.x = sheet->position.x;
		target0.y = sheet->position.y + movey;
		target1.x = sheet->position.x + sheet->size.x - 1 + movex;
		target1.y = sheet->position.y + sheet->size.y - 1;
	} else if(movex < 0 && movey > 0){	//�����փX���C�h
		target0.x = sheet->position.x + movex;
		target0.y = sheet->position.y;
		target1.x = sheet->position.x + sheet->size.x - 1;
		target1.y = sheet->position.y + sheet->size.y - 1 + movey;
	} else if(movex < 0 && movey < 0){	//����փX���C�h
		target0.x = sheet->position.x + movex;
		target0.y = sheet->position.y + movey;
		target1.x = sheet->position.x + sheet->size.x - 1;
		target1.y = sheet->position.y + sheet->size.y - 1;
	}

	sheet->visible = false;
	Sheet_Refresh_Map(sheet, sheet->position.x, sheet->position.y, sheet->position.x + sheet->size.x - 1, sheet->position.y + sheet->size.y - 1);
	sheet->position.x = px;
	sheet->position.y = py;
	sheet->visible = true;
	Sheet_Refresh_Map(sheet, sheet->position.x, sheet->position.y, sheet->position.x + sheet->size.x - 1, sheet->position.y + sheet->size.y - 1);
	Sheet_Refresh_All(sheetctrl.next, sheet->next, target0.x, target0.y, target1.x, target1.y);
	return;
}

void Sheet_Refresh_Map(UI_Sheet *sheet, int x0, int y0, int x1, int y1)
{
	UI_Sheet **before;
	uint i, x, y;
	DATA_Position2D target0, target1;

	if(x0 >= sheetctrl.mainvramsize.x || y0 >= sheetctrl.mainvramsize.y || x1 < 0 || y1 < 0){
		return;
	}
	if(x0 < 0){
		x0 = 0;
	}
	if(y0 < 0){
		y0 = 0;
	}
	if(x1 >= sheetctrl.mainvramsize.x){
		x1 = sheetctrl.mainvramsize.x - 1;
	}
	if(y1 >= sheetctrl.mainvramsize.y){
		y1 = sheetctrl.mainvramsize.y - 1;
	}

	before = &sheetctrl.next;
	for(i = 0; i < sheetctrl.sheets; i++){
		for(y = y0; y <= y1; y++){
			for(x = x0; x <= x1; x++){
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

		if(target0.x <= x1 && target0.y <= y1 && target1.x >= x0 && target1.y >= y0 && (*before)->visible){
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
			(*before)->WriteMap(*before, target0.x, target0.y, target1.x, target1.y);
		}
		before = &(*before)->before;
	}
	return;
}

void Sheet_Write_Map_32(UI_Sheet *sheet, int x0, int y0, int x1, int y1)
{
	int x, y;

	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == 0 && ((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)] != sheet->invcol){
				sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] = (uint)sheet;
			}
		}
	}
	return;
}

void Sheet_Write_Map_16(UI_Sheet *sheet, int x0, int y0, int x1, int y1)
{
	int x, y;

	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == 0 && ((ushort *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)] != (ushort)sheet->invcol){
				sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] = (uint)sheet;
			}
		}
	}
	return;
}

void Sheet_Write_Map_08(UI_Sheet *sheet, int x0, int y0, int x1, int y1)
{
	int x, y;

	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == 0 && ((uchar *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)] != (uchar)sheet->invcol){
				sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] = (uint)sheet;
			}
		}
	}
	return;
}

void Sheet_Write_Map_NoInvisible(UI_Sheet *sheet, int x0, int y0, int x1, int y1)
{
	int x, y;

	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == 0){
				sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] = (uint)sheet;
			}
		}
	}
	return;
}

void Sheet_Refresh_All(UI_Sheet *sheet0, UI_Sheet *sheet1, int x0, int y0, int x1, int y1)
{
	UI_Sheet *now;
	uint i;
	DATA_Position2D target0, target1;

	now = sheet0;
	for(i = 0; i < sheetctrl.sheets; i++){
		target0.x = now->position.x;
		target0.y = now->position.y;
		target1.x = now->position.x + now->size.x - 1;
		target1.y = now->position.y + now->size.y - 1;

		if(target0.x <= x1 && target0.y <= y1 && target1.x >= x0 && target1.y >= y0 && now->visible){
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
			now->Refresh(now, target0.x - now->position.x, target0.y - now->position.y, target1.x - now->position.x, target1.y - now->position.y);
		}
		if(now->next == sheet1 || now->next == 0){
			break;
		}
		now = now->next;
	}
	return;
}

void Sheet_Refresh_32from32(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	int x, y;

	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= sheetctrl.mainvramsize.x || py0 >= sheetctrl.mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= sheetctrl.mainvramsize.x){
		px1 = sheetctrl.mainvramsize.x - 1;
	}
	if(py1 >= sheetctrl.mainvramsize.y){
		py1 = sheetctrl.mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == (uint)sheet){
				((uint *)sheetctrl.mainvram)[(y * sheetctrl.mainvramsize.x) + x] = ((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)];
			}
		}
	}
	return;
}

void Sheet_Refresh_16from32(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	int x, y;

	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= sheetctrl.mainvramsize.x || py0 >= sheetctrl.mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= sheetctrl.mainvramsize.x){
		px1 = sheetctrl.mainvramsize.x - 1;
	}
	if(py1 >= sheetctrl.mainvramsize.y){
		py1 = sheetctrl.mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == (uint)sheet){
				((ushort *)sheetctrl.mainvram)[(y * sheetctrl.mainvramsize.x) + x] = RGB_32_To_16(((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)]);
			}
		}
	}
	return;
}

void Sheet_Refresh_08from32(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	int x, y;

	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= sheetctrl.mainvramsize.x || py0 >= sheetctrl.mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= sheetctrl.mainvramsize.x){
		px1 = sheetctrl.mainvramsize.x - 1;
	}
	if(py1 >= sheetctrl.mainvramsize.y){
		py1 = sheetctrl.mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == (uint)sheet){
				((uchar *)sheetctrl.mainvram)[(y * sheetctrl.mainvramsize.x) + x] = RGB_32_To_08(((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)]);
			}
		}
	}
	return;
}

void Sheet_Refresh_16from16(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	int x, y;

	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= sheetctrl.mainvramsize.x || py0 >= sheetctrl.mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= sheetctrl.mainvramsize.x){
		px1 = sheetctrl.mainvramsize.x - 1;
	}
	if(py1 >= sheetctrl.mainvramsize.y){
		py1 = sheetctrl.mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == (uint)sheet){
				((ushort *)sheetctrl.mainvram)[(y * sheetctrl.mainvramsize.x) + x] = ((ushort *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)];
			}
		}
	}
	return;
}

void Sheet_Refresh_08from08(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	int x, y;

	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= sheetctrl.mainvramsize.x || py0 >= sheetctrl.mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= sheetctrl.mainvramsize.x){
		px1 = sheetctrl.mainvramsize.x - 1;
	}
	if(py1 >= sheetctrl.mainvramsize.y){
		py1 = sheetctrl.mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(sheetctrl.map[(y * sheetctrl.mainvramsize.x) + x] == (uint)sheet){
				((uchar *)sheetctrl.mainvram)[(y * sheetctrl.mainvramsize.x) + x] = ((uchar *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)];
			}
		}
	}
	return;
}

void Sheet_Refresh_Invalid(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	return;
}

void Sheet_Draw_Put_String(UI_Sheet *sheet, uint x, uint y, uint c, const uchar *s)
{
	uint i;

	if(sheet->bpp == 32){
		Draw_Put_String_32(sheet->vram, sheet->size.x, x, y, c, s);
	} else if(sheet->bpp == 16){
		Draw_Put_String_16(sheet->vram, sheet->size.x, x, y, c, s);
	} else if(sheet->bpp == 8){
		Draw_Put_String_08(sheet->vram, sheet->size.x, x, y, c, s);
	}
	for(i = 0; s[i] != 0x00; i++){

	}
	sheet->Refresh(sheet, x, y, x + (i * 8), y + 16);
	return;
}

void Sheet_Draw_Fill_Rectangle(UI_Sheet *sheet, uint c, uint x0, uint y0, uint x1, uint y1)
{
	if(sheet->bpp == 32){
		Draw_Fill_Rectangle_32(sheet->vram, sheet->size.x, c, x0, y0, x1, y1);
	} else if(sheet->bpp == 16){
		Draw_Fill_Rectangle_16(sheet->vram, sheet->size.x, c, x0, y0, x1, y1);
	} else if(sheet->bpp == 8){
		Draw_Fill_Rectangle_08(sheet->vram, sheet->size.x, c, x0, y0, x1, y1);
	}
	sheet->Refresh(sheet, x0, y0, x1, y1);
	return;
}

void Sheet_Draw_Point(UI_Sheet *sheet, uint c, uint x, uint y)
{
	if(sheet->bpp == 32){
		((uint *)sheet->vram)[y * sheet->size.x + x] = c;
	} else if(sheet->bpp == 16){
		c = RGB_32_To_16(c);
		((ushort *)sheet->vram)[y * sheet->size.x + x] = (ushort)c;
	} else if(sheet->bpp == 8){
		c = RGB_32_To_08(c);
		((uchar *)sheet->vram)[y * sheet->size.x + x] = (uchar)c;
	}
	sheet->Refresh(sheet, x, y, x, y);
	return;
}
