#include <apilib.h>

void CHNMain(void)
{
	uchar *buf;
	uint winID;
	int i, x, y;

	api_initmalloc();
	buf = api_malloc(150*100);
	winID = api_openwin(buf, 150, 100, -1, "stars");
	api_boxfilwin(winID, 6, 26, 143, 93, black);
	for(i = 0; i < 50; i++){
		x = (rand() % 137) + 6;
		y = (rand() % 67) + 26;
		api_point(winID, x, y, yellow);
	}
	api_end();
}

