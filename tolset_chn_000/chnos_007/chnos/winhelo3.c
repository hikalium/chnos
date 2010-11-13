
void api_boxfilwin(unsigned int win, int x0, int y0, int x1, int y1, unsigned int c);
void api_putstrwin(unsigned int win, int x, int y, unsigned int c, int len, unsigned char *s);
unsigned int api_openwin(unsigned int *buf, int xsize, int ysize, int col_inv, char *title);
void api_initmalloc(void);
void *api_malloc(unsigned int size);
void api_end(void);

void CHNMain(void)
{
	unsigned int winID;
	unsigned int *win_buf;

	api_initmalloc();
	win_buf = api_malloc(150 * 50 * 4);
	winID = api_openwin(win_buf, 150, 50, 0, "WinHelo3");
	api_boxfilwin(winID, 8, 36, 141, 43, 0xFFFF00);
	api_putstrwin(winID, 30, 30, 0x000000, 12, "hello,world");
	api_end();
}
