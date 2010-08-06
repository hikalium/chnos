
#include <stdio.h>
#include "core.h"

struct SYSTEM system;

void CHNMain(void)
{
	unsigned char s[64];	

	init_system();

	system.io.mem.init();
	system.io.mem.free(0x00400000, system.sys.memtotal - 0x00400000);

	system.draw.init_screen(system.sys.vram, system.sys.xsize, system.sys.ysize, system.sys.bpp, (unsigned int *)system.sys.mousecursor);

	sprintf(s, "“”ÿ∞ %dMB ±∑ %dKB", system.sys.memtotal, system.io.mem.free_total);
	system.draw.putfonts(system.sys.vram, system.sys.xsize, 0, 32, s);	

	for(;;){
		system.io.hlt();
	}
}

