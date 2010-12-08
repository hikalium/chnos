
#include "core.h"

struct SYSTEM *sys_main_str_buf;

void CHNMain(void)
{
	uchar s[128];
	int i;
	UI_Timer *t_10sec;

	init_system();

	init_screen_i(system.ui.draw.sht.desktop_buf, system.ui.draw.sht.taskbar_buf, system.ui.draw.sht.mouse_buf);

	sheet_updown(system.ui.draw.sht.core, -1);
	sheet_updown(system.ui.draw.sht.desktop, 0);
	sheet_updown(system.ui.draw.sht.taskbar, 1);
	sheet_updown(system.ui.draw.sht.mouse, 2);

	t_10sec = timer_alloc();
	timer_init(t_10sec, &system.data.fifo.main, 10);
	timer_settime(t_10sec, 1000);

	for(;;){
		io_cli();
		if(fifo32_status(&system.data.fifo.main) == 0){
			task_sleep(system.ui.task.main);
			io_sti();
		} else{
			i = fifo32_get(&system.data.fifo.main);
			if(i == 10){
				putfonts_asc_sht_i(system.ui.draw.sht.desktop, 0, 0, 0x000000, 0xffffff, "10sec");
			}
		}
	}
}
