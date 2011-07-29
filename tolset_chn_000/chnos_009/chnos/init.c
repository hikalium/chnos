
#include "core.h"

void Initialise_System(System_CommonData *systemdata)
{
	uint eflags;

	systemdata->vesainfo = (DATA_VESAInfo *) ADR_VESAINFO;
	systemdata->bootinfo = (DATA_BootInfo *) ADR_BOOTINFO;
	systemdata->focus = 0;
	systemdata->key_focus = 0;

	eflags = IO_Load_EFlags();
	IO_CLI();

	Initialise_SerialPort();

	Initialise_Graphic(systemdata->vesainfo->BitsPerPixel);
	Draw_Fill_Rectangle(systemdata->vesainfo->PhysBasePtr, systemdata->bootinfo->scrnx - 1, 0x00000000, 0, 0, systemdata->bootinfo->scrnx - 1, systemdata->bootinfo->scrny - 1);

	Initialise_GlobalDescriptorTable();
	Initialise_InterruptDescriptorTable();

	System_MemoryControl_Initialise();
	Initialise_Paging(systemdata->vesainfo->PhysBasePtr, systemdata->bootinfo->scrnx, systemdata->bootinfo->scrny, systemdata->vesainfo->BitsPerPixel);
	Initialise_MemoryBlock(&sys_mem_ctrl);

	FIFO32_Initialise(&systemdata->sysfifo, SYS_FIFOSIZE);
	FIFO32_Initialise(&systemdata->keyboardfifo, KEYCTRL_FIFOSIZE);
	FIFO32_Initialise(&systemdata->mousefifo, MOUSECTRL_FIFOSIZE);
	FIFO32_Initialise(&systemdata->keycmdfifo, KEYCMD_FIFOSIZE);

	Initialise_ProgrammableInterruptController();
	Initialise_ProgrammableIntervalTimer();

	Initialise_MultiTask();
	systemdata->maintask = taskctrl->main;

	Initialise_Keyboard(&systemdata->keyboardfifo, &systemdata->keycmdfifo, DATA_BYTE, systemdata->bootinfo->leds, &systemdata->keycmd_wait);
	Initialise_Mouse(&systemdata->mousefifo, DATA_BYTE, &systemdata->mousedecode);

	System_Sheet_Initialise(systemdata->vesainfo->PhysBasePtr, systemdata->bootinfo->scrnx, systemdata->bootinfo->scrny, systemdata->vesainfo->BitsPerPixel);
	Initialise_Window(&systemdata->windowctrl);
	Initialise_Console(&systemdata->consctrl);

	IO_Store_EFlags(eflags);

	return;
}
