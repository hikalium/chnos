
#include "core.h"

void hrb_api(uint edi, uint esi, uint ebp, uint esp, uint ebx, uint edx, uint ecx, uint eax)
{
	struct WINDOWINFO *win = (struct WINDOWINFO *) *((int *) 0x0fec);
	struct POSITION_2D *prompt = (struct POSITION_2D *) *((int *) 0x0fe8);
	struct POSITION_2D *cursor = (struct POSITION_2D *) *((int *) 0x0fe4);
	uint app_cs_base = *((unsigned int *) 0x0fe0);
	if(edx == 1){
		cons_put_char(win, prompt, cursor, (unsigned char)(eax & 0xff));
	} else if(edx == 2){
		cons_put_str(win, prompt, cursor, (unsigned char *)(ebx + app_cs_base));
	}
	return;
}
