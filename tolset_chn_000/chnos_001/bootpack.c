#include "core.h"
#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))

void boxfill16(unsigned short *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1);

void CHNMain(void)
{	
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	struct VESAINFO *vinfo = (struct VESAINFO *) 0x0e00;
	boxfill16(vinfo->PhysBasePtr,binfo->scrnx,RGB16(31,0,31),0,0,binfo->scrnx/2,binfo->scrny/2);
	for(;;) {
	io_hlt();
	}


}

void boxfill16(unsigned short *vram, int xsize, unsigned short c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0;y <= y1;y++) {
		for (x = x0; x <= x1; x++)
		vram[y * xsize + x] = c;
	}
	return;
}
