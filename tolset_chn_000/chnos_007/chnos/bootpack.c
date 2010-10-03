
#include <stdio.h>
#include "core.h"

struct SYSTEM system;

void check_newline(struct POSITION_2D *p, int line_x, int line_y);

void CHNMain(void)
{
	struct KEYINFO dec_key;
	struct WINDOWINFO *testwin, *console_win;
	struct POSITION_2D c_cursor;
	struct TASK *console_task;
	unsigned char s[64];	
	int i, mx = 0, my = 0;
	bool cursor = false;
	int key_to = 0;

	init_system();

	system.draw.init_screen(system.sys.sht.desktop_buf, system.sys.sht.taskbar_buf, *system.sys.sht.mouse_buf);

	sheet_refresh_full(system.sys.sht.desktop);
	sheet_refresh_full(system.sys.sht.taskbar);
	sheet_refresh_full_alpha(system.sys.sht.mouse);
	sprintf(s, "ÒÓØ°: %dMB ±·: %dKB ÃÞ½¸Ä¯Ìß: %d À½¸ÊÞ°: %d Ï³½: %d", system.sys.memtotal / (1024 * 1024), system.io.mem.free_total() / 1024, system.sys.sht.desktop->height, system.sys.sht.taskbar->height, system.sys.sht.mouse->height);
	putfonts_asc_sht_i(system.sys.sht.desktop, 8, 168, 0xFFFFFF, 0x000000, s);	

	testwin = make_window("Ã½Ä³¨ÝÄÞ³", 200, 100, 200, 300, 2, true);

	system.sys.timer.t500 = timer_alloc();
	timer_init(system.sys.timer.t500, &system.sys.fifo, SYS_FIFO_SIG_TIMERC);
	timer_settime(system.sys.timer.t500, 50);

	c_cursor.x = 0;
	c_cursor.y = 0;

	console_win = make_window("console", CONSOLE_XCHARS * 8, CONSOLE_YCHARS * 16, 200, 50, 3, false);
	console_task = task_alloc();
	console_task->tss.esp = (int)system.io.mem.alloc(64 * 1024) + 64 * 1024;
	console_task->tss.eip = (int)&console_main;
	console_task->tss.es = 1 * 8;
	console_task->tss.cs = 2 * 8;
	console_task->tss.ss = 1 * 8;
	console_task->tss.ds = 1 * 8;
	console_task->tss.fs = 1 * 8;
	console_task->tss.gs = 1 * 8;
	task_arguments(console_task, 1, console_win);
	task_run(console_task, 2, 2);

	for(;;){
		system.io.cli();
		if(system.data.fifo.status(&system.sys.fifo) == 0){
			task_sleep(system.sys.task.main);
			system.io.sti();
		} else {
			i = system.data.fifo.get(&system.sys.fifo);
			if(i == SYS_FIFO_SIG_TIMERC){
				if(cursor){
					boxfill_win(testwin, 0xFFFFFF, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
					cursor = false;
				} else{
					boxfill_win(testwin, 0x000000, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
					cursor = true;
				}
				timer_settime(system.sys.timer.t500, 50);
			} else if(SYS_FIFO_START_KEYB <= i && i <= SYS_FIFO_START_KEYB + DATA_BYTE){
				decode_key(&dec_key, i - SYS_FIFO_START_KEYB);
				sprintf(s, "INT:21 IRQ:01 PS/2·°ÎÞ°ÄÞ   ");
				s[26] = dec_key.c;
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
				}else if(dec_key.make && dec_key.keycode == 0x0E){/*BackSpace*/
					if(key_to == 0){
						boxfill_win(testwin, 0xFFFFFF, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
						c_cursor.x -= 8;
						check_newline(&c_cursor, testwin->xsize, testwin->ysize);
						boxfill_win(testwin, 0xFFFFFF, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
					} else if(key_to == 1){
						system.data.fifo.put(&console_task->fifo, 0x0E + SYS_FIFO_START_KEYB);
					}
				}else if(dec_key.make && dec_key.keycode == 0x0f){/*Tab*/
					if(key_to == 0){
						key_to = 1;
						change_window_active(console_win, true);
						change_window_active(testwin, false);
					} else{
						key_to = 0;
						change_window_active(console_win, false);
						change_window_active(testwin, true);
					}
				}
				sprintf(s, "testwin size x: %5d y %5d c_cursor x: %5d y: %5d", testwin->xsize, testwin->ysize, c_cursor.x, c_cursor.y);
				putfonts_asc_sht_i(system.sys.sht.desktop, 8, 232, 0xFFFFFF, 0x000000, s); 
			} else if(SYS_FIFO_START_MOUSE <= i && i <= SYS_FIFO_START_MOUSE + DATA_BYTE){
				if(decode_mouse(i - SYS_FIFO_START_MOUSE) != 0){
					mx += system.sys.mouse_decode.x;
					my += system.sys.mouse_decode.y;
					if(mx < 0) mx = 0;
					if(my < 0) my = 0;
					if(mx > system.sys.xsize - 1) mx = system.sys.xsize - 1;
					if(my > system.sys.ysize - 1) my = system.sys.ysize - 1;
					sprintf(s, "INT:2C IRQ:12 PS/2Ï³½ [lcr %4d %4d]", system.sys.mouse_decode.x, system.sys.mouse_decode.y);
					if((system.sys.mouse_decode.btn & 0x01) != 0){
						s[23] = 'L';
						slide_window(testwin, mx - 10, my - 8);
					}
					if((system.sys.mouse_decode.btn & 0x02) != 0){
						s[25] = 'R';
					}
					if((system.sys.mouse_decode.btn & 0x04) != 0){
						s[24] = 'C';
					}
					putfonts_asc_sht_i(system.sys.sht.desktop, 8, 200, 0xFFFFFF, 0x000000, s);
					sprintf(s, "(%3d, %3d)", mx, my);
					putfonts_asc_sht_i(system.sys.sht.desktop, 8, 216, 0xFFFFFF, 0x000000, s);
					system.draw.sht.slide(system.sys.sht.mouse, mx, my);
				}
			}
		}
	}
}

void check_newline(struct POSITION_2D *p, int line_x, int line_y)
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

