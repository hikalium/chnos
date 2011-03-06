
#include "core.h"

void InputBox_Initialise(UI_InputBox *box, void *vram, uint vxsize, uint x, uint y, uint xsize, uint ysize, uint txtbufsize, uint forecol, uint backcol)
{
	box->vram = vram;
	box->position.x = x;
	box->position.y = y;
	box->size.x = xsize;
	box->size.y = ysize;
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

	Draw_Fill_Rectangle(box->vram, box->vxsize, box->backcol, box->position.x, box->position.y, box->position.x + box->size.x, box->position.y + box->size.y);
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

	box->input_count = box->input_count + k - (2 * j);

	return k - (2 * j);

}

/*
void InputBox_Put_String_Main(UI_InputBox *box, const uchar *str)
{
	int i;
	uchar s[3];

	for(i = 0; ; i++){
		if(str[i] == 0x00){
			break;
		} else if(str[i] == '\r'){

		} else if(str[i] == '\n'){
			if(box->cursor.x != 0)InputBox_NewLine(cons);
		} else if(str[i] == '\t'){
			for(;;){
				Draw_Put_String(box->vram, box->cursor.x, box->cursor.y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, " ");
				cons->cursor.x += 8;
				cons_check_newline(cons);
				if((cons->cursor.x & 0x1f) == 0 && cons->cursor.x != 0) break;
			}
		} else{
			s[0] = str[i];
			s[1] = 0x00;
			putfonts_win(cons->win, box->cursor.x, box->cursor.y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, s);
			cons->cursor.x += 8;
			cons_check_newline(cons);
		}
	}
	return;
}
*/
