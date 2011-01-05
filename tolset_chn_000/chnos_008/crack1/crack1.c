#include <apilib.h>

void CHNMain(void)
{
	*((char *)0x00102600) = 0;
	api_end();
}
