#include "core.h"


void init_system(struct SYSTEM *system)
{
	system->window.init = init_windows;
	system->memory.init = memman_init;
	return;
}