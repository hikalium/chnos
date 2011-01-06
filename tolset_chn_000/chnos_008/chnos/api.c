
#include "core.h"

uint hrb_api(uint edi, uint esi, uint ebp, uint esp, uint ebx, uint edx, uint ecx, uint eax)
{
	UI_Task *task = task_now();
	UI_Console *cons;
	UI_Window *win;
	uchar s[64];
	uchar *app_s;
	uint i;

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
		win->app_buf_bits = 8;
		boxfill8(win->app_buf, esi, COL8_FFFFFF, 0, 0, esi, edi);
		reg[7] = GetWindowNumber(win);
		sprintf(s, "winID = %u\n", reg[7]);
		cons_put_str(cons, s);
	} else if(edx == 6){
		i = esi;
		app_s = (uchar *)(ebp + cons->app_ds_base);
		if(GetWindowInfo(ebx)->app_buf_bits == 8){
			for (; *app_s != 0x00; app_s++) {
				putfont8(GetWindowInfo(ebx)->app_buf, GetWindowInfo(ebx)->winxsize, esi, edi, eax, hankaku + (*app_s * 16));
				esi += 8;
			}
		} else if(GetWindowInfo(ebx)->app_buf_bits == 16){
			for (; *app_s != 0x00; app_s++) {
				putfont16(GetWindowInfo(ebx)->app_buf, GetWindowInfo(ebx)->winxsize, esi, edi, eax, hankaku + (*app_s * 16));
				esi += 8;
			}
		} else if(GetWindowInfo(ebx)->app_buf_bits == 32){
			for (; *app_s != 0x00; app_s++) {
				putfont32(GetWindowInfo(ebx)->app_buf, GetWindowInfo(ebx)->winxsize, esi, edi, eax, hankaku + (*app_s * 16));
				esi += 8;
			}
		}
		putblock_i_convert(GetWindowInfo(ebx)->buf, GetWindowInfo(ebx)->winxsize, i, edi, esi, edi + 16, GetWindowInfo(ebx)->app_buf, system.data.info.vesa.BitsPerPixel, GetWindowInfo(ebx)->app_buf_bits);
		sheet_refresh(GetWindowInfo(ebx)->win, i, edi, esi, edi + 16);
	} else if(edx == 7){
		if(GetWindowInfo(ebx)->app_buf_bits == 8){
			boxfill8(GetWindowInfo(ebx)->app_buf, GetWindowInfo(ebx)->winxsize, ebp, eax, ecx, esi, edi);	
		} else if(GetWindowInfo(ebx)->app_buf_bits == 16){
			boxfill16(GetWindowInfo(ebx)->app_buf, GetWindowInfo(ebx)->winxsize, ebp, eax, ecx, esi, edi);
		} else if(GetWindowInfo(ebx)->app_buf_bits == 32){
			boxfill32(GetWindowInfo(ebx)->app_buf, GetWindowInfo(ebx)->winxsize, ebp, eax, ecx, esi, edi);
		}
		putblock_i_convert(GetWindowInfo(ebx)->buf, GetWindowInfo(ebx)->winxsize, eax, ecx, esi, edi, GetWindowInfo(ebx)->app_buf, system.data.info.vesa.BitsPerPixel, GetWindowInfo(ebx)->app_buf_bits);
		sheet_refresh(GetWindowInfo(ebx)->win, eax, ecx, esi, edi);
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
