#include "core.h"

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_palette(void);

void CHNMain(void)
{	
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	for(;;) {
	io_hlt();
	}


}

