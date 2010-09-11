
#include "core.h"

void init_system(void)
{
	struct VESAINFO *vesa = (struct VESAINFO *) ADR_VESAINFO;
	struct BOOTINFO *boot = (struct BOOTINFO *) ADR_BOOTINFO;

	system.io.clts					= clts;
	system.io.fnsave				= fnsave;
	system.io.frstore				= frstore;
	system.io.hlt					= io_hlt;
	system.io.cli					= io_cli;
	system.io.sti					= io_sti;
	system.io.stihlt				= io_stihlt;
	system.io.in8					= io_in8;
	system.io.in16					= io_in16;
	system.io.in32					= io_in32;
	system.io.out8					= io_out8;
	system.io.out16					= io_out16;
	system.io.out32					= io_out32;
	system.io.farjmp				= farjmp;
	system.io.farcall				= farcall;

	system.io.mem.init				= sys_memman_init;
	system.io.mem.free_total			= sys_memman_free_total;
	system.io.mem.alloc				= sys_memman_alloc;
	system.io.mem.free				= sys_memman_free;
	system.io.mem.test				= memtest;
	system.io.mem.test_sub				= memtest_sub;

	system.io.mem.org.init				= memman_init;
	system.io.mem.org.free_total			= memman_free_total;
	system.io.mem.org.alloc				= memman_alloc;
	system.io.mem.org.free				= memman_free;
	system.io.mem.org.alloc_4k			= memman_alloc_4k;
	system.io.mem.org.free_4k			= memman_free_4k;

	system.io.tr.load				= load_tr;

	system.io.cr0.load				= load_cr0;
	system.io.cr0.store				= store_cr0;

	system.io.gdt.load				= load_gdtr;

	system.io.idt.load				= load_idtr;

	system.io.eflags.load				= io_load_eflags;
	system.io.eflags.store				= io_store_eflags;

	system.io.beep.on				= pit_beep_on;
	system.io.beep.off				= pit_beep_off;

	system.draw.putfonts_sht			= putfonts_asc_sht_i;
	system.draw.init_screen				= init_screen_i;
	system.draw.circle				= circle_i;
	system.draw.point				= point_i;
	system.draw.boxfill				= boxfill_i;
	system.draw.putfonts				= putfonts_asc_i;
	system.draw.color.mix				= mix_color;
	system.draw.color.int2char			= rgb_int2char;
	system.draw.color.int2short			= rgb_int2short;
	system.draw.color.pattern			= col_pat;

	system.draw.sht.init				= init_sheets;
	system.draw.sht.alloc				= sheet_alloc;
	system.draw.sht.set				= sheet_setbuf;
	system.draw.sht.updown				= sheet_updown;
	system.draw.sht.refresh				= sheet_refresh;
	system.draw.sht.slide				= sheet_slide;
	system.draw.sht.free				= sheet_free;

	system.data.fifo.init				= fifo32_init;
	system.data.fifo.put				= fifo32_put;
	system.data.fifo.get				= fifo32_get;
	system.data.fifo.status				= fifo32_status;

	system.info.vesa				= *vesa;
	system.info.boot				= *boot;					/* 0x0ff0-0x0fff*/

	system.app.start				= start_app;
	system.app.end					= asm_end_app;

	system.sys.vram					= system.info.vesa.PhysBasePtr;
	system.sys.xsize				= system.info.boot.scrnx;
	system.sys.ysize				= system.info.boot.scrny;
	system.sys.bpp					= system.info.vesa.BitsPerPixel;
	system.sys.gdt					= (struct SEGMENT_DESCRIPTOR *)0x00270000;	/*0x270000-0x27ffff*/
	system.sys.idt					= (struct GATE_DESCRIPTOR *)0x0026f800;		/*0x26f800-0x26ffff*/

// system init
	init_gdtidt();
	system.sys.memtotal				= system.io.mem.test(0x00400000, 0xbfffffff);
	system.io.mem.init();
	system.io.mem.free((void *)0x00400000, system.sys.memtotal - 0x00400000);

	system.draw.sht.init(system.sys.vram, system.sys.xsize, system.sys.ysize, system.sys.bpp);

	system.sys.sht.desktop				= system.draw.sht.alloc();
	system.sys.sht.mouse				= system.draw.sht.alloc();
	system.sys.sht.taskbar				= system.draw.sht.alloc();

	system.sys.sht.desktop_buf			= system.io.mem.alloc(system.sys.xsize * system.sys.ysize * (system.sys.bpp / 8));
	system.sys.sht.taskbar_buf			= system.io.mem.alloc(system.sys.xsize * TASKBAR_HEIGHT * (system.sys.bpp / 8));

	system.draw.sht.set(system.sys.sht.desktop, system.sys.sht.desktop_buf, system.sys.xsize, system.sys.ysize, INV_COL32);
	system.draw.sht.set(system.sys.sht.mouse, *system.sys.sht.mouse_buf, 24, 24, INV_COL32);
	system.draw.sht.set(system.sys.sht.taskbar, system.sys.sht.taskbar_buf, system.sys.xsize, system.sys.ysize, INV_COL32);

	system.draw.sht.slide(system.sys.sht.desktop, 0, 0);
	system.draw.sht.updown(system.sys.sht.desktop, 1);
	system.draw.sht.slide(system.sys.sht.taskbar, 0, system.sys.ysize - TASKBAR_HEIGHT);
	system.draw.sht.updown(system.sys.sht.taskbar, 2);
	system.draw.sht.slide(system.sys.sht.mouse, system.sys.xsize / 2, system.sys.ysize / 2);
	system.draw.sht.updown(system.sys.sht.mouse, 3);

	system.data.fifo.init(&system.sys.fifo, SYS_FIFOSIZE, system.sys.fifo_buf);

	init_pic();
	init_keyboard(256);	
	init_mouse(512);
	init_serial();
	init_pit();

	fdc_motor_off(4);

	init_windows();
	
	return;
}
