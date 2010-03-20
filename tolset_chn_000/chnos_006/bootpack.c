#include "core.h"

void CHNMain(void)
{	
	unsigned char s[24];
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	struct FIFO32 sysfifo;
	struct MOUSE_DECODE mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET32 *sht_back, *sht_mouse;
	int fifobuf[256], i = 0,time_tick,mx = binfo->scrnx / 2, my = binfo->scrny / 2;
	unsigned int all_mem_size = memtest(0x00400000, 0xbffffffff);
	unsigned int free_mem_size = 0;
	unsigned int *buf_back, buf_mouse[576];

	init_gdtidt();
	init_pic();
	io_sti();

	memman_init(memman);
//	memman_free(0x00001000,0x0009e000);
	memman_free(0x00400000,all_mem_size - 0x00400000);

	fifo32_init(&sysfifo, 256, fifobuf);
	init_keyboard(&sysfifo, SYSFIFO_KEYB);
	init_mouse(&sysfifo, SYSFIFO_MOUSE, &mdec);
	init_pit(&time_tick);
	init_sheets(vinfo->PhysBasePtr,binfo->scrnx,binfo->scrny);
	pit_beep_off();


	sht_back = sheet_alloc();
	sht_mouse = sheet_alloc();
	buf_back = (unsigned int *) memman_alloc_4k(vinfo->BankSize * 1024);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, VOID_INV_COL32);
	sheet_setbuf(sht_mouse, buf_mouse, 24, 24, INV_COL32);
	boxfill32(buf_back, binfo->scrnx, 0x000000, 0,0,binfo->scrnx, binfo->scrny);
	init_scrn_i(buf_back, binfo->scrnx, binfo->scrny, vinfo->BitsPerPixel,buf_mouse);
	sheet_slide(sht_back, 0,0);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back, 0);
	sheet_updown(sht_mouse, 1);


	circle_i(buf_back, binfo->scrnx/2, binfo->scrny/2, 0xff0000, binfo->scrnx, 100);

	sprintf(s,"memory %d Byte(%d KB,%d MB)",all_mem_size,all_mem_size/1024, all_mem_size/(1024*1024));
	boxfill_i(buf_back, binfo->scrnx, 0x000000, 0,304,INT_MONITOR_LONG,320);	
	putfonts_asc_i(buf_back, binfo->scrnx, 0,304,0xffffff,s);
	sheet_refresh(sht_back, 0, 0, binfo->scrnx, binfo->scrny);


	for (;;){
	io_cli();
	if(fifo32_status(&sysfifo) == 0) {
		
		free_mem_size = memman_free_total();
		sprintf(s,"free   %d Byte(%d KB,%d MB)",free_mem_size,free_mem_size/1024, free_mem_size/(1024*1024));
		boxfill_i(buf_back, binfo->scrnx, 0x000000, 0,320,INT_MONITOR_LONG,336);	
		putfonts_asc_i(buf_back, binfo->scrnx, 0,320,0xffffff,s);
		io_stihlt();
	} else {
		i = fifo32_get(&sysfifo);
	}
	if( 256 <= i && i <=511) {
			i -= SYSFIFO_KEYB;
			io_sti();
			boxfill_i(buf_back, binfo->scrnx, 0x000000, 0,240,INT_MONITOR_LONG , 256);	
			sprintf(s,"INT 21(IRQ-1) : PS/2 ���ް��%02X",i);
			putfonts_asc_i(buf_back, binfo->scrnx, 0,240,0xffffff,s);
			sheet_refresh(sht_back, 0,240,INT_MONITOR_LONG , 256);			

	} else if(512 <= i && i <= 767) {
			i -= SYSFIFO_MOUSE;
			io_sti();
			if (decode_mouse(i) == 1) {
				boxfill_i(buf_back, binfo->scrnx, 0x000000, 0,256,INT_MONITOR_LONG , 272);	
				sprintf(s,"INT 2C(IRQ-12) : PS/2 ϳ�(%02X,%02X,%02X)",mdec.buf[0], mdec.buf[1], mdec.buf[2]);
				putfonts_asc_i(buf_back, binfo->scrnx, 0,256,0xffffff,s);

				boxfill_i(buf_back, binfo->scrnx, 0x000000, 0,272,INT_MONITOR_LONG , 288);	
				sprintf(s,"[lcr %4d %4d]",mdec.x, mdec.y);
				if((mdec.btn & 0x01) != 0) s[1] = 'L';
				if((mdec.btn & 0x02) != 0) s[3] = 'R';
				if((mdec.btn & 0x04) != 0) s[2] = 'C';
				putfonts_asc_i(buf_back, binfo->scrnx, 0,272,0xffffff,s);
				mx += mdec.x;
				my += mdec.y;
				if(mx < 0) mx = 0;
				if(my < 0) my = 0;
				if(mx > binfo->scrnx - 24) mx = binfo->scrnx - 24;
				if(my > binfo->scrny - 24) my = binfo->scrny - 24;
				
				sheet_slide(sht_mouse, mx,my);
				boxfill_i(buf_back, binfo->scrnx, 0x000000, 0,288,INT_MONITOR_LONG,304);	
				sprintf(s,"(%4d,%4d)",mx,my);
				putfonts_asc_i(buf_back, binfo->scrnx, 0,288,0xffffff,s);	
				sheet_refresh(sht_back, 0, 256, INT_MONITOR_LONG, 336);			
			}
		}
	}
}



