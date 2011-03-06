
#include "core.h"

void CHNMain(void)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;
	uchar s[128];
	uint fifobuf[128];
	DATA_FIFO fifo;
	UI_InputBox inpbox;

	FIFO32_Initialise(&fifo, 128, fifobuf);

	IO_CLI();
	Initialise_SerialPort();
	Initialise_GlobalDescriptorTable();
	Initialise_InterruptDescriptorTable();
	Initialise_ProgrammableInterruptController();
	Initialise_ProgrammableIntervalTimer();
	Initialise_Graphic(vesa->BitsPerPixel);
	System_MemoryControl_Initialise();
	Initialise_Paging(vesa->PhysBasePtr, boot->scrnx, boot->scrny, vesa->BitsPerPixel);
	IO_STI();

	InputBox_Initialise(&inpbox, vesa->PhysBasePtr, boot->scrnx, 0, 0, boot->scrnx - 1, boot->scrny - 1, 32, 0xFFFFFF, 0xc6c6c6);

	Draw_Put_String(vesa->PhysBasePtr, boot->scrnx, 0, 0, 0xFFFFFF, "Welcome to CHNOSProject.");
	sprintf(s, "Memory:%dByte:%dMB", System_MemoryControl_FullSize(), System_MemoryControl_FullSize() >> 20);
	Draw_Put_String(vesa->PhysBasePtr, boot->scrnx, 0, 16, 0xFFFFFF, s);
	sprintf(s, "Free:%dByte:%dMB", System_MemoryControl_FreeSize(), System_MemoryControl_FreeSize() >> 20);
	Draw_Put_String(vesa->PhysBasePtr, boot->scrnx, 0, 32, 0xFFFFFF, s);

	for (;;) {
		IO_CLI();

		IO_STIHLT();
	}
}
