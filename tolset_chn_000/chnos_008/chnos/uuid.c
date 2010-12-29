
#include "core.h"

void gen_UUID(UUID *uuid)
{
	uint tsc[2];

	/*from date*/
	srand((uint)readcmos(0x32));
	uuid->data[0] = (uchar)rand();

	srand((uint)readcmos(0x09));
	uuid->data[1] = (uchar)rand();

	srand((uint)readcmos(0x08));
	uuid->data[2] = (uchar)rand();

	srand((uint)readcmos(0x07));
	uuid->data[3] = (uchar)rand();

	srand((uint)readcmos(0x04));
	uuid->data[4] = (uchar)rand();

	srand((uint)readcmos(0x02));
	uuid->data[5] = (uchar)rand();

	srand((uint)readcmos(0x00));
	uuid->data[6] = (uchar)rand();

	/*from tsc*/
	read_tsc(tsc);
	srand((uint)(tsc[0] & 0x000000ff));
	uuid->data[7] = (uchar)rand();

	read_tsc(tsc);
	srand((uint)(tsc[0] & 0x0000ff00) >> 8);
	uuid->data[8] = (uchar)rand();

	read_tsc(tsc);
	srand((uint)(tsc[0] & 0x00ff0000) >> 16);
	uuid->data[9] = (uchar)rand();

	read_tsc(tsc);
	srand((uint)(tsc[0] >> 24));
	uuid->data[10] = (uchar)rand();

	read_tsc(tsc);
	srand((uint)(tsc[1] & 0x000000ff));
	uuid->data[11] = (uchar)rand();

	read_tsc(tsc);
	srand((uint)(tsc[1] & 0x0000ff00) >> 8);
	uuid->data[12] = (uchar)rand();

	read_tsc(tsc);
	srand((uint)(tsc[1] & 0x00ff0000) >> 16);
	uuid->data[13] = (uchar)rand();

	read_tsc(tsc);
	srand((uint)(tsc[1] >> 24));
	uuid->data[14] = (uchar)rand();

	read_tsc(tsc);
	uuid->data[15] = (uchar)(tsc[1] & 0x000000ff);

	return;
}
