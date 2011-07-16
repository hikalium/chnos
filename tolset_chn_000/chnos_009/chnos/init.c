
#include "core.h"

extern IO_MemoryControl sys_mem_ctrl;

void Initialise_System(DATA_FIFO *fifo, DATA_FIFO *keycmd, uint *keycmd_wait, UI_MouseInfo *decode)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

Emergency_Out_Reset();
	Initialise_SerialPort();
	Initialise_Graphic(vesa->BitsPerPixel);
	Draw_Fill_Rectangle(vesa->PhysBasePtr, boot->scrnx - 1, 0x00000000, 0, 0, boot->scrnx - 1, boot->scrny - 1);
Emergency_Out("CHNOSProject Core loaded...:Graphics");
	Initialise_GlobalDescriptorTable();
Emergency_Out("CHNOSProject Core loaded...:GDT");
	Initialise_InterruptDescriptorTable();
Emergency_Out("CHNOSProject Core loaded...:IDT");
	System_MemoryControl_Initialise();
Emergency_Out("CHNOSProject Core loaded...:Memory");
	Initialise_Paging(vesa->PhysBasePtr, boot->scrnx, boot->scrny, vesa->BitsPerPixel);
Emergency_Out("CHNOSProject Core loaded...:Memory-Paging");
	Initialise_MemoryBlock(&sys_mem_ctrl);
Emergency_Out("CHNOSProject Core loaded...:Memory-Block");
	FIFO32_Initialise(fifo, 128);
Emergency_Out("CHNOSProject Core loaded...:SystemFIFO");
	FIFO32_Initialise(keycmd, 128);
Emergency_Out("CHNOSProject Core loaded...:KeycmdFIFO");
	Initialise_ProgrammableInterruptController();
Emergency_Out("CHNOSProject Core loaded...:PIC");
	Initialise_ProgrammableIntervalTimer();
Emergency_Out("CHNOSProject Core loaded...:PIT");
	Initialise_MultiTask();
Emergency_Out("CHNOSProject Core loaded...:MultiTask");
	Initialise_Keyboard(fifo, keycmd, DATA_BYTE, boot->leds, keycmd_wait);
Emergency_Out("CHNOSProject Core loaded...:Keyboard");
	Initialise_Mouse(fifo, DATA_BYTE * 2, decode);
Emergency_Out("CHNOSProject Core loaded...:Mouse");
	System_Sheet_Initialise(vesa->PhysBasePtr, boot->scrnx, boot->scrny, vesa->BitsPerPixel);
Emergency_Out("CHNOSProject Core loaded...:SystemSheet");
	IO_Store_EFlags(eflags);
Emergency_Out_Reset();
	return;
}
