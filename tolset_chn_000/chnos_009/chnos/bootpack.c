
#include "core.h"

extern UI_Sheet_Control sheetctrl;

void CHNMain(void)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;
	uchar s[128];
	DATA_FIFO fifo, keycmd;
	int keycmd_wait = 0;
	UI_InputBox inpbox;
	UI_KeyInfo kinfo;
	uint i, j, x, y;
	uint cpuidbuf[5];	//EAX-EBX-EDX-ECX-0x00000000
	UI_Timer *c_timer;
	UI_Sheet *testsheet, *testsheet2, *testsheet3;

	IO_CLI();

	Initialise_System(&fifo, &keycmd, &keycmd_wait);

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
			InputBox_Put_String(&inpbox, "ExtendedCPUID is Enable.\n");
			sprintf(s, "Max=0x%08X.\n", cpuidbuf[0]);
			InputBox_Put_String(&inpbox, s);
			if(cpuidbuf[0] >= 0x80000004){
				CPUID2(&s[0], 0x80000002);
				CPUID2(&s[16], 0x80000003);
				CPUID2(&s[32], 0x80000004);
				InputBox_Put_String(&inpbox, s);
				InputBox_Put_String(&inpbox, "\n");
			}
		} else{
			InputBox_Put_String(&inpbox, "ExtendedCPUID is Disable.\n");
		}
	} else{
		InputBox_Put_String(&inpbox, "CPUID is Disable.\n");
	}
	IO_Store_EFlags(i);

	c_timer = Timer_Get(&fifo, 5);
	Timer_Set(c_timer, 50, interval);
	Timer_Run(c_timer);

	testsheet = Sheet_Get(100, 100, 32);
	for(y = 0; y < 100; y++){
		for(x = 0; x < 100; x++){
			((uint *)testsheet->vram)[100 * y + x] = 1643 * y + 1024 * x + y;
		}
	}
	Sheet_Show(testsheet, 200, 200, 0);

	testsheet2 = Sheet_Get(100, 100, 32);
	for(y = 0; y < 100; y++){
		for(x = 0; x < 100; x++){
			((uint *)testsheet2->vram)[100 * y + x] = (653 * y + 242 * x + y) * 1024;
		}
	}
	Sheet_Show(testsheet2, 250, 250, 1);

	testsheet3 = Sheet_Get(100, 100, 32);
	for(y = 0; y < 100; y++){
		for(x = 0; x < 100; x++){
			((uint *)testsheet3->vram)[100 * y + x] = (1192 * y + 828 * x + y) * 65536;
		}
	}
	Sheet_Show(testsheet3, 220, 180, 1);

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
			if(i < DATA_BYTE){
				if(i == 5){
					InputBox_Change_Cursor_State(&inpbox);
				}
			} else if(DATA_BYTE <= i && i <= (DATA_BYTE * 2)){
				Keyboard_Decode(&kinfo, i - DATA_BYTE);
				if(kinfo.make){
					if(kinfo.c == '\n'){
						InputBox_NewLine_No_Prompt(&inpbox);
						sprintf(s, "Count=%d\n", inpbox.input_count);
						InputBox_Put_String(&inpbox, inpbox.input_buf);
						InputBox_NewLine_No_Prompt(&inpbox);
						InputBox_Put_String(&inpbox, s);
						sprintf(s, "TimerTick=%u\n", Timer_Get_Tick());
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
}
