
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
		win = make_window_app_compatible_hrb((uchar *)(ecx + app_ds_base), esi, edi, 200, 100, 4, true, (uint *)(ebx + app_ds_base));
		reg[7] = GetWindowNumber(win);
	} else if(edx == 6){
		putfonts_win_no_bc_compatible_hrb(GetWindowInfo(ebx), esi, edi, eax, (uchar *)(ebp + app_ds_base));
	} else if(edx == 7){
		boxfill_win_compatible_hrb(GetWindowInfo(ebx), ebp, eax, ecx, esi, edi);
	} else if(edx == 8){
		memman_init((IO_MemoryControl *)(ebx + app_ds_base));
		ecx &= 0xfffffff0;
		memman_free((IO_MemoryControl *)(ebx + app_ds_base), (uchar *)eax, ecx);
	} else if(edx == 9){
		ecx = (ecx + 0x0f) & 0xfffffff0;
		reg[7] = (uint)memman_alloc((IO_MemoryControl *)(ebx + app_ds_base), ecx);
	} else if(edx == 10){
		ecx = (ecx + 0x0f) & 0xfffffff0;
		memman_free((IO_MemoryControl *)(ebx + app_ds_base), (uchar *)eax, ecx);
	} else if(edx == 11){
		point_win_compatible_hrb(GetWindowInfo(ebx), eax, esi, edi);
	} else {
		cons_put_str(conswin, prompt, cursor, "Unknown api number.");
		return (uint)&(task->tss.esp0);
	}
	return 0;
}

uint GetWindowNumber(UI_Window *win)
{
	return (uint)(win - system.sys.winctl->winfos);
}

UI_Window *GetWindowInfo(uint n)
{
	return &system.sys.winctl->winfos[n];
}
