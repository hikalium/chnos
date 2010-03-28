#include "core.h"


void init_system(struct SYSTEM *system)
{
	system->draw.window.init = init_windows;
	system->io.memory.init = memman_init;
	return;
}