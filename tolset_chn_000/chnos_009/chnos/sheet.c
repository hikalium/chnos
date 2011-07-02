
#include "core.h"

extern IO_MemoryControl sys_mem_ctrl;

UI_Sheet_Control sys_sheet_ctrl;

void Sheet_Initialise(UI_Sheet_Control *sheetctrl, IO_MemoryControl *memctrl, void *vram, uint xsize, uint ysize, uint bpp)
{
	uint x, y;

	sheetctrl->memctrl = memctrl;

	sheetctrl->mainvram = vram;
	sheetctrl->map = MemoryControl_Allocate(sheetctrl->memctrl, xsize * ysize * 4);
	if(sheetctrl->map == 0){
		return;
	}
	for(y = 0; y < ysize; y++){
		for(x = 0; x < xsize; x++){
			sheetctrl->map[(y * xsize) + x] = 0;
		}
	}
	sheetctrl->mainvramsize.x = xsize;
	sheetctrl->mainvramsize.y = ysize;
	sheetctrl->mainvrambpp = bpp;

	sheetctrl->base.vram = 0;
	sheetctrl->base.position.x = 0;
	sheetctrl->base.position.y = 0;
	sheetctrl->base.size.x = 0;
	sheetctrl->base.size.y = 0;
	sheetctrl->base.bpp = 0;
	sheetctrl->base.invcol = 0;
	sheetctrl->base.next = 0;
	sheetctrl->base.before = 0;
	sheetctrl->base.Refresh = 0;
	sheetctrl->base.WriteMap = 0;
	sheetctrl->base.visible = 0;
	sheetctrl->base.myctrl = sheetctrl;

	sheetctrl->sheets = 0;
	return;
}

UI_Sheet *Sheet_Get(UI_Sheet_Control *ctrl, uint xsize, uint ysize, uint bpp, uint invcol)
{
	UI_Sheet *sheet;

	if(bpp == 0){
		bpp = ctrl->mainvrambpp;
	}

	sheet = MemoryControl_Allocate(ctrl->memctrl, sizeof(UI_Sheet));
	sheet->vram = MemoryControl_Allocate(ctrl->memctrl, xsize * ysize * (bpp >> 3));
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
	sheet->myctrl = ctrl;

	return sheet;
}

uint Sheet_Show(UI_Sheet *sheet, int px, int py, uint height)
{
	uint i;
	UI_Sheet *now;
	UI_Sheet_Control *ctrl;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	ctrl = sheet->myctrl;

	now = &ctrl->base;
	for(i = 0; i <= height; i++){
		if(now->next == 0){
			break;
		}
		if(i == height){
			break;
		}
		now = now->next;
	}
	now->next->before = sheet;
	sheet->next = now->next;
	sheet->before = now;
	now->next = sheet;

	ctrl->sheets++;
	sheet->position.x = px;
	sheet->position.y = py;
	sheet->visible = true;

	if(ctrl->mainvrambpp == 32){
		if(sheet->bpp == 32){
			sheet->Refresh = Sheet_Refresh_32from32;
//		} else if(sheet->bpp == 16){

//		} else if(sheet->bpp == 8){

		}
	} else if(ctrl->mainvrambpp == 16){
		if(sheet->bpp == 32){
			sheet->Refresh = Sheet_Refresh_16from32;
		} else if(sheet->bpp == 16){
			sheet->Refresh = Sheet_Refresh_16from16;
//		} else if(sheet->bpp == 8){

		}
	} else if(ctrl->mainvrambpp == 8){
		if(sheet->bpp == 32){
			sheet->Refresh = Sheet_Refresh_08from32;
//		} else if(sheet->bpp == 16){

		} else if(sheet->bpp == 8){
			sheet->Refresh = Sheet_Refresh_08from08;
		}
	}
	IO_Store_EFlags(eflags);

	Sheet_Refresh_Map(sheet, sheet->position.x, sheet->position.y, sheet->position.x + sheet->size.x - 1, sheet->position.y + sheet->size.y - 1);
	sheet->Refresh(sheet, 0, 0, sheet->size.x - 1, sheet->size.y - 1);

	return i;
}

