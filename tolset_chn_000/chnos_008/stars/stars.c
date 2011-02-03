#include <apilib.h>

void CHNMain(void)
{
	uchar *buf;
	uint winID;
	int i, x, y;

	api_initmalloc();
	buf = api_malloc(150*100);
	winID = api_openwin(buf, 150, 100, -1, "stars");
	api_boxfilwin(winID + 1, 6, 26, 143, 93, black);
	for(i = 0; i < 50; i++){
		x = (rand() % 137) + 6;
		y = (rand() % 67) + 26;
		api_point(winID + 1, x, y, yellow);
	}
	api_refreshwin(winID, 6, 26, 144, 94);
	for(;;){
		if(api_getkey(true) == 0x0a) break;
	}
	api_end();
}

