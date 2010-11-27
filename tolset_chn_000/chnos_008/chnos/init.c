
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

	init_gdtidt();
	init_paging();
	sys_memman_init();
	sys_memman_free((void *)0x00400000, system.io.mem.total - sizeof(struct SYSTEM) - 0x00400000);
	init_serial();

	return;
}
