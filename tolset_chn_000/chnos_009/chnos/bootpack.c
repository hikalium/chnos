
#include "core.h"

void CHNMain(void)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;
	uint i;
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
	IO_STI();

	Draw_Fill_Rectangle(vesa->PhysBasePtr, boot->scrnx, 0xc6c6c6, 0, 0, boot->scrnx, boot->scrny);
	Draw_Put_String(vesa->PhysBasePtr, boot->scrnx, 0, 0, 0xFFFFFF, "Welcome to CHNOSProject.");
	i = Memory_Test(0x00400000, 0xbfffffff);
	MemoryControl_Initialise((IO_MemoryControl *)(i - sizeof(IO_MemoryControl)));
	MemoryControl_Free((IO_MemoryControl *)(i - sizeof(IO_MemoryControl)), (void *)0x00400000, i - sizeof(IO_MemoryControl) - 0x00400000);

	InputBox_Initialise(&inpbox, vesa->PhysBasePtr, 0, 0, boot->scrnx, boot->scrny, 32, 0xFFFFFF, 0xc6c6c6);

	sprintf(s, "Memory:%dByte:%dMB", i, i >> 20);
	Draw_Put_String(vesa->PhysBasePtr, boot->scrnx, 0, 16, 0xFFFFFF, s);
	sprintf(s, "Free:%dByte:%dMB", MemoryControl_FreeSize((IO_MemoryControl *)(i - sizeof(IO_MemoryControl))), MemoryControl_FreeSize((IO_MemoryControl *)(i - sizeof(IO_MemoryControl))) >> 20);
	Draw_Put_String(vesa->PhysBasePtr, boot->scrnx, 0, 32, 0xFFFFFF, s);
	for (;;) {
		IO_CLI();

		IO_STIHLT();
	}
}
