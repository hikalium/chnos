
#include <stdio.h>

typedef struct _UUID {
	unsigned short data[8];
} UUID;

void api_putstr(unsigned char *s);
void api_end(void);
int rand(void);

#define srand(seed)			(void) (rand_seed = (seed))
extern unsigned int rand_seed;

void CHNMain(void)
{
	unsigned char s[128];
	UUID test[100];
	unsigned int i, j;

	srand(256);
	for(j = 0; j < 100; j++){
		for(i = 0; i < 8; i++){
			test[j].data[i] = rand();
			srand(test[j].data[i]);
		}
		sprintf(s, "{%04X%04X-%04X-%04X-%04X-%04X%04X%04X}\n", test[j].data[0], test[j].data[1], test[j].data[2], test[j].data[3], test[j].data[4], test[j].data[5], test[j].data[6], test[j].data[7]);
		api_putstr(s);
	}

	api_end();
}
