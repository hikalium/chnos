#include "core.h"

void CHNMain(void)
{	
	unsigned char s[24];
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	struct FIFO32 sysfifo;
	struct MOUSE_DECODE mdec;
	int fifobuf[256], i = 0,time_tick,mx = binfo->scrnx / 2, my = binfo->scrny / 2;
	unsigned int all_mem_size = memtest(0x00400000, 0xbffffffff);
	init_gdtidt();
	init_pic();
	init_pit(&time_tick);

	io_sti();

	init_scrn_i(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrny, vinfo->BitsPerPixel);
	fifo32_init(&sysfifo, 256, fifobuf);
	init_keyboard(&sysfifo, SYSFIFO_KEYB);
	init_mouse(&sysfifo, SYSFIFO_MOUSE, &mdec);
	pit_beep_off();

	sprintf(s,"memory %d Byte(%d KB,%d MB)",all_mem_size,all_mem_size/1024, all_mem_size/(1024*1024));
	boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,304,INT_MONITOR_LONG,320);	
	putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,304,0xffffff,s);
	
	

	for (;;){
	io_cli();
	if(fifo32_status(&sysfifo) == 0) {
		io_stihlt();
	} else {
		i = fifo32_get(&sysfifo);
	}
	if( 256 <= i && i <=511) {
			i -= SYSFIFO_KEYB;
			io_sti();
			boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,240,INT_MONITOR_LONG , 256);	
			sprintf(s,"INT 21(IRQ-1) : PS/2 ·°ÎÞ°ÄÞ%02X",i);
			putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,240,0xffffff,s);
			
	} else if(512 <= i && i <= 767) {
			i -= SYSFIFO_MOUSE;
			io_sti();
			if (decode_mouse(i) == 1) {
				boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,256,INT_MONITOR_LONG , 272);	
				sprintf(s,"INT 2C(IRQ-12) : PS/2 Ï³½(%02X,%02X,%02X)",mdec.buf[0], mdec.buf[1], mdec.buf[2]);
				putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,256,0xffffff,s);

				boxfill_i(vinfo->PhysBasePtr, binfo->scrnx,DESKTOP_COL32,mx,my,mx + 32, my + 32);

				boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,272,INT_MONITOR_LONG , 288);	
				sprintf(s,"[lcr %4d %4d]",mdec.x, mdec.y);
				if((mdec.btn & 0x01) != 0) s[1] = 'L';
				if((mdec.btn & 0x02) != 0) s[3] = 'R';
				if((mdec.btn & 0x04) != 0) s[2] = 'C';
				putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,272,0xffffff,s);
				mx += mdec.x;
				my += mdec.y;
				if(mx < 0) mx = 0;
				if(my < 0) my = 0;
				if(mx > binfo->scrnx - 24) mx = binfo->scrnx - 24;
				if(my > binfo->scrny - 24) my = binfo->scrny - 24;
				
				draw_mouse_i(vinfo->PhysBasePtr, mx, my, binfo->scrnx);
				boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,288,INT_MONITOR_LONG,304);	
				sprintf(s,"(%4d,%4d)",mx,my);
				putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,288,0xffffff,s);				
			}
		}
	}
}



