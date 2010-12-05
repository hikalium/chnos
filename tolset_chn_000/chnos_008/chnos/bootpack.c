
#include "core.h"

struct SYSTEM *sys_main_str_buf;

void CHNMain(void)
{
	uchar s[128];

	init_system();

	init_screen_i(system.draw.sht.desktop_buf, system.draw.sht.taskbar_buf, system.draw.sht.mouse_buf);

	sheet_updown(system.draw.sht.core, -1);
	sheet_updown(system.draw.sht.desktop, 0);
	sheet_updown(system.draw.sht.taskbar, 1);
	sheet_updown(system.draw.sht.mouse, 2);

	for(;;){
		io_hlt();
	}
}
