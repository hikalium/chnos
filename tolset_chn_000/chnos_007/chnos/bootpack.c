
#include <stdio.h>
#include "core.h"

struct SYSTEM system;

void CHNMain(void)
{
	struct KEYINFO dec_key;
	unsigned char s[64];	
	int i, mx = 0, my = 0;
	init_system();

	system.draw.init_screen(system.sys.sht.desktop_buf, system.sys.sht.taskbar_buf, *system.sys.sht.mouse_buf);

	sheet_refresh_full(system.sys.sht.desktop);
	sheet_refresh_full(system.sys.sht.taskbar);
	sheet_refresh_full_alpha(system.sys.sht.mouse);
	sprintf(s, "ÒÓØ°: %dMB ±·: %dKB ÃÞ½¸Ä¯Ìß: %d À½¸ÊÞ°: %d Ï³½: %d", system.sys.memtotal / (1024 * 1024), system.io.mem.free_total() / 1024, system.sys.sht.desktop->height, system.sys.sht.taskbar->height, system.sys.sht.mouse->height);
	putfonts_asc_sht_i(system.sys.sht.desktop, 8, 168, 0xFFFFFF, 0x000000, s);	

	make_window32("Ã½Ä³¨ÝÄÞ³", 200, 100, 100, 100, 2);

	for(;;){
		system.io.cli();
		if(system.data.fifo.status(&system.sys.fifo) == 0){
			system.io.stihlt();
		} else {
			i = system.data.fifo.get(&system.sys.fifo);
			if(256 <= i && i < 512){
				decode_key(&dec_key, i - 256);
				sprintf(s, "INT:21 IRQ:01 PS/2·°ÎÞ°ÄÞ   ");
				s[26] = dec_key.c;
				putfonts_asc_sht_i(system.sys.sht.desktop, 8, 184, 0xFFFFFF, 0x000000, s);
			} else if(512 <= i && i < 768){
				if(decode_mouse(i - 512) != 0){
					sprintf(s, "INT:2C IRQ:12 PS/2Ï³½ [lcr %4d %4d]", system.sys.mouse_decode.x, system.sys.mouse_decode.y);
					if((system.sys.mouse_decode.btn & 0x01) != 0) s[23] = 'L';
					if((system.sys.mouse_decode.btn & 0x02) != 0) s[25] = 'R';
					if((system.sys.mouse_decode.btn & 0x04) != 0) s[24] = 'C';
					putfonts_asc_sht_i(system.sys.sht.desktop, 8, 200, 0xFFFFFF, 0x000000, s);
					mx += system.sys.mouse_decode.x;
					my += system.sys.mouse_decode.y;
					if(mx < 0) mx = 0;
					if(my < 0) my = 0;
					if(mx > system.sys.xsize - 1) mx = system.sys.xsize - 1;
					if(my > system.sys.ysize - 1) my = system.sys.ysize - 1;
					sprintf(s, "(%3d, %3d)", mx, my);
					putfonts_asc_sht_i(system.sys.sht.desktop, 8, 216, 0xFFFFFF, 0x000000, s);
					system.draw.sht.slide(system.sys.sht.mouse, mx, my);
				}
			}
		}
	}
}

