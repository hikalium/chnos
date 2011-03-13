
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
	uint i, j;
	uint cpuidbuf[5];	//EAX-EBX-EDX-ECX-0x00000000

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

	sprintf(s, "VideoMode:%dbit %dx%d\n", vesa->BitsPerPixel, boot->scrnx, boot->scrny);
	InputBox_Put_String(&inpbox, s);

	i = IO_Load_EFlags();
	IO_Store_EFlags(i | 0x00200000);
	j = IO_Load_EFlags();
	if((j | 0xffdfffff) == 0xffffffff){
		InputBox_Put_String(&inpbox, "CPUID is Enable.\n");
		cpuidbuf[4] = 0x00000000;
		CPUID(cpuidbuf, 0);
		sprintf(s, "Max=0x%08X VendorID=%s.\n", cpuidbuf[0], &cpuidbuf[1]);
		InputBox_Put_String(&inpbox, s);
		CPUID(cpuidbuf, 0x80000000);
		if(cpuidbuf[0] >= 0x80000000){
			sprintf(s, "ExtendedCPUIDMax=0x%08X.\n", cpuidbuf[0]);
		} else{
			sprintf(s, "ExtendedCPUID is Disable.\n", cpuidbuf[0]);
		}
		InputBox_Put_String(&inpbox, s);
	} else{
		InputBox_Put_String(&inpbox, "CPUID is Disable.\n");
	}
	IO_Store_EFlags(i);

	InputBox_NewLine(&inpbox);
	InputBox_Reset_Input_Buffer(&inpbox);
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
				if(kinfo.c == '\n'){
					InputBox_NewLine_No_Prompt(&inpbox);
					sprintf(s, "Count=%d\n", inpbox.input_count);
					InputBox_Put_String(&inpbox, inpbox.input_buf);
					InputBox_NewLine_No_Prompt(&inpbox);
					InputBox_Put_String(&inpbox, s);
					InputBox_Reset_Input_Buffer(&inpbox);
					InputBox_NewLine(&inpbox);
				} else{
					InputBox_Put_Character(&inpbox, kinfo.c);
				}
			}
		}
	}
}
