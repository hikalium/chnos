
#include <stdio.h>
#include "core.h"

struct SYSTEM system;

void CHNMain(void)
{
	struct KEYINFO dec_key;
	unsigned char s[64];	
	int i;
	init_system();

	system.draw.init_screen(system.sys.sht.desktop_buf, system.sys.sht.taskbar_buf, *system.sys.sht.mouse_buf);

	sheet_refresh_full(system.sys.sht.desktop);
	sheet_refresh_full(system.sys.sht.taskbar);
	sheet_refresh_full_alpha(system.sys.sht.mouse);
	sprintf(s, "ÒÓØ°: %dMB ±·: %dKB ÃÞ½¸Ä¯Ìß: %d À½¸ÊÞ°: %d Ï³½: %d", system.sys.memtotal / (1024 * 1024), system.io.mem.free_total() / 1024, system.sys.sht.desktop->height, system.sys.sht.taskbar->height, system.sys.sht.mouse->height);
	putfonts_asc_sht_i(system.sys.sht.desktop, 8, 168, 0xFFFFFF, 0x000000, s);	

	for(;;){
		system.io.cli();
		if(system.data.fifo.status(&system.sys.fifo) == 0){
			system.io.stihlt();
		} else {
			i = system.data.fifo.get(&system.sys.fifo);
			if(256 <= i && i <=511){
				decode_key(&dec_key, i - 256);
				sprintf(s, "INT:21 IRQ:01 PS/2·°ÎÞ°ÄÞ   ");
				s[26] = dec_key.c;
				putfonts_asc_sht_i(system.sys.sht.desktop, 8, 184, 0xFFFFFF, 0x000000, s);
			}
		}
	}
}

