
#include "core.h"

void CHNMain(void)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;

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

	for (;;) {
		IO_CLI();

		IO_STIHLT();
	}
}
