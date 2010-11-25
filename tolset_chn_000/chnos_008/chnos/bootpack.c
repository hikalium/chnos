
#include "core.h"

struct SYSTEM *sys_main_str_buf;

void CHNMain(void)
{
	init_system();

	putfonts_asc_i(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 0, 0xFFFFFF, "Hello, World");
	boxfill_i(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0x00ff00, 0, 0, 10, 10);

	for(;;){
		io_hlt();
	}
}
