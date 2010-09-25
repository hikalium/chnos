
#include <stdio.h>
#include "core.h"

struct SYSTEM system;

void check_newline(struct POSITION_2D *p, int line_x, int line_y);
void task_b_main(struct WINDOWINFO *win);

void CHNMain(void)
{
	struct KEYINFO dec_key;
	struct WINDOWINFO *testwin, *win[4];
	struct POSITION_2D c_cursor;
	struct TASK *task[4];
	unsigned char s[64];	
	int i, mx = 0, my = 0;
	bool cursor = false;

	init_system();

	system.draw.init_screen(system.sys.sht.desktop_buf, system.sys.sht.taskbar_buf, *system.sys.sht.mouse_buf);

	sheet_refresh_full(system.sys.sht.desktop);
	sheet_refresh_full(system.sys.sht.taskbar);
	sheet_refresh_full_alpha(system.sys.sht.mouse);
	sprintf(s, "ÒÓØ°: %dMB ±·: %dKB ÃÞ½¸Ä¯Ìß: %d À½¸ÊÞ°: %d Ï³½: %d", system.sys.memtotal / (1024 * 1024), system.io.mem.free_total() / 1024, system.sys.sht.desktop->height, system.sys.sht.taskbar->height, system.sys.sht.mouse->height);
	putfonts_asc_sht_i(system.sys.sht.desktop, 8, 168, 0xFFFFFF, 0x000000, s);	

	testwin = make_window32("Ã½Ä³¨ÝÄÞ³", 200, 100, 200, 300, 2);

	system.sys.timer.t500 = timer_alloc();
	timer_init(system.sys.timer.t500, &system.sys.fifo, SYS_FIFO_SIG_TIMERC);
	timer_settime(system.sys.timer.t500, 50);

	c_cursor.x = 0;
	c_cursor.y = 0;

	for(i = 0; i < 4; i++){
		sprintf(s, "task:%d", i);
		win[i] = make_window32(s, 100, 16, 0, i * 50, 3);
		task[i] = task_alloc();
		task[i]->tss.esp = (int)system.io.mem.alloc(64 * 1024) + 64 * 1024;
		task[i]->tss.eip = (int)&task_b_main;
		task[i]->tss.es = 1 * 8;
		task[i]->tss.cs = 2 * 8;
		task[i]->tss.ss = 1 * 8;
		task[i]->tss.ds = 1 * 8;
		task[i]->tss.fs = 1 * 8;
		task[i]->tss.gs = 1 * 8;
		task_arguments(task[i], 1, win[i]);
		task_run(task[i], 2, i + 1);
	}
	for(;;){
		system.io.cli();
		if(system.data.fifo.status(&system.sys.fifo) == 0){
//			system.io.stihlt();
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
				if(dec_key.make && dec_key.keycode == 0x0E){
					boxfill_win(testwin, 0xFFFFFF, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
					c_cursor.x -= 8;
					check_newline(&c_cursor, testwin->xsize, testwin->ysize);
					boxfill_win(testwin, 0xFFFFFF, c_cursor.x, c_cursor.y, c_cursor.x + 8, c_cursor.y +16);
				} else if(dec_key.make && dec_key.c != 0){
					s[0] = dec_key.c;
					s[1] = 0x00;
					putfonts_win(testwin, c_cursor.x, c_cursor.y, 0x000000, 0xFFFFFF, s);
					c_cursor.x += 8;
					check_newline(&c_cursor, testwin->xsize, testwin->ysize);
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

void task_b_main(struct WINDOWINFO *win)
{
	struct FIFO32 fifo;
	struct TIMER *timer1000;
	int i, fifobuf[128], count = 0, count0 = 0;
	char s[12];

	fifo32_init(&fifo, 128, fifobuf, 0);

	timer1000 = timer_alloc();
	timer_init(timer1000, &fifo, 100);
	timer_settime(timer1000, 100);

	for(;;){
		count ++;
		io_cli();
		if(fifo32_status(&fifo) == 0){
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if(i == 100){
				sprintf(s, "%11d", count - count0);
				putfonts_win(win, 0, 0, 0xFFFFFF, 0x000000, s);
				count0 = count;
				timer_settime(timer1000, 100);	
			}
		}
	}
}
