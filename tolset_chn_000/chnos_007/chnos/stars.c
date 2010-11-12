
void api_boxfilwin(unsigned int win, int x0, int y0, int x1, int y1, unsigned int c);
void api_putstrwin(unsigned int win, int x, int y, unsigned int c, int len, unsigned char *s);
unsigned int api_openwin(unsigned int *buf, int xsize, int ysize, int col_inv, char *title);
void api_initmalloc(void);
void *api_malloc(unsigned int size);
void api_end(void);
void api_point(unsigned int win, int x, int y, int c);

int rand(void);

void CHNMain(void)
{
	unsigned int *buf;
	unsigned int winID;
	int i, x, y;

	api_initmalloc();
	buf = api_malloc(150*50*4);
	winID = api_openwin(buf, 150, 100, -1, "stars");
	api_boxfilwin(winID, 6, 26, 143, 93, 0x000000);
	for(i = 0; i < 50; i++){
		x = (rand() % 137) + 6;
		y = (rand() % 67) + 26;
		api_point(winID, x, y, 0xFFFF00);
	}
	api_end();
}

