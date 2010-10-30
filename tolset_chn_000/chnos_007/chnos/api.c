
#include "core.h"

void hrb_api(uint edi, uint esi, uint ebp, uint esp, uint ebx, uint edx, uint ecx, uint eax)
{
	UI_Window *win = (UI_Window *) *((int *) 0x0fec);
	DATA_Position2D *prompt = (DATA_Position2D *) *((int *) 0x0fe8);
	DATA_Position2D *cursor = (DATA_Position2D *) *((int *) 0x0fe4);
	uint app_cs_base = *((uint *) 0x0fe0);
	if(edx == 1){
		cons_put_char(win, prompt, cursor, (uchar)(eax & 0xff));
	} else if(edx == 2){
		cons_put_str(win, prompt, cursor, (uchar *)(ebx + app_cs_base));
	}
	return;
}
