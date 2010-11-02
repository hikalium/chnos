
#include <stdio.h>
#include "core.h"

struct SYSTEM *sys_main_str_buf;

void check_newline(DATA_Position2D *p, int line_x, int line_y);

void CHNMain(void)
{
	UI_KeyInfo dec_key;
	UI_Window *testwin, *console_win;
	DATA_Position2D c_cursor;
	UI_Task *console_task;
	uchar s[64];	
	int i, mx = 0, my = 0;
	bool cursor = false;
	bool cursor_on = true;
	int key_to = 0;

	init_system();

	system.draw.init_screen(system.sys.sht.desktop_buf, system.sys.sht.taskbar_buf, *system.sys.sht.mouse_buf);

	system.draw.boxfill(system.sys.sht.core_buf, system.sys.xsize, 0x000000, 0, 0, system.sys.xsize, system.sys.ysize);
	draw_chnos_logo(system.sys.sht.core_buf, system.sys.xsize, system.sys.xsize >> 4, system.sys.xsize >> 1, (system.sys.ysize >> 1) - (system.sys.ysize / 10));
	sheet_updown(system.sys.sht.core, 0);
	sheet_refresh_full(system.sys.sht.core);
	for(;;){
		system.io.cli();
		if(system.data.fifo.status(&system.sys.fifo) == 0){
			task_sleep(system.sys.task.main);
			system.io.sti();
		} else {
			i = system.data.fifo.get(&system.sys.fifo);
			if(i - SYS_FIFO_START_KEYB == 0x1c){
				break;
			} else if(SYS_FIFO_START_MOUSE <= i && i <= SYS_FIFO_START_MOUSE + DATA_BYTE){
				decode_mouse(i - SYS_FIFO_START_MOUSE);
			}
		}
	}


	sheet_updown(system.sys.sht.core, -1);
	sheet_updown(system.sys.sht.desktop, 0);
	sheet_updown(system.sys.sht.taskbar, 1);
	sheet_updown(system.sys.sht.mouse, 2);

	sheet_refresh_full(system.sys.sht.desktop);
	sheet_refresh_full(system.sys.sht.taskbar);
	sheet_refresh_full_alpha(system.sys.sht.mouse);

	testwin = make_window("ýĳ���޳", 300, 300, 0, 0, 2, true);

	system.sys.timer.t500 = timer_alloc();
	timer_init(system.sys.timer.t500, &system.sys.fifo, SYS_FIFO_SIG_TIMERC);
	timer_settime(system.sys.timer.t500, 50);

	c_cursor.x = 0;
	c_cursor.y = 0;

	console_win = make_window("console", system.sys.cons.org_xsize, system.sys.cons.org_ysize, 10, 10, 2, false);
	console_task = task_alloc();
	console_task->tss.esp = (int)system.io.mem.alloc(64 * 1024) + 64 * 1024;
	console_task->tss.eip = (int)&console_main;
	console_task->tss.es = 1 * 8;
	console_task->tss.cs = 2 * 8;
	console_task->tss.ss = 1 * 8;
	console_task->tss.ds = 1 * 8;
	console_task->tss.fs = 1 * 8;
	console_task->tss.gs = 1 * 8;
	task_arguments(console_task, 2, console_win);
	task_run(console_task, 2, 2);


	for(;;){
		if(system.data.fifo.status(&system.sys.keycmd) > 0 && system.sys.keycmd_wait < 0){
			system.sys.keycmd_wait = system.data.fifo.get(&system.sys.keycmd);
			wait_KBC_sendready();
			system.io.out8(KEYB_DATA, system.sys.keycmd_wait);
		}
		system.io.cli();
		if(system.data.fifo.status(&system.sys.fifo) == 0){
			task_sleep(system.sys.task.main);
			system.io.sti();
		} else {
			i = system.data.fifo.get(&system.sys.fifo);
			if(i == SYS_FIFO_SIG_TIMERC){
				if(cursor_on){
					if(cursor){
						boxfill_win(testwin, 0xFFFFFF, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
						cursor = false;
					} else{
						boxfill_win(testwin, 0x000000, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
						cursor = true;
					}
				} else if(cursor){
					boxfill_win(testwin, 0xFFFFFF, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
					cursor = false;
				}
				timer_settime(system.sys.timer.t500, 50);
			} else if(SYS_FIFO_START_KEYB <= i && i <= SYS_FIFO_START_KEYB + DATA_BYTE){
				decode_key(&dec_key, i - SYS_FIFO_START_KEYB);
				putfonts_asc_sht_i(system.sys.sht.desktop, 8, 184, 0xFFFFFF, 0x000000, s); 
				if(dec_key.make && dec_key.c != 0){
					if(key_to == 0){
						s[0] = dec_key.c;
						s[1] = 0x00;
						putfonts_win(testwin, c_cursor.x, c_cursor.y, 0x000000, 0xFFFFFF, s);
						c_cursor.x += 8;
						check_newline(&c_cursor, testwin->xsize, testwin->ysize);
					} else if(key_to == 1){
						system.data.fifo.put(&console_task->fifo, dec_key.c + SYS_FIFO_START_KEYB);
					}
				} else if(dec_key.make && dec_key.keycode == 0x3B && key_shift != 0){/*Shift + F1*/
					if(console_task->tss.ss0 != 0){
						cons_put_str((UI_Window *) *((int *) 0x0fec), (DATA_Position2D *) *((int *) 0x0fe8), (DATA_Position2D *) *((int *) 0x0fe4), "\nBreak(key) :\n");
						io_cli();
						console_task->tss.eax = (int)&(console_task->tss.esp0);
						console_task->tss.eip = (int)asm_end_app;
						io_sti();
					}
				} else if(dec_key.make && dec_key.keycode == 0x0E){/*BackSpace*/
					if(key_to == 0){
						boxfill_win(testwin, 0xFFFFFF, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
						c_cursor.x -= 8;
						check_newline(&c_cursor, testwin->xsize, testwin->ysize);
						boxfill_win(testwin, 0xFFFFFF, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
					} else if(key_to == 1){
						system.data.fifo.put(&console_task->fifo, 0x0e + SYS_FIFO_START_KEYB);
					}
				} else if(dec_key.make && dec_key.keycode == 0x0f){/*Tab*/
					if(key_to == 0){
						key_to = 1;
						change_window_active(console_win, true);
						change_window_active(testwin, false);
						cursor_on = false;
						system.data.fifo.put(&console_task->fifo, CONSOLE_FIFO_CURSOR_START);
					} else{
						key_to = 0;
						change_window_active(console_win, false);
						change_window_active(testwin, true);
						cursor_on = true;
						system.data.fifo.put(&console_task->fifo, CONSOLE_FIFO_CURSOR_STOP);
					}
				} else if(dec_key.make && dec_key.keycode == 0x1c){/*Enter*/
					if(key_to == 1){
						system.data.fifo.put(&console_task->fifo, 0x0a + SYS_FIFO_START_KEYB);
					}
				}
			} else if(SYS_FIFO_START_MOUSE <= i && i <= SYS_FIFO_START_MOUSE + DATA_BYTE){
				if(decode_mouse(i - SYS_FIFO_START_MOUSE) != 0){
					mx += system.sys.mouse_decode.x;
					my += system.sys.mouse_decode.y;
					if(mx < 0) mx = 0;
					if(my < 0) my = 0;
					if(mx > system.sys.xsize - 1) mx = system.sys.xsize - 1;
					if(my > system.sys.ysize - 1) my = system.sys.ysize - 1;
					if((system.sys.mouse_decode.btn & 0x01) != 0){
						slide_window(testwin, mx - 10, my - 8);
					}
					if((system.sys.mouse_decode.btn & 0x02) != 0){
					}
					if((system.sys.mouse_decode.btn & 0x04) != 0){
					}
					system.draw.sht.slide(system.sys.sht.mouse, mx, my);
				}
			}
		}
	}
}

void check_newline(DATA_Position2D *p, int line_x, int line_y)
{
	if(p->x < 0){
		if(p->y != 0){
			p->y -= 16;
			p->x = line_x & 0xFFFFFFF8;
		} else{
			p->y = 0;
			p->x = 0;
		}
	} else if(p->x >= (line_x & 0xFFFFFFF8)){
		if(p->y <= ((line_y - 15) & 0xFFFFFFEF)){
			p->y += 16;
			p->x = 0;
		} else {
			p->x -= 8;
		}
	} 
	return;
}

