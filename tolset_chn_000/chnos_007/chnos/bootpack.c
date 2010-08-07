
#include <stdio.h>
#include "core.h"

struct SYSTEM system;

void CHNMain(void)
{
	unsigned char s[64];	

	init_system();

	system.draw.init_screen(system.sys.sht.desktop_buf, system.sys.xsize, system.sys.ysize, system.sys.bpp, (unsigned int *)system.sys.sht.mouse_buf);
	sprintf(s, "“”ÿ∞: %dMB ±∑: %dKB", system.sys.memtotal / (1024 * 1024), system.io.mem.free_total() / 1024);
	putfonts_asc_i(system.sys.sht.desktop_buf, system.sys.xsize, 8, 168, 0xFFFFFF, s);	
	system.draw.sht.slide(system.sys.sht.desktop, 0, 0);
	system.draw.sht.updown(system.sys.sht.desktop, 1);
	system.draw.sht.slide(system.sys.sht.mouse,system.sys.xsize / 2, system.sys.ysize / 2);
	system.draw.sht.updown(system.sys.sht.mouse, 2);	


	for(;;){
		system.io.hlt();
	}
}

