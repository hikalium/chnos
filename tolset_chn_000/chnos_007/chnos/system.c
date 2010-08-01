
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

	system.io.mem.test_sub				= memtest_sub;

	system.io.tr.load				= load_tr;

	system.io.cr0.load				= load_cr0;
	system.io.cr0.store				= store_cr0;

	system.io.gdt.load				= load_gdtr;

	system.io.idt.load				= load_idtr;

	system.io.eflags.load				= io_load_eflags;
	system.io.eflags.store				= io_store_eflags;

	system.io.beep.on				= pit_beep_on;
	system.io.beep.off				= pit_beep_off;

	system.info.vesa				= *vesa;
	system.info.boot				= *boot;

	system.app.start				= start_app;
	system.app.end					= asm_end_app;
	return;
}
