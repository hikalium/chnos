
#include "core.h"

extern UI_Sheet_Control sheetctrl;
extern IO_MemoryControl sys_mem_ctrl;

void CHNMain(void)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;
	uchar s[128];
	DATA_FIFO fifo, keycmd;
	int keycmd_wait = 0;
	UI_InputBox console;
	UI_KeyInfo kinfo;
	uint i, j, x, y;
	uint cpuidbuf[5];	//EAX-EBX-EDX-ECX-0x00000000
	UI_Timer *c_timer;
	UI_Sheet *testsheet, *testsheet2, *testsheet3, *taskbar, *desktop;
	UI_Mouse mdecode;
	DATA_Position2D mcursor;

	mcursor.x = 0;
	mcursor.y = 0;

	IO_CLI();

	Initialise_System(&fifo, &keycmd, &keycmd_wait, &mdecode);

	IO_STI();

	desktop = Sheet_Get(boot->scrnx, boot->scrny, 0);
	Sheet_Show(desktop, 0, 0, 0);
	Sheet_Draw_Fill_Rectangle(desktop, 0x66FF66, 0, 0, desktop->size.x - 1, desktop->size.y - 1);

	InputBox_Initialise(&console, 8, 16, boot->scrnx - 16, boot->scrny >> 1, 1024, 0xFFFFFF, 0xc6c6c6, 1);

	taskbar = Sheet_Get(boot->scrnx, 32, 0);
	Sheet_Show(taskbar, 0, boot->scrny - 32, 2);
	Sheet_Draw_Fill_Rectangle(taskbar, 0x6666FF, 0, 0, taskbar->size.x - 1, taskbar->size.y - 1);
	Sheet_Draw_Put_String(taskbar, 0, 0, 0xFFFFFF, "Taskbar");


	InputBox_Put_String(&console, "Welcome to CHNOSProject.\n");

	sprintf(s, "Memory:%dByte:%dMB\n", System_MemoryControl_FullSize(), System_MemoryControl_FullSize() >> 20);
	InputBox_Put_String(&console, s);

	sprintf(s, "MemoryControl:[0x%08X]\n", sys_mem_ctrl.next);
	InputBox_Put_String(&console, s);

	sprintf(s, "Free:%dByte:%dMB\n", System_MemoryControl_FreeSize(), System_MemoryControl_FreeSize() >> 20);
	InputBox_Put_String(&console, s);

	sprintf(s, "VideoMode:%dbit(%dx%d)[0x%08X]\n", vesa->BitsPerPixel, boot->scrnx, boot->scrny, vesa->PhysBasePtr);
	InputBox_Put_String(&console, s);

	i = IO_Load_EFlags();
	IO_Store_EFlags(i | 0x00200000);
	j = IO_Load_EFlags();
	if((j | 0xffdfffff) == 0xffffffff){
		InputBox_Put_String(&console, "CPUID is Enable.\n");
		cpuidbuf[4] = 0x00000000;
		CPUID(cpuidbuf, 0);
		sprintf(s, "Max=0x%08X VendorID=%s.\n", cpuidbuf[0], &cpuidbuf[1]);
		InputBox_Put_String(&console, s);
		CPUID(cpuidbuf, 0x80000000);
		if(cpuidbuf[0] >= 0x80000000){
			InputBox_Put_String(&console, "ExtendedCPUID is Enable.\n");
			sprintf(s, "Max=0x%08X.\n", cpuidbuf[0]);
			InputBox_Put_String(&console, s);
			if(cpuidbuf[0] >= 0x80000004){
				CPUID2(&s[0], 0x80000002);
				CPUID2(&s[16], 0x80000003);
				CPUID2(&s[32], 0x80000004);
				InputBox_Put_String(&console, s);
				InputBox_Put_String(&console, "\n");
			}
		} else{
			InputBox_Put_String(&console, "ExtendedCPUID is Disable.\n");
		}
	} else{
		InputBox_Put_String(&console, "CPUID is Disable.\n");
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
	Sheet_Show(testsheet, 200, 200, 3);
	Sheet_Draw_Put_String(testsheet, 0, 0, 0xFFFFFF, "TestSheet");

	testsheet2 = Sheet_Get(100, 100, 32);
	for(y = 0; y < 100; y++){
		for(x = 0; x < 100; x++){
			((uint *)testsheet2->vram)[100 * y + x] = (653 * y + 242 * x + y) * 1024;
		}
	}
	Sheet_Show(testsheet2, 250, 250, 4);
	Sheet_Draw_Put_String(testsheet2, 0, 0, 0xFFFFFF, "TestSheet2");

	testsheet3 = Sheet_Get(100, 100, 32);
	for(y = 0; y < 100; y++){
		for(x = 0; x < 100; x++){
			((uint *)testsheet3->vram)[100 * y + x] = (1192 * y + 828 * x + y) * 65536;
		}
	}
	Sheet_Show(testsheet3, 220, 180, 4);
	Sheet_Draw_Put_String(testsheet3, 0, 0, 0xFFFFFF, "TestSheet3");

	InputBox_NewLine(&console);
	InputBox_Reset_Input_Buffer(&console);
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
					InputBox_Change_Cursor_State(&console);
				}
			} else if(DATA_BYTE <= i && i < (DATA_BYTE * 2)){
				Keyboard_Decode(&kinfo, i - DATA_BYTE);
				if(kinfo.make){
					if(kinfo.c == '\n'){
						InputBox_NewLine_No_Prompt(&console);
						sprintf(s, "Count=%d\n", console.input_count);
						InputBox_Put_String(&console, console.input_buf);
						InputBox_NewLine_No_Prompt(&console);
						InputBox_Put_String(&console, s);
						sprintf(s, "TimerTick=%u\n", Timer_Get_Tick());
						InputBox_Put_String(&console, s);
						InputBox_Reset_Input_Buffer(&console);
						InputBox_NewLine(&console);
					} else{
						if(kinfo.c == 'a'){
							Sheet_Slide(testsheet3, testsheet3->position.x - 5, testsheet3->position.y);
						} else if(kinfo.c == 'w'){
							Sheet_Slide(testsheet3, testsheet3->position.x, testsheet3->position.y - 5);
						} else if(kinfo.c == 'd'){
							Sheet_Slide(testsheet3, testsheet3->position.x + 5, testsheet3->position.y);
						} else if(kinfo.c == 'x'){
							Sheet_Slide(testsheet3, testsheet3->position.x, testsheet3->position.y + 5);
						}
						InputBox_Put_Character(&console, kinfo.c);
					}
				}
			} else if((DATA_BYTE * 2) <= i && i < (DATA_BYTE * 3)){
				if(Mouse_Decode(i - DATA_BYTE * 2) != 0){
					Sheet_Draw_Fill_Rectangle(taskbar, 0x6666FF, 0, 0, taskbar->size.x - 1, 15);
					mcursor.x += mdecode.move.x;
					mcursor.y += mdecode.move.y;
					if(mcursor.x < 0){
						mcursor.x = 0;
					}
					if(mcursor.y < 0){
						mcursor.y = 0;
					}
					if(mcursor.x >= boot->scrnx){
						mcursor.x = boot->scrnx - 1;
					}
					if(mcursor.y >= boot->scrny){
						mcursor.y = boot->scrny - 1;
					}
					sprintf(s, "Mouse Type:0x%02X Button:0x%02X (%04d, %04d)", mdecode.type, mdecode.btn, mcursor.x, mcursor.y);
					Sheet_Draw_Put_String(taskbar, 0, 0, 0xFFFFFF, s);
					Sheet_Slide(testsheet3, mcursor.x, mcursor.y);
				}
			}
		}
	}
}
