
unsigned int api_openwin(unsigned int *buf, int xsize, int ysize, int col_inv, char *title);
void api_initmalloc(void);
void *api_malloc(unsigned int size);
void api_end(void);
void api_linewin(unsigned int win, int x0, int y0, int x1, int y1, int c);

static int rgb_int2char_list [16] = {
	0x000000,
	0xff0000,
	0x00ff00,
	0xffff00,
	0x0000ff,
	0xff00ff,
	0x00ffff,
	0xffffff,
	0xc6c6c6,
	0x840000,
	0x008400,
	0x848400,
	0x000084,
	0x840084,
	0x008484,
	0x848484
};

void CHNMain(void)
{
	unsigned int *buf;
	unsigned int winID;
	int i;

	api_initmalloc();
	buf = api_malloc(160*100*4);
	winID = api_openwin(buf, 160, 100, -1, "lines");
	for(i = 0; i < 8; i++){
		api_linewin(winID, 8, 26, 77, i * 9 + 26, rgb_int2char_list[i]);
		api_linewin(winID, 88, 26, i * 9 + 88, 89, rgb_int2char_list[i]);
	}
	api_end();
}

