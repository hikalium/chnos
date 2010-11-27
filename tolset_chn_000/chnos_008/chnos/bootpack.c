
#include "core.h"

struct SYSTEM *sys_main_str_buf;

void CHNMain(void)
{
	uchar s[128];

	init_system();

	col_pat(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, system.data.info.boot.scrny);
	draw_chnos_logo(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, system.data.info.boot.scrnx >> 4, system.data.info.boot.scrnx >> 1, (system.data.info.boot.scrny >> 1) - (system.data.info.boot.scrny / 10));

	sprintf(s, "Welcome to CHNOS Project Paging-Test-Version.");
	putfonts_asc_i(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 0, 0xFFFFFF, s);

	sprintf(s, "Physical-Memory:%dMB(%dKB) Free_Total:%dMB(%dKB)", system.io.mem.total >> 20, system.io.mem.total >> 10, sys_memman_free_total() >> 20, sys_memman_free_total() >> 10);
	putfonts_asc_i(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 16, 0xFFFFFF, s);

	for(;;){
		io_hlt();
	}
}
