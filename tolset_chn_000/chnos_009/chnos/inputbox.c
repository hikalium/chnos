
#include "core.h"

UI_InputBox *InputBox_Initialise(UI_Sheet_Control *sheetctrl, IO_MemoryControl *memctrl, uint x, uint y, uint xsize, uint ysize, uint txtbufsize, uint forecol, uint backcol, uint height)
{
	UI_InputBox *box;

	box = MemoryBlock_Allocate_System(sizeof(UI_InputBox));
	MemoryBlock_Write_Description(box, "UI_InputBox");

	xsize = xsize & ~7;
	ysize = ysize & ~15;
	box->sheet = Sheet_Get(sheetctrl, xsize, ysize, 0, 0);
	box->input_buf_size = txtbufsize;
	box->input_buf = (uchar *)MemoryBlock_Allocate_User(box->input_buf_size, memctrl);
	MemoryBlock_Write_Description(box->input_buf, "INPBOX_TXTBUF");
	box->input_count = 0;
	box->cursor.x = 0;
	box->cursor.y = 0;
	box->prompt.x = -8;
	box->prompt.y = 0;
	box->forecol = forecol;
	box->backcol = backcol;
	box->cursor_state = false;
	box->record = true;
	Draw_Fill_Rectangle(box->sheet->vram, box->sheet->size.x, box->backcol, 0, 0, box->sheet->size.x - 1, box->sheet->size.y - 1);
	Sheet_Show(box->sheet, x, y, height);
	return box;
}

int InputBox_Put_String(UI_InputBox *box, const uchar *s)
{
	uint i;
	int count, old;

	if(!box->record){
		InputBox_Put_String_Main(box, s);
		return 0;
	}

	count = 0;

	for(i = 0; s[i] != 0x00; i++){
		if(s[i] == '\b'){
			if(count != 0){
				count--;
			}
		} else{
			count++;
		}
	}

	if(count > (box->input_buf_size - box->input_count)){
		return -1;
	}

	InputBox_Put_String_Main(box, s);

	old = count;
	count = 0;

	for(i = 0; count < old; i++){
		if(s[i] == '\b'){
			if(count != 0) count--;
		} else{
			box->input_buf[box->input_count + count] = s[i];
			count++;
		}
	}
	box->input_buf[box->input_count + count] = 0x00;
	box->input_count += count;

	return count;	
}

int InputBox_Put_Character(UI_InputBox *box, uchar c)
{
	uchar str[2];

	str[0] = c;
	str[1] = 0x00;
	return InputBox_Put_String(box, str);
}

void InputBox_Put_String_Main(UI_InputBox *box, const uchar *str)
{
	int i;
	uchar s[3];

	InputBox_Check_NewLine(box);

	for(i = 0; ; i++){
		if(str[i] == 0x00){
			break;
		} else if(str[i] == '\r'){

		} else if(str[i] == '\n'){
			if(box->cursor.x != 0){
				InputBox_NewLine_No_Prompt(box);
			}
		} else if(str[i] == '\t'){
			for(;;){
				Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8 - 1, box->cursor.y + 16 - 1);
				box->cursor.x += 8;
				InputBox_Check_NewLine(box);
				if((box->cursor.x & 0x1f) == 0 && box->cursor.x != 0) break;
			}
		} else if(str[i] == '\b'){
			Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8 - 1, box->cursor.y + 16 - 1);
			box->cursor.x -= 8;
			InputBox_Check_NewLine(box);
			Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8 - 1, box->cursor.y + 16 - 1);
			if(box->input_count != 0 && box->record){
				box->input_count--;
				box->input_buf[box->input_count] = 0x00;
			}
		} else{
			s[0] = str[i];
			s[1] = 0x00;
			Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8 - 1, box->cursor.y + 16 - 1);
			Sheet_Draw_Put_String(box->sheet, box->cursor.x, box->cursor.y, box->forecol, s);
			box->cursor.x += 8;
			InputBox_Check_NewLine(box);
		}
	}
	return;
}

