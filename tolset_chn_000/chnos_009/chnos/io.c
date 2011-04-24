
#include "core.h"

uint IO_Read_CMOS(uchar addr)
{
	IO_Out8(0x70, addr);
	return IO_In8(0x71);
}
