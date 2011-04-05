
#include "core.h"

void Initialise_System(DATA_FIFO *fifo, DATA_FIFO *keycmd, uint *keycmd_wait, UI_MouseInfo *decode)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	Initialise_SerialPort();
	Initialise_GlobalDescriptorTable();
	Initialise_InterruptDescriptorTable();
	System_MemoryControl_Initialise();
	Initialise_Paging(vesa->PhysBasePtr, boot->scrnx, boot->scrny, vesa->BitsPerPixel);
	FIFO32_Initialise(fifo, 128);
	FIFO32_Initialise(keycmd, 128);
	Initialise_ProgrammableInterruptController();
	Initialise_ProgrammableIntervalTimer();
	Initialise_Keyboard(fifo, keycmd, DATA_BYTE, boot->leds, keycmd_wait);
	Initialise_Mouse(fifo, DATA_BYTE * 2, decode);
	Initialise_Graphic(vesa->BitsPerPixel);
	Initialise_Sheet(vesa->PhysBasePtr, boot->scrnx, boot->scrny, vesa->BitsPerPixel);

	IO_Store_EFlags(eflags);
	return;
}
