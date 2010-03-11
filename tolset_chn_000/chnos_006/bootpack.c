#include "core.h"

void CHNMain(void)
{	
	unsigned char s[24] , mdatabuf[3], m_phase = 0;
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct VESAINFO *vinfo = (struct VESAINFO *) ADR_VESAINFO;
	struct FIFO32 sysfifo;
	int fifobuf[256], i,time_tick;
	init_gdtidt();
	init_pic();
	init_pit(&time_tick);
	i = 0;

	io_sti();

	init_scrn_i(vinfo->PhysBasePtr, binfo->scrnx, binfo->scrny, vinfo->BitsPerPixel);
	fifo32_init(&sysfifo, 256, fifobuf);
	init_keyboard(&sysfifo, SYSFIFO_KEYB);
	init_mouse(&sysfifo, SYSFIFO_MOUSE);
	pit_beep_off();

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
			boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,240,300 , 256);	
			sprintf(s,"INT 21(IRQ-1) : PS/2 ���ް��%02X",i);
			putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,240,0xffffff,s);
			
	} else if(512 <= i && i <= 767) {
			i -= SYSFIFO_MOUSE;
			io_sti();
			switch (m_phase) {
				case 0:
					if(i == 0xfa) { m_phase = 1; }
					break;
				case 1:
					mdatabuf[0] = i;
					m_phase = 2;
					break;
				case 2:
					mdatabuf[1] = i;
					m_phase = 3;
					break;
				case 3:
					mdatabuf[2] = i;
					m_phase = 1;
					boxfill_i(vinfo->PhysBasePtr, binfo->scrnx, 0x000000, 0,256,300 , 272);	
					sprintf(s,"INT 2C(IRQ-12) : PS/2 ϳ� %02X %02X %02X",mdatabuf[0], mdatabuf[1], mdatabuf[2]);
					putfonts_asc_i(vinfo->PhysBasePtr, binfo->scrnx, 0,256,0xffffff,s);	
					break;
					
				}

	
		}
	}
}



