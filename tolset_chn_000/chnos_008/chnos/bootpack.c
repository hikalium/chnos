
#include "core.h"

struct SYSTEM *sys_main_str_buf;

void CHNMain(void)
{
	uchar s[128];
	int i;
	UI_Timer *t_1sec;

	init_system();

	init_screen_i(system.ui.draw.sht.desktop_buf, system.ui.draw.sht.taskbar_buf, system.ui.draw.sht.mouse_buf);

	sheet_updown(system.ui.draw.sht.core, -1);
	sheet_updown(system.ui.draw.sht.desktop, 0);
	sheet_updown(system.ui.draw.sht.taskbar, 1);
	sheet_updown(system.ui.draw.sht.mouse, 2);

	t_1sec = timer_alloc();
	timer_init(t_1sec, &system.data.fifo.main, 1);
	timer_settime_millisec(t_1sec, 1000);

	for(;;){
		io_cli();
		if(fifo32_status(&system.data.fifo.main) == 0){
			task_sleep(system.ui.task.main);
			io_sti();
		} else{
			i = fifo32_get(&system.data.fifo.main);
			if(i == 1){
				sprintf(s, "%02X%02X/%02X/%02X %02X:%02X:%02X", readcmos(0x32), readcmos(0x09), readcmos(0x08), readcmos(0x07), readcmos(0x04), readcmos(0x02), readcmos(0x00));
				putfonts_asc_sht_i(system.ui.draw.sht.taskbar, (system.data.info.boot.scrnx - (8 * 19)) - 5, 5, 0x000000, 0xffffff, s);
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
					putfonts_asc_sht_i(system.ui.draw.sht.taskbar, (system.data.info.boot.scrnx - (8 * (19 + 1 + 3))) - 5, 5, 0x000000, 0xffffff, s);
				}
			}
		}
	}
}
