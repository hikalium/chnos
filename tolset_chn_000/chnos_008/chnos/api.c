
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
		win = make_window_app_compatible_hrb((uchar *)(ecx + cons->app_ds_base), esi, edi, 200, 100, sheet_get_topheight(), true, sys_memman_alloc(esi * edi * (system.data.info.vesa.BitsPerPixel >> 3)), task);
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
		if((ebx & 1) == 0){
			putblock_i_convert(GetWindowInfo(ebx)->buf, GetWindowInfo(ebx)->winxsize, i, edi, esi, edi + 16, GetWindowInfo(ebx)->app_buf, system.data.info.vesa.BitsPerPixel, GetWindowInfo(ebx)->app_buf_bits);
			sheet_refresh(GetWindowInfo(ebx)->win, i, edi, esi, edi + 16);
		}
	} else if(edx == 7){
		boxfill_bpp(GetWindowInfo(ebx)->app_buf, GetWindowInfo(ebx)->winxsize, ebp, eax, ecx, esi, edi, GetWindowInfo(ebx)->app_buf_bits);
		if((ebx & 1) == 0){
			putblock_i_convert(GetWindowInfo(ebx)->buf, GetWindowInfo(ebx)->winxsize, eax, ecx, esi, edi, GetWindowInfo(ebx)->app_buf, system.data.info.vesa.BitsPerPixel, GetWindowInfo(ebx)->app_buf_bits);
			sheet_refresh(GetWindowInfo(ebx)->win, eax, ecx, esi, edi);
		}
	} else if(edx == 8){
		memman_init((IO_MemoryControl *)(ebx + cons->app_ds_base));
		ecx &= 0xfffffff0;
		memman_free((IO_MemoryControl *)(ebx + cons->app_ds_base), (uchar *)eax, ecx);
	} else if(edx == 9){
		ecx = (ecx + 0x0f) & 0xfffffff0;
		reg[7] = (uint)memman_alloc((IO_MemoryControl *)(ebx + cons->app_ds_base), ecx);
	} else if(edx == 10){
		ecx = (ecx + 0x0f) & 0xfffffff0;
		memman_free((IO_MemoryControl *)(ebx + cons->app_ds_base), (uchar *)eax, ecx);
	} else if(edx == 11){
		point_bpp(GetWindowInfo(ebx)->app_buf, esi, edi, eax, GetWindowInfo(ebx)->winxsize, GetWindowInfo(ebx)->app_buf_bits);
		if((ebx & 1) == 0){
			putblock_i_convert(GetWindowInfo(ebx)->buf, GetWindowInfo(ebx)->winxsize, esi, edi, esi + 1, edi + 1, GetWindowInfo(ebx)->app_buf, system.data.info.vesa.BitsPerPixel, GetWindowInfo(ebx)->app_buf_bits);
			sheet_refresh(GetWindowInfo(ebx)->win, esi, edi, esi + 1, edi + 1);
		}
	} else if(edx == 12){
		putblock_i_convert(GetWindowInfo(ebx)->buf, GetWindowInfo(ebx)->winxsize, eax, ecx, esi, edi, GetWindowInfo(ebx)->app_buf, system.data.info.vesa.BitsPerPixel, GetWindowInfo(ebx)->app_buf_bits);
		sheet_refresh(GetWindowInfo(ebx)->win, eax, ecx, esi, edi);
	} else if(edx == 13){
		line_bpp(GetWindowInfo(ebx)->app_buf, GetWindowInfo(ebx)->winxsize, eax, ecx, esi, edi, ebp, GetWindowInfo(ebx)->app_buf_bits);
		if((ebx & 1) == 0){
			putblock_i_convert(GetWindowInfo(ebx)->buf, GetWindowInfo(ebx)->winxsize, eax, ecx, esi, edi, GetWindowInfo(ebx)->app_buf, system.data.info.vesa.BitsPerPixel, GetWindowInfo(ebx)->app_buf_bits);
			sheet_refresh(GetWindowInfo(ebx)->win, eax, ecx, esi, edi);
		}
	} else if(edx == 14){
		sys_memman_free(GetWindowInfo(ebx)->buf, GetWindowInfo(ebx)->winxsize * GetWindowInfo(ebx)->winysize * (system.data.info.vesa.BitsPerPixel >> 3));
		free_window_app(GetWindowInfo(ebx));
	} else if(edx == 15){
		for(;;){
			io_cli();
			if(fifo32_status(&cons->task->fifo) == 0){
				if(eax == 0){
					io_sti();
					reg[7] = 0xFFFFFFFF;
					return 0;
				} else{
					task_sleep(cons->task);
				}
			} else {
				i = fifo32_get(&cons->task->fifo);
				io_sti();
				if(i == 1){
					if(cons->cursor_on){
						if(cons->cursor_state){
							cons->cursor_c = CONSOLE_COLOR_CHAR;
							cons->cursor_state = false;
						} else{
							cons->cursor_c = CONSOLE_COLOR_BACKGROUND;
							cons->cursor_state = true;
						}
						boxfill_win(cons->win, cons->cursor_c, cons->cursor.x, cons->cursor.y, cons->cursor.x + 8, cons->cursor.y +16);
					}
					timer_settime(cons->timer, 50);
				} else if(i == CONSOLE_FIFO_CURSOR_START){
					cons->cursor_on = true;
				} else if(i == CONSOLE_FIFO_CURSOR_STOP){
					cons->cursor_on = false;
					boxfill_win(cons->win, CONSOLE_COLOR_BACKGROUND, cons->cursor.x, cons->cursor.y, cons->cursor.x + 8, cons->cursor.y +16);
				} else if(CONSOLE_FIFO_START_KEYB <= i && i <= CONSOLE_FIFO_START_KEYB + DATA_BYTE){
					i -= CONSOLE_FIFO_START_KEYB;
						reg[7] = i;
						return 0;
				}
			}
		}
	} else {
		cons_put_str(cons, "Unknown api number.");
		return (uint)&(task->tss.esp0);
	}
	return 0;
}

uint GetWindowNumber(UI_Window *win)
{
	return (uint)((win - system.ui.window.ctrl.winfos) << 1);
}

UI_Window *GetWindowInfo(uint n)
{
	return &system.ui.window.ctrl.winfos[n >> 1];
}
