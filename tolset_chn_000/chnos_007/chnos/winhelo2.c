
void api_boxfilwin(unsigned int win, int x0, int y0, int x1, int y1, unsigned int c);
void api_putstrwin(unsigned int win, int x, int y, unsigned int c, int len, unsigned char *s);
unsigned int api_openwin(unsigned int *buf, int xsize, int ysize, int col_inv, char *title);
void api_end(void);

unsigned int win_buf[150 * 50];

void CHNMain(void)
{
	unsigned int winID;
	
	winID = api_openwin(win_buf, 150, 50, 0, "WinHello2");
	api_boxfilwin(winID, 0, 0, 10, 10, 0xFFFF00);
	api_putstrwin(winID, 0, 0, 0x000000, 0, "hello,world");
	api_end();
}
