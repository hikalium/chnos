#include "core.h"


void init_system(struct SYSTEM *system)
{
	struct VESAINFO *vesa = (struct VESAINFO *) ADR_VESAINFO;
	struct BOOTINFO *boot = (struct BOOTINFO *) ADR_BOOTINFO;

	system->io.clts			= clts;
	system->io.fnsave		= fnsave;
	system->io.frstore		= frstor;
	system->io.hlt			= io_hlt;
	system->io.cli			= io_cli;
	system->io.sti			= io_sti;
	system->io.stihlt		= io_stihlt;
	system->io.in8			= io_in8;
	system->io.in16			= io_in16;
	system->io.in32			= io_in32;
	system->io.out8			= io_out8;
	system->io.out16		= io_out16;
	system->io.out32		= io_out32;
	system->io.farcall		= farcall;
	system->io.farjmp		= farjmp;
	system->io.readrtc		= readrtc;

	system->io.pic.init		= init_pic;

	system->io.eflags.load		= io_load_eflags;
	system->io.eflags.store		= io_store_eflags;

	system->io.gdtr.load		= load_gdtr;	

	system->io.idtr.load		= load_idtr;	

	system->io.cr0.load		= load_cr0;
	system->io.cr0.store		= store_cr0;

	system->io.tr.load		= load_tr;

	system->io.keyboard.wait	= wait_KBC_sendready;
	system->io.keyboard.init	= init_keyboard;
	system->io.keyboard.inthandler	= inthandler21;

	system->io.timer.init		= init_pit;
	system->io.timer.inthandler	= inthandler20;

	system->io.mouse.inthandler	= inthandler2c;
	system->io.mouse.init		= init_mouse;
	system->io.mouse.decode		= decode_mouse;

	system->io.interrupt.init	= init_gdtidt;
	system->io.interrupt.set_segment	= set_segmdesc;	
	system->io.interrupt.set_gate	= set_gatedesc;
	system->io.interrupt.handler.int27	= inthandler27;	

	system->io.memory.init		= memman_init;
	system->io.memory.test		= memtest;
	system->io.memory.freesize	= memman_free_total;
	system->io.memory.allocb	= memman_alloc;
	system->io.memory.freeb		= memman_free;
	system->io.memory.alloc		= memman_alloc_4k;
	system->io.memory.free		= memman_free_4k;

	system->io.beep.on		= pit_beep_on;
	system->io.beep.off		= pit_beep_off;

	system->io.serial.init		= init_serial;
	system->io.serial.send		= send_serial;

	system->io.fdc.motor_on		= fdc_motor_on;
	system->io.fdc.motor_off	= fdc_motor_off;

	system->draw.circle		= circle_i;
	system->draw.int2short		= rgb_int2short;
	system->draw.int2char		= rgb_int2char;
	system->draw.init_scrn		= init_scrn_i;
	system->draw.boxfill		= boxfill_i;
	system->draw.col_pat_256safe	= col_pat_256safe;
	system->draw.putfonts		= putfonts_asc_i;
	system->draw.point		= point_i;

	system->draw.bits8.init		= init_palette;
	system->draw.bits8.set		= set_palette;
	system->draw.bits8.boxfill	= boxfill8;
	system->draw.bits8.init_scrn	= init_scrn8;
	system->draw.bits8.putfont	= putfont8;
	system->draw.bits8.putfonts	= putfonts8_asc;
	system->draw.bits8.mouse_cursor	= init_mouse_cursor8;
	system->draw.bits8.putblock	= putblock8_8;

	system->draw.bits16.boxfill	= boxfill16;
	system->draw.bits16.init_scrn	= init_scrn16;
	system->draw.bits16.putfont	= putfont16;
	system->draw.bits16.putfonts	= putfonts16_asc;
	system->draw.bits16.mouse_cursor	= init_mouse_cursor16;
	system->draw.bits16.putblock	= putblock16_16;

	system->draw.bits32.boxfill	= boxfill32;
	system->draw.bits32.init_scrn	= init_scrn32;
	system->draw.bits32.putfont	= putfont32;
	system->draw.bits32.putfonts	= putfonts32_asc;
	system->draw.bits32.mouse_cursor	= init_mouse_cursor32;
	system->draw.bits32.putblock	= putblock32_32;
 
	system->draw.window.init	= init_windows;
	system->draw.window.alloc	= window_alloc;
	system->draw.window.make	= make_window32;
	system->draw.window.slide	= slide_window;
 
	system->draw.sheet.init		= init_sheets;
	system->draw.sheet.alloc	= sheet_alloc;
	system->draw.sheet.set		= sheet_setbuf;
	system->draw.sheet.updown	= sheet_updown;
	system->draw.sheet.refresh	= sheet_refresh;
	system->draw.sheet.slide	= sheet_slide;
	system->draw.sheet.free		= sheet_free;
	system->draw.sheet.refsub	= sheet_refreshsub;
	system->draw.sheet.refmap	= sheet_refreshmap;
 
	system->data.fifo.init		= fifo32_init;
	system->data.fifo.put		= fifo32_put;
	system->data.fifo.get		= fifo32_get;
	system->data.fifo.status	= fifo32_status;

	system->info.vesa = *vesa;
	system->info.boot = *boot;
	


	return;
}
