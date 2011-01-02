
#include "core.h"

uint hrb_api(uint edi, uint esi, uint ebp, uint esp, uint ebx, uint edx, uint ecx, uint eax)
{
	UI_Task *task = task_now();
	UI_Console *cons;
	UI_Window *win;
	uchar s[64];
	int i;

	uint *reg = &eax + 1;

	for(i = 0; i < MAX_CONSOLES; i++){
		if(system.ui.console.consoles[i].task == task)break;
	}
	cons = &system.ui.console.consoles[i];

	if(edx == 1){
		cons_put_char(cons, (uchar)(eax & 0xff));
	} else if(edx == 2){
		cons_put_str(cons, (uchar *)(ebx + cons->app_ds_base));
	} else if(edx == 4){
		return (uint)&(task->tss.esp0);
	} else if(edx == 5){
		win = make_window_app_compatible_hrb((uchar *)(ecx + cons->app_ds_base), esi, edi, 200, 100, sheet_get_topheight(), true, sys_memman_alloc(esi * edi * (system.data.info.vesa.BitsPerPixel >> 3)));
		win->app_buf = (uchar *)(ebx + cons->app_ds_base);
		win->app_buf_bits = (uchar)ebp;
		reg[7] = GetWindowNumber(win);
		sprintf(s, "winID = %u\n", reg[7]);
		cons_put_str(cons, s);
	} else if(edx == 6){
//		putfonts_win_no_bc_compatible_hrb(GetWindowInfo(ebx), esi, edi, eax, (uchar *)(ebp + app_ds_base));
	} else if(edx == 7){
//		boxfill_win_compatible_hrb(GetWindowInfo(ebx), ebp, eax, ecx, esi, edi);
	} else if(edx == 8){
//		memman_init((IO_MemoryControl *)(ebx + app_ds_base));
//		ecx &= 0xfffffff0;
//		memman_free((IO_MemoryControl *)(ebx + app_ds_base), (uchar *)eax, ecx);
	} else if(edx == 9){
//		ecx = (ecx + 0x0f) & 0xfffffff0;
//		reg[7] = (uint)memman_alloc((IO_MemoryControl *)(ebx + app_ds_base), ecx);
	} else if(edx == 10){
//		ecx = (ecx + 0x0f) & 0xfffffff0;
//		memman_free((IO_MemoryControl *)(ebx + app_ds_base), (uchar *)eax, ecx);
	} else if(edx == 11){
//		point_win_compatible_hrb(GetWindowInfo(ebx), eax, esi, edi);
	} else if(edx == 13){
//		line_win_compatible_hrb(GetWindowInfo(ebx), eax, ecx, esi, edi, ebp);
//		refresh_window(GetWindowInfo(ebx));
	} else {
		cons_put_str(cons, "Unknown api number.");
		return (uint)&(task->tss.esp0);
	}
	return 0;
}

uint GetWindowNumber(UI_Window *win)
{
	return (uint)(win - system.ui.window.ctrl.winfos);
}

UI_Window *GetWindowInfo(uint n)
{
	return &system.ui.window.ctrl.winfos[n];
}

