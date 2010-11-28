
#include "core.h"

void init_system(void)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;
	uint i;

	i = memtest(0x00400000, 0xbfffffff);
	sys_main_str_buf = (struct SYSTEM *)(i - sizeof(struct SYSTEM));

	system.io.mem.paging.dir			= (uint *)0x00400000;
	(uint)system.io.mem.paging.table		= 0x00400000 + (1024 * 4);

	system.io.mem.segment.gdt			= (IO_SegmentDescriptor *)ADR_GDT;
	system.io.interrupt.idt				= (IO_GateDescriptor *)ADR_IDT;

	system.io.mem.total				= i;
	system.data.info.vesa				= *vesa;
	system.data.info.boot				= *boot;

	init_serial();
	init_gdtidt();
	init_paging();
	sys_memman_init();
	sys_memman_free((void *)(0x00400000 + (1024 * 4) + (1024 * 1024 * 4)), system.io.mem.total - sizeof(struct SYSTEM) - 0x00400000) - (1024 * 4) - (1024 * 1024 * 4);
	init_sheets(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, system.data.info.boot.scrny, system.data.info.vesa.BitsPerPixel);


	system.draw.sht.core				= sheet_alloc();
	system.draw.sht.desktop				= sheet_alloc();
	system.draw.sht.taskbar				= sheet_alloc();
	system.draw.sht.mouse				= sheet_alloc();

	system.draw.sht.core_buf			= sys_memman_alloc(system.data.info.boot.scrnx * system.data.info.boot.scrny * (system.data.info.vesa.BitsPerPixel >> 3));
	system.draw.sht.desktop_buf			= sys_memman_alloc(system.data.info.boot.scrnx * system.data.info.boot.scrny * (system.data.info.vesa.BitsPerPixel >> 3));
	system.draw.sht.taskbar_buf			= sys_memman_alloc(system.data.info.boot.scrnx * TASKBAR_HEIGHT * (system.data.info.vesa.BitsPerPixel >> 3));
	system.draw.sht.mouse_buf			= sys_memman_alloc(24 * 24 * (system.data.info.vesa.BitsPerPixel >> 3));


	sheet_setbuf(system.draw.sht.desktop, system.draw.sht.desktop_buf, system.data.info.boot.scrnx, system.data.info.boot.scrny, INV_COL32);
	sheet_setbuf(system.draw.sht.mouse, system.draw.sht.mouse_buf, 24, 24, INV_COL32);
	sheet_setbuf(system.draw.sht.taskbar, system.draw.sht.taskbar_buf, system.data.info.boot.scrnx, TASKBAR_HEIGHT, INV_COL32);
	sheet_setbuf(system.draw.sht.core, system.draw.sht.core_buf, system.data.info.boot.scrnx, system.data.info.boot.scrny, INV_COL32);

	sheet_slide(system.draw.sht.desktop, 0, 0);
	sheet_updown(system.draw.sht.desktop, -1);
	sheet_slide(system.draw.sht.taskbar, 0, system.data.info.boot.scrny - TASKBAR_HEIGHT);
	sheet_updown(system.draw.sht.taskbar, -1);
	sheet_slide(system.draw.sht.mouse, system.data.info.boot.scrnx >> 1, system.data.info.boot.scrny >> 1);
	sheet_updown(system.draw.sht.mouse, -1);
	sheet_slide(system.draw.sht.core, 0, 0);
	sheet_updown(system.draw.sht.core, -1);

	return;
}
