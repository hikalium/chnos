
#include "core.h"

uint hrb_api(uint edi, uint esi, uint ebp, uint esp, uint ebx, uint edx, uint ecx, uint eax)
{
	UI_Window *conswin = (UI_Window *) *((int *) 0x0fec);
	DATA_Position2D *prompt = (DATA_Position2D *) *((int *) 0x0fe8);
	DATA_Position2D *cursor = (DATA_Position2D *) *((int *) 0x0fe4);
	uint app_ds_base = *((uint *) 0x0fe0);
	UI_Task *task = task_now();
	UI_Window *win;

	uint *reg = &eax + 1;

	if(edx == 1){
		cons_put_char(conswin, prompt, cursor, (uchar)(eax & 0xff));
	} else if(edx == 2){
		cons_put_str(conswin, prompt, cursor, (uchar *)(ebx + app_ds_base));
	} else if(edx == 4){
		return (uint)&(task->tss.esp0);
	} else if(edx == 5){
		if(ebx == 0){
			reg[7] = (esi * edi) * (system.sys.bpp >> 2);
		} else{
			win = make_window_app_hrb((uchar *)(ecx + app_ds_base), esi, edi, 100, 50, 4, true, (uint *)(ebx + app_ds_base));
			reg[7] = GetWindowNumber(win);
		}
	} else {
		cons_put_str(conswin, prompt, cursor, "Unknown api number.");
		return (uint)&(task->tss.esp0);
	}
	return 0;
}

uint GetWindowNumber(UI_Window *win)
{
	return (uint)win - (uint)&system.sys.winctl->winfos;
}

UI_Window *GetWindowInfo(uint n)
{
	return &system.sys.winctl->winfos[n];
}
