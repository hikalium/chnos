#include <apilib.h>

uchar win_buf[150 * 50];

void CHNMain(void)
{
	uint winID;
	
	winID = api_openwin(win_buf, 150, 50, -1, "WinHello2");
	api_boxfilwin(winID, 8, 36, 141, 43, yellow);
	api_putstrwin(winID, 30, 30, black, 12, "hello,world");
	for(;;){
		if(api_getkey(true) == 0x0a) break;
	}
	api_end();
}
