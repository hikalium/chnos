#include <apilib.h>

void CHNMain(void)
{
	uint winID;
	uchar *win_buf;

	api_initmalloc();
	win_buf = api_malloc(150 * 50);
	winID = api_openwin(win_buf, 150, 50, 0, "WinHelo3");
	api_boxfilwin(winID, 8, 36, 141, 43, yellow);
	api_putstrwin(winID, 30, 30, black, 12, "hello,world");
	api_end();
}
