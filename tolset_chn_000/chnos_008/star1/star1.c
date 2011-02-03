#include <apilib.h>

void CHNMain(void)
{
	uchar *buf;
	uint winID;

	api_initmalloc();
	buf = api_malloc(150 * 100);
	winID = api_openwin(buf, 150, 100, -1, "star1");
	api_boxfilwin(winID, 6, 26, 143, 93, black);
	api_point(winID, 75, 59, yellow);
	for(;;){
		if(api_getkey(true) == 0x0a) break;
	}
	api_end();
}

