#include <apilib.h>

void CHNMain(void)
{
	uchar *buf;
	uint winID;
	int i;

	api_initmalloc();
	buf = api_malloc(160 * 100);
	winID = api_openwin(buf, 160, 100, -1, "lines");
	for(i = 0; i < 8; i++){
		api_linewin(winID + 1, 8, 26, 77, i * 9 + 26, i);
		api_linewin(winID + 1, 88, 26, i * 9 + 88, 89, i);
	}
	api_refreshwin(winID, 6, 26, 154, 90);
	api_end();
}

