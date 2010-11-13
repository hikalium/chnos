
void api_boxfilwin(unsigned int win, int x0, int y0, int x1, int y1, unsigned int c);
unsigned int api_openwin(unsigned int *buf, int xsize, int ysize, int col_inv, char *title);
void api_initmalloc(void);
void *api_malloc(unsigned int size);
void api_end(void);
void api_point(unsigned int win, int x, int y, int c);

void CHNMain(void)
{
	unsigned int *buf;
	unsigned int winID;

	api_initmalloc();
	buf = api_malloc(150*100*4);
	winID = api_openwin(buf, 150, 100, -1, "star1");
	api_boxfilwin(winID, 6, 26, 143, 93, 0x000000);
	api_point(winID, 75, 59, 0xFFFF00);
	api_end();
}

