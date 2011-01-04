
#include "core.h"

void init_system(void)
{
	DATA_VESAInfo *vesa 				= (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot 				= (DATA_BootInfo *) ADR_BOOTINFO;
	uint i;

	i 						= memtest(0x00400000, 0xbfffffff);
	sys_main_str_buf 				= (struct SYSTEM *)(i - sizeof(struct SYSTEM));

	system.io.mem.paging.dir			= (uint *)0x00400000;
	(uint)system.io.mem.paging.table		= 0x00400000 + (1024 * 4);

	system.io.mem.segment.gdt			= (IO_SegmentDescriptor *)ADR_GDT;
	system.io.interrupt.idt				= (IO_GateDescriptor *)ADR_IDT;

	system.io.mem.total				= i;
	system.data.info.vesa				= *vesa;
	system.data.info.boot				= *boot;

	system.ui.console.org_xsize			= (system.data.info.boot.scrnx >> 1) & 0xfffffff0;
	system.ui.console.org_ysize			= (system.data.info.boot.scrny >> 1) & 0xffffffe0;
	system.ui.console.org_xchars			= system.ui.console.org_xsize >> 3;
	system.ui.console.org_ychars			= system.ui.console.org_ysize >> 4;

	system.io.file.list				= (IO_FileInfo *)(ADR_DISKIMG + 0x00002600);

	init_gdtidt();
	init_pic();
	init_serial();
	init_paging();
	sys_memman_init();
	sys_memman_free((void *)(0x00400000 + (1024 * 4) + (1024 * 1024 * 4)), system.io.mem.total - sizeof(struct SYSTEM) - 0x00400000) - (1024 * 4) - (1024 * 1024 * 4);
	init_sheets(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, system.data.info.boot.scrny, system.data.info.vesa.BitsPerPixel);

	system.ui.draw.sht.core				= sheet_alloc();
	system.ui.draw.sht.desktop			= sheet_alloc();
	system.ui.draw.sht.taskbar			= sheet_alloc();
	system.ui.draw.sht.mouse			= sheet_alloc();

	system.ui.draw.sht.core_buf			= sys_memman_alloc(system.data.info.boot.scrnx * system.data.info.boot.scrny * (system.data.info.vesa.BitsPerPixel >> 3));
	system.ui.draw.sht.desktop_buf			= sys_memman_alloc(system.data.info.boot.scrnx * system.data.info.boot.scrny * (system.data.info.vesa.BitsPerPixel >> 3));
	system.ui.draw.sht.taskbar_buf			= sys_memman_alloc(system.data.info.boot.scrnx * TASKBAR_HEIGHT * (system.data.info.vesa.BitsPerPixel >> 3));
	system.ui.draw.sht.mouse_buf			= sys_memman_alloc(24 * 24 * (system.data.info.vesa.BitsPerPixel >> 3));

	sheet_setbuf(system.ui.draw.sht.desktop, system.ui.draw.sht.desktop_buf, system.data.info.boot.scrnx, system.data.info.boot.scrny, INV_COL32);
	sheet_setbuf(system.ui.draw.sht.mouse, system.ui.draw.sht.mouse_buf, 24, 24, INV_COL32);
	sheet_setbuf(system.ui.draw.sht.taskbar, system.ui.draw.sht.taskbar_buf, system.data.info.boot.scrnx, TASKBAR_HEIGHT, INV_COL32);
	sheet_setbuf(system.ui.draw.sht.core, system.ui.draw.sht.core_buf, system.data.info.boot.scrnx, system.data.info.boot.scrny, INV_COL32);

	sheet_slide(system.ui.draw.sht.desktop, 0, 0);
	sheet_updown(system.ui.draw.sht.desktop, -1);
	sheet_slide(system.ui.draw.sht.taskbar, 0, system.data.info.boot.scrny - TASKBAR_HEIGHT);
	sheet_updown(system.ui.draw.sht.taskbar, -1);
	sheet_slide(system.ui.draw.sht.mouse, system.data.info.boot.scrnx >> 1, system.data.info.boot.scrny >> 1);
	sheet_updown(system.ui.draw.sht.mouse, -1);

	sheet_slide(system.ui.draw.sht.core, 0, 0);
	boxfill_i(system.ui.draw.sht.core_buf, system.data.info.boot.scrnx, 0x000000, 0, 0, system.data.info.boot.scrnx, system.data.info.boot.scrny);
	draw_chnos_logo(system.ui.draw.sht.core_buf, system.data.info.boot.scrnx, system.data.info.boot.scrnx >> 4, system.data.info.boot.scrnx >> 1, (system.data.info.boot.scrny >> 1) - (system.data.info.boot.scrny / 10));
	sheet_updown(system.ui.draw.sht.core, 0);

	fifo32_init(&system.data.fifo.main, SYS_FIFOSIZE, system.data.fifo.main_buf, 0);
	fifo32_init(&system.data.fifo.keycmd, KEYCMD_FIFOSIZE, system.data.fifo.keycmd_buf, 0);
	fifo32_init(&system.data.fifo.keyctrl, KEYCTRL_FIFOSIZE, system.data.fifo.keyctrl_buf, 0);
	fifo32_init(&system.data.fifo.mousectrl, MOUSECTRL_FIFOSIZE, system.data.fifo.mousectrl_buf, 0);
	init_pit();
	task_init();

	system.ui.task.keyctrl 				= task_alloc();
	system.ui.task.keyctrl->tss.esp 		= (int)sys_memman_alloc(64 * 1024) + 64 * 1024;
	system.ui.task.keyctrl->tss.eip 		= (int)&KeyBoardControlTask;
	system.ui.task.keyctrl->tss.es 			= 1 * 8;
	system.ui.task.keyctrl->tss.cs 			= 2 * 8;
	system.ui.task.keyctrl->tss.ss 			= 1 * 8;
	system.ui.task.keyctrl->tss.ds 			= 1 * 8;
	system.ui.task.keyctrl->tss.fs 			= 1 * 8;
	system.ui.task.keyctrl->tss.gs 			= 1 * 8;

	system.ui.task.mousectrl 			= task_alloc();
	system.ui.task.mousectrl->tss.esp 		= (int)sys_memman_alloc(64 * 1024) + 64 * 1024;
	system.ui.task.mousectrl->tss.eip 		= (int)&MouseControlTask;
	system.ui.task.mousectrl->tss.es 		= 1 * 8;
	system.ui.task.mousectrl->tss.cs 		= 2 * 8;
	system.ui.task.mousectrl->tss.ss 		= 1 * 8;
	system.ui.task.mousectrl->tss.ds 		= 1 * 8;
	system.ui.task.mousectrl->tss.fs 		= 1 * 8;
	system.ui.task.mousectrl->tss.gs 		= 1 * 8;

	system.data.fifo.main.task 			= system.ui.task.main;
	system.data.fifo.keycmd.task 			= system.ui.task.keyctrl;
	system.data.fifo.keyctrl.task 			= system.ui.task.keyctrl;
	system.data.fifo.mousectrl.task 		= system.ui.task.mousectrl;

	task_run(system.ui.task.main, 1, 0);
	task_run(system.ui.task.keyctrl, 1, 1);
	task_run(system.ui.task.mousectrl, 1, 1);

	init_keyboard(0x100);
	init_mouse(0x100);
	init_windows();

	fdc_motor_off(4);

	system.io.file.fat 				= sys_memman_alloc(2 * 2880);
	decode_fat(system.io.file.fat, false);

	return;
}