void InputBox_Check_NewLine(UI_InputBox *box)
{
	if(box->cursor.x <= box->prompt.x || box->cursor.x < 0){	/*�v�����v�g�܂��͉�ʂ�荶�Ɉړ����悤�Ƃ���*/
		if(box->cursor.y != box->prompt.y){	/*�v�����v�g�Ɠ����s�ł͂Ȃ�*/
				box->cursor.y -= 16;
				box->cursor.x = box->sheet->size.x - 8;
		} else{/*�v�����v�g�Ɠ����s*/
			box->cursor.y = box->prompt.y;
			box->cursor.x = box->prompt.x + 8;
		}
	} else if(box->cursor.x >= box->sheet->size.x){
		if(box->cursor.y <= box->sheet->size.y - 17){
			box->cursor.x = box->prompt.x + 8;
			box->cursor.y += 16;
		} else{
			InputBox_Slide_Line(box);
			box->cursor.x = box->prompt.x + 8;
			if(box->prompt.y > 0){
				box->prompt.y -= 16;
			} else{
				box->prompt.y = 0;
			}
		}
	}
	if(box->cursor.y < box->prompt.y){
		box->cursor.y = box->prompt.y;
	}
	return;
}

void InputBox_NewLine_No_Prompt(UI_InputBox *box)
{
	if(box->cursor.y <= box->sheet->size.y - 17){
		Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8 - 1, box->cursor.y + 16 - 1);
		box->cursor.x = box->prompt.x + 8;
		box->cursor.y = box->cursor.y + 16;
	} else{
		Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8 - 1, box->cursor.y + 16 - 1);
		InputBox_Slide_Line(box);
		box->cursor.x = box->prompt.x + 8;
	}
	InputBox_Check_NewLine(box);
	return;
}

void InputBox_NewLine(UI_InputBox *box)
{
	if(box->cursor.y <= box->sheet->size.y - 17){
		box->prompt.y = box->cursor.y + 16;
		InputBox_Put_Prompt(box);
	} else{
		Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 16 - 1, box->cursor.y + 16 - 1);
		InputBox_Slide_Line(box);
		box->prompt.y = box->sheet->size.y - 16;
		InputBox_Put_Prompt(box);
	}
	return;
}

void InputBox_Slide_Line(UI_InputBox *box)
{
//	Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8 - 1, box->cursor.y + 16 - 1);
	Draw_Slide_Line(box->sheet->vram, box->sheet->size.x, box->sheet->size.y, box->sheet->size.x, 0, 0);
	Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, 0, box->sheet->size.y - 16, box->sheet->size.x - 1, box->sheet->size.y - 1);
	box->sheet->Refresh(box->sheet, 0, 0, box->sheet->size.x - 1, box->sheet->size.y - 1);
	return;
}

void InputBox_Put_Prompt(UI_InputBox *box)
{
	Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 16 - 1, box->cursor.y + 16 - 1);
	box->prompt.x = 0;
	Sheet_Draw_Put_String(box->sheet, box->prompt.x, box->prompt.y, box->forecol, ">");
	box->cursor.x = box->prompt.x + 8;
	box->cursor.y = box->prompt.y;
	return;
}

void InputBox_Reset_Input_Buffer(UI_InputBox *box)
{
	box->input_buf[0] = 0x00;
	box->input_count = 0;
	return;
}

void InputBox_Change_Cursor_State(UI_InputBox *box)
{
	if(box->cursor_state){
		Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8 - 1, box->cursor.y + 16 - 1);
		box->cursor_state = false;
	} else{
		Sheet_Draw_Fill_Rectangle(box->sheet, box->forecol, box->cursor.x, box->cursor.y, box->cursor.x + 8 - 1, box->cursor.y + 16 - 1);
		box->cursor_state = true;
	}
	return;
}

void InputBox_Clear(UI_InputBox *box)
{
	InputBox_Reset_Input_Buffer(box);
	box->cursor.x = 0;
	box->cursor.y = 0;
	box->prompt.x = -8;
	box->prompt.y = 0;
	Sheet_Draw_Fill_Rectangle(box->sheet, box->backcol, 0, 0, box->sheet->size.x - 1, box->sheet->size.y - 1);
	return;
}

void InputBox_Set_Record(UI_InputBox *box, bool record)
{
	box->record = record;
	return;
}
