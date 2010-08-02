#include "core.h"

struct SYSTEM system;

void CHNMain(void)
{
	unsigned int mousecursor[24][24];

	init_system();

	system.draw.init_screen(system.info.vesa.PhysBasePtr, system.info.boot.scrnx, system.info.boot.scrny, system.info.vesa.BitsPerPixel, (unsigned int *)mousecursor);

	for(;;){
		system.io.hlt();
	}
}

