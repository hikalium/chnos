
#include "core.h"

void CHNMain(void)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;
	uchar s[128];
	uint fifobuf[128], keycmdbuf[64];
	DATA_FIFO fifo, keycmd;
	int keycmd_wait = 0;
	UI_InputBox inpbox;
	UI_KeyInfo kinfo;
	uint i;

	FIFO32_Initialise(&fifo, 128, fifobuf);
	FIFO32_Initialise(&keycmd, 128, keycmdbuf);

	IO_CLI();
	Initialise_SerialPort();
	Initialise_GlobalDescriptorTable();
	Initialise_InterruptDescriptorTable();
	Initialise_ProgrammableInterruptController();
	Initialise_ProgrammableIntervalTimer();
	Initialise_Keyboard(&fifo, &keycmd, DATA_BYTE, boot->leds, &keycmd_wait);
	Initialise_Graphic(vesa->BitsPerPixel);
	System_MemoryControl_Initialise();
	Initialise_Paging(vesa->PhysBasePtr, boot->scrnx, boot->scrny, vesa->BitsPerPixel);
	IO_STI();

	InputBox_Initialise(&inpbox, vesa->PhysBasePtr, boot->scrnx, 0, 0, boot->scrnx - 1, boot->scrny - 1, 1024, 0xFFFFFF, 0xc6c6c6);

	InputBox_Put_String(&inpbox, "Welcome to CHNOSProject.\n");
	sprintf(s, "Memory:%dByte:%dMB\n", System_MemoryControl_FullSize(), System_MemoryControl_FullSize() >> 20);
	InputBox_Put_String(&inpbox, s);
	sprintf(s, "Free:%dByte:%dMB\n", System_MemoryControl_FreeSize(), System_MemoryControl_FreeSize() >> 20);
	InputBox_Put_String(&inpbox, s);

	for (;;) {
		if(FIFO32_Status(&keycmd) > 0 && keycmd_wait < 0){
			keycmd_wait = FIFO32_Get(&keycmd);
			Keyboard_Controller_Wait_SendReady();
			IO_Out8(KEYB_DATA, keycmd_wait);
		}
		IO_CLI();
		if(FIFO32_Status(&fifo) == 0){
			IO_STIHLT();
		} else{
			i = FIFO32_Get(&fifo);
			Keyboard_Decode(&kinfo, i - DATA_BYTE);
			if(kinfo.make){
				InputBox_Put_Character(&inpbox, kinfo.c);
			}
		}
	}
}
