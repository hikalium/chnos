#include <apilib.h>

void CHNMain(void)
{
	char a[100];
	a[10] = 'A';
	api_putchar(a[10]);
	a[10] = 'B';
	api_putchar(a[102]);
	a[10] = 'C';
	api_putchar(a[123]);
	api_end();
}
