
unsigned int api_openwin(unsigned int *buf, int xsize, int ysize, int col_inv, char *title);
void api_end(void);

unsigned int win_buf[150 * 50];

void CHNMain(void)
{
	unsigned int winID;
	
	winID = api_openwin(win_buf, 150, 50, 0, "WinHello");
	api_end();
}
