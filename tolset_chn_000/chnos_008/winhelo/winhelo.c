#include<apilib.h>

uchar win_buf[150 * 50];

void CHNMain(void)
{
	uint winID;
	
	winID = api_openwin(win_buf, 150, 50, 0, "WinHello");
	api_end();
}