void Sheet_Slide(UI_Sheet *sheet, int px, int py)
{
	int movex, movey;
	DATA_Position2D target0, target1;
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	movex = px - sheet->position.x;
	movey = py - sheet->position.y;

	if(movex == 0 && movey == 0){	//動かない
		return;
	}
	if(movey == 0){	//横にスライドするだけ
		target0.y = sheet->position.y;
		target1.y = sheet->position.y + sheet->size.y - 1;
		if(movex > 0){	//右方向
			target0.x = sheet->position.x;
			target1.x = sheet->position.x + sheet->size.x - 1 + movex;
		} else{	//左方向
			target0.x = sheet->position.x + movex;
			target1.x = sheet->position.x + sheet->size.x - 1;
		}
	} else if(movex == 0){	//縦にスライドするだけ
		target0.x = sheet->position.x;
		target1.x = sheet->position.x + sheet->size.x - 1;
		if(movey > 0){	//下方向
			target0.y = sheet->position.y;
			target1.y = sheet->position.y + sheet->size.y - 1 + movey;
		} else{	//上方向
			target0.y = sheet->position.y + movey;
			target1.y = sheet->position.y + sheet->size.y - 1;
		}
	} else if(movex > 0 && movey > 0){	//右下へスライド
		target0.x = sheet->position.x;
		target0.y = sheet->position.y;
		target1.x = sheet->position.x + sheet->size.x - 1 + movex;
		target1.y = sheet->position.y + sheet->size.y - 1 + movey;
	} else if(movex > 0 && movey < 0){	//右上へスライド
		target0.x = sheet->position.x;
		target0.y = sheet->position.y + movey;
		target1.x = sheet->position.x + sheet->size.x - 1 + movex;
		target1.y = sheet->position.y + sheet->size.y - 1;
	} else if(movex < 0 && movey > 0){	//左下へスライド
		target0.x = sheet->position.x + movex;
		target0.y = sheet->position.y;
		target1.x = sheet->position.x + sheet->size.x - 1;
		target1.y = sheet->position.y + sheet->size.y - 1 + movey;
	} else if(movex < 0 && movey < 0){	//左上へスライド
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
	Sheet_Refresh_All(ctrl->base.next, sheet->next, target0.x, target0.y, target1.x, target1.y);
	return;
}

uint Sheet_UpDown(UI_Sheet *sheet, uint height)
{
	uint i;
	UI_Sheet *now;

	now = sheet;
	for(i = 0; ; i++){
		if(now->before->before == 0){
			break;
		}
		now = now->before;
	}
	if(i == height - 1){
		return i;
	}
	Sheet_Remove(sheet);
	return Sheet_Show(sheet, sheet->position.x, sheet->position.y, height - 1);
}

void Sheet_Remove(UI_Sheet *sheet)
{
	UI_Sheet_Control *ctrl;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	ctrl = sheet->myctrl;

	ctrl->sheets--;
	ctrl = sheet->myctrl;
	sheet->visible = false;
	Sheet_Refresh_Map(sheet, sheet->position.x, sheet->position.y, sheet->position.x + sheet->size.x - 1, sheet->position.y + sheet->size.y - 1);
	sheet->before->next = sheet->next;
	sheet->next->before = sheet->before;
	Sheet_Refresh_All(ctrl->base.next, sheet->next, sheet->position.x, sheet->position.y, sheet->position.x + sheet->size.x - 1, sheet->position.y + sheet->size.y - 1);
	sheet->next = 0;
	sheet->before = 0;

	IO_Store_EFlags(eflags);
	return;
}

void Sheet_Refresh_Map(UI_Sheet *sheet, int x0, int y0, int x1, int y1)
{
	UI_Sheet **before;
	uint i, x, y;
	DATA_Position2D target0, target1;
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	if(x0 >= ctrl->mainvramsize.x || y0 >= ctrl->mainvramsize.y || x1 < 0 || y1 < 0){
		return;
	}
	if(x0 < 0){
		x0 = 0;
	}
	if(y0 < 0){
		y0 = 0;
	}
	if(x1 >= ctrl->mainvramsize.x){
		x1 = ctrl->mainvramsize.x - 1;
	}
	if(y1 >= ctrl->mainvramsize.y){
		y1 = ctrl->mainvramsize.y - 1;
	}

	before = &ctrl->base.next;
	for(i = 0; i < ctrl->sheets; i++){
		for(y = y0; y <= y1; y++){
			for(x = x0; x <= x1; x++){
				if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == (uint)*before){
					ctrl->map[(y * ctrl->mainvramsize.x) + x] = 0;
				}
			}
		}
		if(*before == sheet){
			break;
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
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == 0 && ((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)] != sheet->invcol){
				ctrl->map[(y * ctrl->mainvramsize.x) + x] = (uint)sheet;
			}
		}
	}
	return;
}

