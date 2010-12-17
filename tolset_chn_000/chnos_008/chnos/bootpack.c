
#include "core.h"

struct SYSTEM *sys_main_str_buf;

void CHNMain(void)
{
	uchar s[128];
	int i;
	UI_Timer *t_1sec;
	UI_Window *testwin;

	init_system();

	init_screen_i(system.ui.draw.sht.desktop_buf, system.ui.draw.sht.taskbar_buf, system.ui.draw.sht.mouse_buf);

	sheet_updown(system.ui.draw.sht.core, -1);
	sheet_updown(system.ui.draw.sht.desktop, 0);
	sheet_updown(system.ui.draw.sht.taskbar, 1);
	sheet_updown(system.ui.draw.sht.mouse, 2);

	t_1sec = timer_alloc();
	timer_init(t_1sec, &system.data.fifo.main, 1);
	timer_settime_millisec(t_1sec, 1000);

	testwin = make_window("Ã½Ä³¨ÝÄÞ³", 200, 100, 50, 50, sheet_get_topheight(), true);


	for(;;){
		io_cli();
		if(fifo32_status(&system.data.fifo.main) == 0){
			task_sleep(system.ui.task.main);
			io_sti();
		} else{
			i = fifo32_get(&system.data.fifo.main);
			if(i == 1){
				sprintf(s, "%02X%02X/%02X/%02X", readcmos(0x32), readcmos(0x09), readcmos(0x08), readcmos(0x07));
				putfonts_asc_sht_i(system.ui.draw.sht.taskbar, (system.data.info.boot.scrnx - (8 * 10)) - 4, 4, 0x000000, 0xffffff, s);
				sprintf(s, "%02X:%02X:%02X", readcmos(0x04), readcmos(0x02), readcmos(0x00));
				putfonts_asc_sht_i(system.ui.draw.sht.taskbar, (system.data.info.boot.scrnx - (8 * 9)) - 4, 4 + 16, 0x000000, 0xffffff, s);
				timer_settime_millisec(t_1sec, 1000);
			}
		}
	}
}

void KeyBoardControlTask(void)
{
	int i;
	uchar s[128];
	UI_KeyInfo dec_key;

	for(;;){
		if(fifo32_status(&system.data.fifo.keycmd) > 0 && system.io.keyboard.cmd_wait < 0){
			system.io.keyboard.cmd_wait = fifo32_get(&system.data.fifo.keycmd);
			wait_KBC_sendready();
			io_out8(KEYB_DATA, system.io.keyboard.cmd_wait);
		}
		io_cli();
		if(fifo32_status(&system.data.fifo.keyctrl) == 0){
			task_sleep(system.ui.task.keyctrl);
			io_sti();
		} else{
			i = fifo32_get(&system.data.fifo.keyctrl);
			if(0x100 <= i && i <= 0x1ff){
				decode_key(&dec_key, i - 0x100);
				if(dec_key.make && dec_key.c != 0){/*’Êí•¶Žš*/
					sprintf(s, "[ ]");
					s[1] = dec_key.c;
					putfonts_asc_sht_i(system.ui.draw.sht.taskbar, (system.data.info.boot.scrnx - (8 * (10 + 1 + 3))) - 4, 4, 0x000000, 0xffffff, s);
				}
			}
		}
	}
}

void MouseControlTask(void)
{
	int i, mx, my, scrool = 0;
	uchar s[128];

	mx = system.data.info.boot.scrnx >> 1;
	my = system.data.info.boot.scrny >> 1;

	for(;;){
		io_cli();
		if(fifo32_status(&system.data.fifo.mousectrl) == 0){
			task_sleep(system.ui.task.mousectrl);
			io_sti();
		} else{
			i = fifo32_get(&system.data.fifo.mousectrl);
			if(0x100 <= i && i <= 0x1ff){
				if(decode_mouse(i - 0x100) != 0){
					mx += system.io.mouse.decode.x;
					my += system.io.mouse.decode.y;
					if(mx < 0) mx = 0;
					if(my < 0) my = 0;
					if(mx > system.data.info.boot.scrnx - 1) mx = system.data.info.boot.scrnx - 1;
					if(my > system.data.info.boot.scrny - 1) my = system.data.info.boot.scrny - 1;

					if(system.io.mouse.decode.scrool == 0xFFFFFFFF) scrool++;
					if(system.io.mouse.decode.scrool == 0x00000001) scrool--;

					sprintf(s, "[lcr](%4d,%4d,%04d)", mx, my, scrool);
					if((system.io.mouse.decode.btn & 0x01) != 0){
						s[1] = 'L';
					}
					if((system.io.mouse.decode.btn & 0x02) != 0){
						s[3] = 'R';
					}
					if((system.io.mouse.decode.btn & 0x04) != 0){
						s[2] = 'C';
					}
					putfonts_asc_sht_i(system.ui.draw.sht.taskbar, (system.data.info.boot.scrnx - (8 * (10 + 1 + 3 + 21 + 1))) - 4, 4, 0x000000, 0xffffff, s);
					sheet_slide(system.ui.draw.sht.mouse, mx, my);
				}
			}
		}
	}
}

