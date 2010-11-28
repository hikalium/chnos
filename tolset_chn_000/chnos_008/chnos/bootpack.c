
#include "core.h"

struct SYSTEM *sys_main_str_buf;

void CHNMain(void)
{
	uchar s[128];

	init_system();

	init_screen_i(system.draw.sht.desktop_buf, system.draw.sht.taskbar_buf, system.draw.sht.mouse_buf);

	boxfill_i(system.draw.sht.core_buf, system.data.info.boot.scrnx, 0x000000, 0, 0, system.data.info.boot.scrnx, system.data.info.boot.scrny);
	draw_chnos_logo(system.draw.sht.core_buf, system.data.info.boot.scrnx, system.data.info.boot.scrnx >> 4, system.data.info.boot.scrnx >> 1, (system.data.info.boot.scrny >> 1) - (system.data.info.boot.scrny / 10));
	sheet_updown(system.draw.sht.core, 0);

	sheet_updown(system.draw.sht.core, -1);
	sheet_updown(system.draw.sht.desktop, 0);
	sheet_updown(system.draw.sht.taskbar, 1);
	sheet_updown(system.draw.sht.mouse, 2);

	for(;;){
		io_hlt();
	}
}
