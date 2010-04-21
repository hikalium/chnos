#include "core.h"
#include <string.h>

struct SYSTEM system;

void reboot(void);

void CHNMain(void)
{	
	unsigned char s[24];
//	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
//	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	struct FIFO32 sysfifo;
	struct MOUSE_DECODE mdec;
	static struct TIMERCTL timerctl;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET32 *sht_back, *sht_mouse;
	struct WINDOWINFO *winfo1;
	int fifobuf[256], i = 0,mx , my;
	int scrool = 0;
	volatile int time_tick;
	unsigned int all_mem_size = memtest(0x00400000, 0xbffffffff);
	unsigned int free_mem_size = 0;
	unsigned int *buf_back, buf_mouse[576], *buf_win;
	int scrnx,scrny;
	unsigned char *diskimg;


	init_system(&system);
	init_gdtidt();
	init_pic();
	io_sti();

	scrnx = system.info.boot.scrnx;
	scrny = system.info.boot.scrny ;
	mx = scrnx/2;
	my = scrny/2;
	diskimg = ADR_DISKIMG;

	system.io.memory.init(memman);
	system.io.memory.free(0x00400000,all_mem_size - 0x00400000);

	system.io.fdc.motor_off(4);
	system.io.timer.init(&timerctl);
	system.data.fifo.init(&sysfifo, 256, fifobuf);
	system.io.keyboard.init(&sysfifo, SYSFIFO_KEYB);
	system.io.mouse.init(&sysfifo, SYSFIFO_MOUSE, &mdec);
	system.draw.sheet.init(system.info.vesa.PhysBasePtr,scrnx,scrny);

	system.draw.window.init();
	system.io.beep.off();
	system.io.serial.init();



	sht_back = system.draw.sheet.alloc();
	sht_mouse = system.draw.sheet.alloc();

	buf_back = (unsigned int *) system.io.memory.alloc(scrnx * scrny * 4);
	buf_win = (unsigned int *) system.io.memory.alloc(INT_MONITOR_LONG * 150 * 4);

	system.draw.sheet.set(sht_back, buf_back, scrnx, scrny, VOID_INV_COL32);
	system.draw.sheet.set(sht_mouse, buf_mouse, 24, 24, INV_COL32);

	system.draw.init_scrn(buf_back, scrnx, scrny, system.info.vesa.BitsPerPixel,buf_mouse);

	system.draw.sheet.slide(sht_back, 0,0);
	system.draw.sheet.slide(sht_mouse, mx, my);

	system.draw.sheet.updown(sht_back, 0);

	winfo1 = system.draw.window.make(buf_win, "Ã½Ä³¨ÝÄÞ³", INT_MONITOR_LONG, 150, 200, 50, 1);	

	system.draw.sheet.updown(sht_mouse, 6);

//	system.draw.circle(buf_back, 100,100, 0xff0000, scrnx, 100);

	sprintf(s,"memory %d Byte(%d KB,%d MB) ",all_mem_size,all_mem_size/1024, all_mem_size/(1024*1024));
	system.draw.boxfill(buf_win, INT_MONITOR_LONG, mix_color(0x0000ff00, 0x7fff0000), 0,0,INT_MONITOR_LONG,16);	
	system.draw.putfonts(buf_win, INT_MONITOR_LONG, 0,0,0xffffff,s);
	system.draw.sheet.refresh(winfo1->center, 0, 0, INT_MONITOR_LONG, 16);

	free_mem_size = system.io.memory.freesize();

	sprintf(s,"free   %d Byte(%d KB,%d MB)",free_mem_size,free_mem_size/1024, free_mem_size/(1024*1024));
	system.draw.boxfill(buf_win, INT_MONITOR_LONG, 0x000000, 0,16,INT_MONITOR_LONG,32);	
	system.draw.putfonts(buf_win, INT_MONITOR_LONG, 0,16,0xffffff,s);
	system.draw.sheet.refresh(winfo1->center, 0, 16, INT_MONITOR_LONG, 32);

	system.draw.boxfill(buf_win, INT_MONITOR_LONG, mix_color(0x0000ff00, 0x7fff0000), 0,32,INT_MONITOR_LONG,48);	
	system.draw.putfonts(buf_win, INT_MONITOR_LONG, 0,32,0xffffff,strncpy(s, (char *)(diskimg + 0x002600 + 0x20 * 0),23));
	system.draw.sheet.refresh(winfo1->center, 0, 32, INT_MONITOR_LONG, 48);

	for (;;){
	system.io.cli();
	if(system.data.fifo.status(&sysfifo) == 0) {
		system.io.stihlt();
	} else {
		i = system.data.fifo.get(&sysfifo);
	}
	if( 256 <= i && i <=511) {
		i -= SYSFIFO_KEYB;
		system.io.sti();
		system.draw.boxfill(buf_win, INT_MONITOR_LONG, 0x000000, 0,48,INT_MONITOR_LONG, 64);	
		sprintf(s,"INT 21(IRQ-1) : PS/2 ·°ÎÞ°ÄÞ%02X",i);
		system.draw.putfonts(buf_win, INT_MONITOR_LONG, 0,48,0xffffff,s);
		system.io.serial.send(s);
		system.draw.sheet.refresh(winfo1->center, 0,48,INT_MONITOR_LONG , 64);			
		switch(i){
			case 0x13:
				reboot();
				break;
		}			
		system.draw.boxfill(buf_win, INT_MONITOR_LONG, 0x000000, 0,112,INT_MONITOR_LONG, 128);	
		sprintf(s,"À²Ï° = %u",timerctl.count);
		system.draw.putfonts(buf_win, INT_MONITOR_LONG, 0,112,0xffffff,s);
		system.draw.sheet.refresh(winfo1->center, 0,112,INT_MONITOR_LONG , 128);						
	} else if(512 <= i && i <= 767) {
			i -= SYSFIFO_MOUSE;
			system.io.sti();
			if (system.io.mouse.decode(i) == 1) {
			system.draw.boxfill(buf_win, INT_MONITOR_LONG, 0x000000, 0,64,INT_MONITOR_LONG , 80);	
			sprintf(s,"INT 2C(IRQ-12) : PS/2 Ï³½(%02X,%02X,%02X,%02X)",mdec.buf[0], mdec.buf[1], mdec.buf[2], mdec.whinfo);
			system.draw.putfonts(buf_win, INT_MONITOR_LONG, 0,64,0xffffff,s);
			system.draw.boxfill(buf_win, INT_MONITOR_LONG, 0x000000, 0,80,INT_MONITOR_LONG , 96);	
			sprintf(s,"[lcr %4d %4d]",mdec.x, mdec.y);
			if((mdec.btn & 0x01) != 0) s[1] = 'L';
			if((mdec.btn & 0x02) != 0) s[3] = 'R';
			if((mdec.btn & 0x04) != 0) s[2] = 'C';
			system.draw.putfonts(buf_win, INT_MONITOR_LONG, 0,80,0xffffff,s);
			mx += mdec.x;
			my += mdec.y;
			if(mx < 0) mx = 0;
			if(my < 0) my = 0;
			if(mx > scrnx - 1) mx = scrnx - 1;
			if(my > scrny - 1) my = scrny - 1;
			
			system.draw.sheet.slide(sht_mouse, mx,my);
			system.draw.boxfill(buf_win, INT_MONITOR_LONG, 0x000000, 0,96,INT_MONITOR_LONG,112);	
			if(mdec.scrool == 0xffffffff) scrool++;
			if(mdec.scrool == 0x00000001) scrool--;
			sprintf(s,"(%4d,%4d)  %d,%X",mx,my,scrool,system.info.vesa.PhysBasePtr,mdec.scrool);
			system.draw.putfonts(buf_win, INT_MONITOR_LONG, 0,96,0xffffff,s);	
			system.draw.sheet.refresh(winfo1->center, 0,64,INT_MONITOR_LONG , 112);
			}
		}
	}
}

void reboot(void)
{
	system.io.out8(PORT_KEYCMD, 0xfe);
	for(;;) {
		system.io.hlt();
	}
}