void Sheet_Write_Map_16(UI_Sheet *sheet, int x0, int y0, int x1, int y1)
{
	int x, y;
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == 0 && ((ushort *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)] != (ushort)sheet->invcol){
				ctrl->map[(y * ctrl->mainvramsize.x) + x] = (uint)sheet;
			}
		}
	}
	return;
}

void Sheet_Write_Map_08(UI_Sheet *sheet, int x0, int y0, int x1, int y1)
{
	int x, y;
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == 0 && ((uchar *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)] != (uchar)sheet->invcol){
				ctrl->map[(y * ctrl->mainvramsize.x) + x] = (uint)sheet;
			}
		}
	}
	return;
}

void Sheet_Write_Map_NoInvisible(UI_Sheet *sheet, int x0, int y0, int x1, int y1)
{
	int x, y;
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == 0){
				ctrl->map[(y * ctrl->mainvramsize.x) + x] = (uint)sheet;
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
	UI_Sheet_Control *ctrl;

	ctrl = sheet0->myctrl;
	now = sheet0;
	for(i = 0; i < ctrl->sheets; i++){
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
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= ctrl->mainvramsize.x || py0 >= ctrl->mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= ctrl->mainvramsize.x){
		px1 = ctrl->mainvramsize.x - 1;
	}
	if(py1 >= ctrl->mainvramsize.y){
		py1 = ctrl->mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == (uint)sheet){
				((uint *)ctrl->mainvram)[(y * ctrl->mainvramsize.x) + x] = ((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)];
			}
		}
	}
	return;
}

void Sheet_Refresh_16from32(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	int x, y;
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= ctrl->mainvramsize.x || py0 >= ctrl->mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= ctrl->mainvramsize.x){
		px1 = ctrl->mainvramsize.x - 1;
	}
	if(py1 >= ctrl->mainvramsize.y){
		py1 = ctrl->mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == (uint)sheet){
				((ushort *)ctrl->mainvram)[(y * ctrl->mainvramsize.x) + x] = RGB_32_To_16(((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)]);
			}
		}
	}
	return;
}

void Sheet_Refresh_08from32(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	int x, y;
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= ctrl->mainvramsize.x || py0 >= ctrl->mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= ctrl->mainvramsize.x){
		px1 = ctrl->mainvramsize.x - 1;
	}
	if(py1 >= ctrl->mainvramsize.y){
		py1 = ctrl->mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == (uint)sheet){
				((uchar *)ctrl->mainvram)[(y * ctrl->mainvramsize.x) + x] = RGB_32_To_08_xy(((uint *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)], x, y);
			}
		}
	}
	return;
}

void Sheet_Refresh_16from16(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	int x, y;
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= ctrl->mainvramsize.x || py0 >= ctrl->mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= ctrl->mainvramsize.x){
		px1 = ctrl->mainvramsize.x - 1;
	}
	if(py1 >= ctrl->mainvramsize.y){
		py1 = ctrl->mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == (uint)sheet){
				((ushort *)ctrl->mainvram)[(y * ctrl->mainvramsize.x) + x] = ((ushort *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)];
			}
		}
	}
	return;
}

