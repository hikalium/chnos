
#include "core.h"

void InputBox_Initialise(UI_InputBox *box, void *vram, uint vxsize, uint x, uint y, uint xsize, uint ysize, uint txtbufsize, uint forecol, uint backcol)
{
	box->vram = vram;
	box->position.x = x;
	box->position.y = y;
	box->size.x = xsize & ~7;
	box->size.y = ysize & ~15;
	box->input_buf_size = txtbufsize;
	box->input_buf = (uchar *)System_MemoryControl_Allocate(box->input_buf_size);
	box->input_count = 0;
	box->cursor.x = 0;
	box->cursor.y = 0;
	box->prompt.x = 0;
	box->prompt.y = 0;
	box->vxsize = vxsize;
	box->forecol = forecol;
	box->backcol = backcol;

	Draw_Fill_Rectangle(box->vram, box->vxsize, box->backcol, box->position.x, box->position.y, box->position.x + xsize, box->position.y + ysize);
	return;
}

int InputBox_Put_String(UI_InputBox *box, const uchar *s)
{
	uint i, j;
	int k;

	j = 0;

	for(i = 0; (i - j) < (box->input_buf_size + 1); i++){
		if(s[i] == 0x00){
			break;
		}
		if(s[i] == '\b'){
			j++;
		}
	}

	if((i - j) > (box->input_buf_size - box->input_count)){
		return -1;
	}

	j = 0;

	for(k = 0; k < i; k++){
		if(s[k] == '\b'){
			box->input_buf[box->input_count + k - (2 * j) - 1] = 0x00;
			j++;
		} else{
			box->input_buf[box->input_count + k - (2 * j)] = s[k];
		}
	}
	box->input_buf[box->input_count + k - (2 * j)] = 0x00;

	InputBox_Put_String_Main(box, s);

	box->input_count = box->input_count + k - (2 * j);

	return k - (2 * j);

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
				Draw_Put_String(box->vram, box->vxsize, box->cursor.x, box->cursor.y, box->forecol, " ");
				box->cursor.x += 8;
				InputBox_Check_NewLine(box);
				if((box->cursor.x & 0x1f) == 0 && box->cursor.x != 0) break;
			}
		} else if(str[i] == '\b'){
			Draw_Fill_Rectangle(box->vram, box->vxsize, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8, box->cursor.y + 16);
			box->cursor.x -= 8;
			InputBox_Check_NewLine(box);
			Draw_Fill_Rectangle(box->vram, box->vxsize, box->backcol, box->cursor.x, box->cursor.y, box->cursor.x + 8, box->cursor.y + 16);
			if(box->input_count != 0){
				box->input_count--;
				box->input_buf[box->input_count] = 0x00;
			}
		} else{
			s[0] = str[i];
			s[1] = 0x00;
			Draw_Put_String(box->vram, box->vxsize, box->cursor.x, box->cursor.y, box->forecol, s);
			box->cursor.x += 8;
			InputBox_Check_NewLine(box);
		}
	}
	return;
}

void InputBox_Check_NewLine(UI_InputBox *box)
{
	if(box->cursor.x <= box->prompt.x){
		if(box->cursor.y != box->prompt.y){
			if(box->cursor.x < box->prompt.x){
				box->cursor.y -= 16;
				box->cursor.x = box->size.x - 8;
			}
		} else{
			box->cursor.y = box->prompt.y;
			box->cursor.x = 8;
		}
	} else if(box->cursor.x >= box->size.x){
		if(box->cursor.y <= box->size.y - 17){
			box->cursor.x = 0;
			box->cursor.y += 16;
		} else{
			InputBox_Slide_Line(box);
			box->cursor.x = 0;
			if(box->prompt.y > 0) box->prompt.y -= 16;
			else{
				box->prompt.y = 0;
				box->prompt.x = 0;
			}
		}
	} 
	return;
}

void InputBox_NewLine_No_Prompt(UI_InputBox *box)
{
	if(box->cursor.y <= box->size.y - 17){
		Draw_Put_String(box->vram, box->vxsize, box->cursor.x, box->cursor.y, box->forecol, " ");
		box->cursor.x = 0;
		box->cursor.y = box->cursor.y + 16;
	} else{
		Draw_Put_String(box->vram, box->vxsize, box->cursor.x, box->cursor.y, box->forecol, " ");
		InputBox_Slide_Line(box);
		box->cursor.x = 0;
	}
	return;
}

void InputBox_Slide_Line(UI_InputBox *box)
{
	Draw_Slide_Line(box->vram, box->size.x, box->size.y, box->vxsize, box->position.x, box->position.y);
	Draw_Fill_Rectangle(box->vram, box->vxsize, box->backcol, 0, box->position.y + box->size.y - 16, box->position.x + box->size.x, box->position.y + box->size.y);
	return;
}

