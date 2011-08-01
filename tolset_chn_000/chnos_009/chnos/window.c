
#include "core.h"

uchar closebutton[16][40] = {
	"OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO",
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

UI_Window *window_root;

void Initialise_Window(UI_Window *windowctrl)
{
	window_root = windowctrl;
	window_root->client = 0;
	window_root->control = 0;
	window_root->title[0] = 0x00;
	window_root->next = 0;
	return;
}

UI_Window *Window_Create(const uchar *title, uint flags, uint xsize, uint ysize)
{
	DATA_BootInfo *boot = (DATA_BootInfo *)ADR_BOOTINFO;
	UI_Window *win, *end;
	uint i;
	int x, y;

	win = MemoryBlock_Allocate_System(sizeof(UI_Window));
	MemoryBlock_Write_Description(win, "UI_Window");

	for(i = 0; i < (WINDOW_TITLE_LENGTH - 1); i++){
		if(title[i] == 0x00){
			break;
		}
		win->title[i] = title[i];
	}
	win->title[i] = 0x00;

	win->control = System_Sheet_Get((i + 8) << 3, WINDOW_CONTROL_XSIZE, 0, 0);
	win->client = System_Sheet_Get(xsize, ysize, 0, 0);
	Sheet_Set_Movable(win->control, true);

	Sheet_Draw_Fill_Rectangle_Gradation_Vertical(win->control, 0xffffff, 0x7cfc00, 0, 0, win->control->size.x - 1, win->control->size.y - 1);

	Sheet_Draw_Fill_Rectangle(win->client, 0xffffff, 0, 0, win->client->size.x - 1, win->client->size.y - 1);

	Window_Draw_CloseButton(win, false);

//	Sheet_Draw_Fill_Rectangle(win->control, 0xffffff, 0, 0, win->control->size.x - 1, 0);
//	Sheet_Draw_Fill_Rectangle(win->control, 0xffffff, 0, 0, 0, win->control->size.y - 1);
//	Sheet_Draw_Fill_Rectangle(win->control, 0x32cd32, win->control->size.x - 1, 0, win->control->size.x - 1, win->control->size.y - 1);
//	Sheet_Draw_Fill_Rectangle(win->control, 0x32cd32, 0, win->control->size.y - 1, win->control->size.x - 1, win->control->size.y - 1);

	x = (boot->scrnx >> 1) - (win->client->size.x >> 1);
	y = (boot->scrny >> 1) - (win->client->size.y >> 1) - (win->control->size.y >> 1);

	if(x < 0){
		x = 0;
	}
	if(y < 0){
		y = 0;
	}
	Sheet_Draw_Put_String(win->control, 4 + 8, 4, 0x00000000, win->title);

	Sheet_Set_MouseEventProcedure(win->control, &Window_Control_MouseEventProcedure, 0);

	Sheet_Show(win->control, x, y, System_Sheet_Get_Top_Of_Height());
	Sheet_Show(win->client, x, y + win->control->size.y, System_Sheet_Get_Top_Of_Height());

	for(end = window_root; end->next != 0; end = end->next){

	}
	win->next = 0;
	end->next = win;

	CHNOS_UI_KeyFocus_Change(win->client);

	return win;
}

UI_Window *Window_Create_User(const uchar *title, uint flags, UI_Sheet *client)
{
	DATA_BootInfo *boot = (DATA_BootInfo *)ADR_BOOTINFO;
	UI_Window *win, *end;
	uint i;
	int x, y;

	win = MemoryBlock_Allocate_System(sizeof(UI_Window));
	MemoryBlock_Write_Description(win, "UI_Window");

	for(i = 0; i < (WINDOW_TITLE_LENGTH - 1); i++){
		if(title[i] == 0x00){
			break;
		}
		win->title[i] = title[i];
	}
	win->title[i] = 0x00;

	win->control = System_Sheet_Get((i + 8) << 3, WINDOW_CONTROL_XSIZE, 0, 0);
	win->client = client;
	Sheet_Set_Movable(win->control, true);

	Sheet_Draw_Fill_Rectangle_Gradation_Vertical(win->control, 0xffffff, 0x7cfc00, 0, 0, win->control->size.x - 1, win->control->size.y - 1);

	Window_Draw_CloseButton(win, false);

//	Sheet_Draw_Fill_Rectangle(win->control, 0xffffff, 0, 0, win->control->size.x - 1, 0);
//	Sheet_Draw_Fill_Rectangle(win->control, 0xffffff, 0, 0, 0, win->control->size.y - 1);
//	Sheet_Draw_Fill_Rectangle(win->control, 0x32cd32, win->control->size.x - 1, 0, win->control->size.x - 1, win->control->size.y - 1);
//	Sheet_Draw_Fill_Rectangle(win->control, 0x32cd32, 0, win->control->size.y - 1, win->control->size.x - 1, win->control->size.y - 1);

	x = (boot->scrnx >> 1) - (win->client->size.x >> 1);
	y = (boot->scrny >> 1) - (win->client->size.y >> 1) - (win->control->size.y >> 1);

	if(x < 0){
		x = 0;
	}
	if(y < 0){
		y = 0;
	}
	Sheet_Draw_Put_String(win->control, 4 + 8, 4, 0x00000000, win->title);

	Sheet_Set_MouseEventProcedure(win->control, &Window_Control_MouseEventProcedure, 0);

	Sheet_Show(win->control, x, y, System_Sheet_Get_Top_Of_Height());

	if(!win->client->visible){
		Sheet_Show(win->client, x, y + win->control->size.y, System_Sheet_Get_Top_Of_Height());
	} else{
		Sheet_Slide(win->client, x, y + win->control->size.y);
		Sheet_UpDown(win->client, System_Sheet_Get_Top_Of_Height());
	}

	for(end = window_root; end->next != 0; end = end->next){

	}
	win->next = 0;
	end->next = win;

	CHNOS_UI_KeyFocus_Change(win->client);

	return win;
}

UI_Window *Window_Get_From_Sheet(UI_Sheet *sheet)
{
	UI_Window *now;

	for(now = window_root->next; now != 0; now = now->next){
		if(now->control == sheet || now->client == sheet){
			return now;
		}
	}
	return 0;
}

void Window_Control_MouseEventProcedure(UI_MouseEventArguments *e)
{
	UI_Window *focus_win;

	focus_win = Window_Get_From_Sheet(e->focus);

	if(focus_win == 0){
		return;
	}

	if((e->button & MOUSE_BUTTON_L) != 0 && (e->button_before & MOUSE_BUTTON_L) == 0){	/*L down*/
		CHNOS_UI_KeyFocus_Change(focus_win->client);
		Sheet_UpDown(focus_win->client, System_Sheet_Get_Top_Of_Height() - 1);
		if(focus_win->control->size.x - 44 <= e->position_local.x && e->position_local.x <= focus_win->control->size.x - 4 - 1 && 0 <= e->position_local.y && e->position_local.y <= 16 - 1){
			Window_Draw_CloseButton(focus_win, true);
		}
	}
	if((e->button & MOUSE_BUTTON_L) == 0 && (e->button_before & MOUSE_BUTTON_L) != 0){	/*L release*/
		Sheet_Slide(focus_win->client, focus_win->control->position.x, focus_win->control->position.y + focus_win->control->size.y);
		Window_Draw_CloseButton(focus_win, false);
	}
	return;
}

void Window_Draw_CloseButton(UI_Window *win, bool pressed)
{
	uint x, y, color;
	uchar c;

	if(pressed){
		Sheet_Draw_Fill_Rectangle_Gradation_Vertical(win->control, 0xff0000, 0xffffff, win->control->size.x - 44, 0, win->control->size.x - 4 - 1, 16 - 1);
	} else{
		Sheet_Draw_Fill_Rectangle_Gradation_Vertical(win->control, 0xffffff, 0xff0000, win->control->size.x - 44, 0, win->control->size.x - 4 - 1, 16 - 1);
	}

	for(y = 0; y < 16; y++){
		for(x = 0; x < 40; x++){
			c = closebutton[y][x];
			color = 0x000000;
			if(c == '@'){
				//color = 0x8b0000;
			} else if(c == '$'){
				//color = 0xff0000;
			} else if(c == 'Q'){
				//color = 0xff0000;
			} else if(c == 'J'){
				color = 0xffffff;
			} else if(c == 'O'){
				//color = 0xffffff;
			}
			if(color != 0x000000){
				Sheet_Draw_Point(win->control, color, win->control->size.x - 44 + x, y);
			}
		}
	}

	return;
}