void Sheet_Refresh_08from08(UI_Sheet *sheet, int px0, int py0, int px1, int py1)
{
	int x, y;
	UI_Sheet_Control *ctrl;

	ctrl = sheet->myctrl;
	px0 = px0 + sheet->position.x;
	py0 = py0 + sheet->position.y;
	px1 = px1 + sheet->position.x;
	py1 = py1 + sheet->position.y;

	if(px0 >= ctrl->mainvramsize.x || py0 >= ctrl->mainvramsize.y || px1 < 0 || py1 < 0){
		return;
	}
	if(px0 < 0){
		px0 = 0;
	}
	if(py0 < 0){
		py0 = 0;
	}
	if(px1 >= ctrl->mainvramsize.x){
		px1 = ctrl->mainvramsize.x - 1;
	}
	if(py1 >= ctrl->mainvramsize.y){
		py1 = ctrl->mainvramsize.y - 1;
	}

	for(y = py0; y <= py1; y++){
		for(x = px0; x <= px1; x++){
			if(ctrl->map[(y * ctrl->mainvramsize.x) + x] == (uint)sheet){
				((uchar *)ctrl->mainvram)[(y * ctrl->mainvramsize.x) + x] = ((uchar *)sheet->vram)[((y - sheet->position.y) * sheet->size.x) + (x - sheet->position.x)];
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

	for(i = 0; s[i] != 0x00; i++){

	}

	if(y > sheet->size.y - 16){
		return;
	}

	if(sheet->bpp == 32){
		Draw_Put_String_32(sheet->vram, sheet->size.x, x, y, c, s);
	} else if(sheet->bpp == 16){
		Draw_Put_String_16(sheet->vram, sheet->size.x, x, y, c, s);
	} else if(sheet->bpp == 8){
		Draw_Put_String_08(sheet->vram, sheet->size.x, x, y, c, s);
	}

	sheet->Refresh(sheet, x, y, x + (i * 8), y + 16);
	return;
}

void Sheet_Draw_Put_String_With_BackColor(UI_Sheet *sheet, uint x, uint y, uint c, uint bc, const uchar *s)
{
	uint i;

	for(i = 0; s[i] != 0x00; i++){

	}

	Sheet_Draw_Fill_Rectangle(sheet, bc, x, y, x + (i * 8) - 1,  y + 16 - 1);

	if(y > sheet->size.y - 16){
		return;
	}

	if(sheet->bpp == 32){
		Draw_Put_String_32(sheet->vram, sheet->size.x, x, y, c, s);
	} else if(sheet->bpp == 16){
		Draw_Put_String_16(sheet->vram, sheet->size.x, x, y, c, s);
	} else if(sheet->bpp == 8){
		Draw_Put_String_08(sheet->vram, sheet->size.x, x, y, c, s);
	}

	sheet->Refresh(sheet, x, y, x + (i * 8), y + 16);
	return;
}

void Sheet_Draw_Fill_Rectangle(UI_Sheet *sheet, uint c, uint x0, uint y0, uint x1, uint y1)
{
	if(x1 >= sheet->size.x || y1 >= sheet->size.y){
		return;
	}

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

void System_Sheet_Initialise(void *vram, uint xsize, uint ysize, uint bpp)
{
	Sheet_Initialise(&sys_sheet_ctrl, &sys_mem_ctrl, vram, xsize, ysize, bpp);
	return;
}

UI_Sheet *System_Sheet_Get(uint xsize, uint ysize, uint bpp, uint invcol)
{
	return Sheet_Get(&sys_sheet_ctrl, xsize, ysize, bpp, invcol);
}

UI_Sheet *Sheet_Get_From_Position(UI_Sheet_Control *ctrl, int x, int y)
{
	if(x < 0 || y < 0 || x > ctrl->mainvramsize.x || y > ctrl->mainvramsize.y){
		return 0;
	}
	return (UI_Sheet *)ctrl->map[(y * ctrl->mainvramsize.x) + x];
}

uint Sheet_Get_Top_Of_Height(UI_Sheet_Control *ctrl)
{
	if(ctrl->sheets < 1){
		return 0;
	}
	return ctrl->sheets - 1;
}

uint System_Sheet_Get_Top_Of_Height(void)
{
	return Sheet_Get_Top_Of_Height(&sys_sheet_ctrl);
}

