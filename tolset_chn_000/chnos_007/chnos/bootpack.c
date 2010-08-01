#include "core.h"

struct SYSTEM system;

void CHNMain(void)
{
	init_system();


	for(;;){
		system.io.hlt();
	}
}

