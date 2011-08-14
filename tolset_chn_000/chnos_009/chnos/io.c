
#include "core.h"

uchar IO_Read_CMOS(uchar addr)
{
	IO_Out8(0x70, addr);
	return IO_In8(0x71);
}

void IO_Beep(uint fq, uint microsec)
{
	uint timebase;

	PIT_Beep_Off();
	PIT_Beep_Set(fq);
	PIT_Beep_On();
	timebase = Timer_Get_Tick();	//1sec = 100
	IO_Wait(microsec);
	return;
}

void IO_Wait(uint microsec)
{
	uint timebase;

	timebase = Timer_Get_Tick();	//1sec = 100
	for(;;){
		if(microsec < (Timer_Get_Tick() - timebase) * 10000){
			break;
		}
	}
	return;
}
