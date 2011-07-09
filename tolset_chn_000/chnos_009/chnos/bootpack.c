
#include "core.h"

extern UI_Sheet_Control sys_sheet_ctrl;
extern IO_MemoryControl sys_mem_ctrl;

extern uint memcounter;

char *ACPI_MemoryMap_Type[5] = {
	"  USABLE",
	"RESERVED",
	"    ACPI",
	"ACPI_NVS",
	"UNUSABLE"
};

void CHNMain(void)
{
	DATA_VESAInfo *vesa = (DATA_VESAInfo *) ADR_VESAINFO;
	DATA_BootInfo *boot = (DATA_BootInfo *) ADR_BOOTINFO;
	uchar s[128];
	DATA_FIFO fifo, keycmd;
	int keycmd_wait = 0;
	UI_InputBox console;
	UI_KeyInfo kinfo;
	uint i, j;
	UI_Timer *c_timer, *core_timer;
	UI_Sheet *taskbar, *desktop, *focus, *core;
	UI_MouseInfo mdecode;
	UI_MouseCursor mouse_cursor;
	DATA_Position2D focus_moveorg;
	bool coremode = false;
	DATA_CPUID cpuid;
	uint tsc[2];

	focus = (UI_Sheet *)0xFFFFFFFF;

	IO_CLI();

	Initialise_System(&fifo, &keycmd, &keycmd_wait, &mdecode);

	IO_STI();

	Mouse_Make_MouseCursor(&mouse_cursor, 0, 0, boot->scrnx - 1, boot->scrny - 1, System_Sheet_Get_Top_Of_Height());
	Mouse_Move_Absolute(&mouse_cursor, boot->scrnx >> 1, boot->scrny >> 1);

	core = System_Sheet_Get(boot->scrnx, boot->scrny, 0, 0);
	Sheet_Draw_Fill_Rectangle(core, 0x333333, 0, 0, core->size.x - 1, core->size.y - 1);
	Sheet_Draw_Put_String(core, 0, 0, 0xFFFFFF, "CHNOSProject Beta 009");
	Sheet_Draw_Put_String(core, boot->scrnx - (8 * 14), 0, 0xFFFFFF, "System Console");
	sprintf(s, "Memory        :%dByte:%dMB", System_MemoryControl_FullSize(), System_MemoryControl_FullSize() >> 20);
	Sheet_Draw_Put_String(core, 0, 16 * 2, 0xFFFFFF, s);
	sprintf(s, "Free          :%dByte:%dMB", System_MemoryControl_FreeSize(), System_MemoryControl_FreeSize() >> 20);
	Sheet_Draw_Put_String(core, 0, 16 * 3, 0xFFFFFF, s);
	sprintf(s, "MemoryControl :[0x%08X]", &sys_mem_ctrl);
	Sheet_Draw_Put_String(core, 0, 16 * 4, 0xFFFFFF, s);
	sprintf(s, "SheetControl  :[0x%08X]", &sys_sheet_ctrl);
	Sheet_Draw_Put_String(core, 0, 16 * 5, 0xFFFFFF, s);
	sprintf(s, "VideoMode     :%dbit(%dx%d)[0x%08X]", vesa->BitsPerPixel, boot->scrnx, boot->scrny, vesa->PhysBasePtr);
	Sheet_Draw_Put_String(core, 0, 16 * 6, 0xFFFFFF, s);
	CPU_Identify(&cpuid);
	if(cpuid.enable){
		Sheet_Draw_Put_String(core, 0, 16 * 7, 0xFFFFFF, "CPUID         :Enable");
		sprintf(s, "+---Max       :0x%08X", cpuid.max);
		Sheet_Draw_Put_String(core, 0, 16 * 8, 0xFFFFFF, s);
		sprintf(s, "+---VendorID  :%s", cpuid.vendor);
		Sheet_Draw_Put_String(core, 0, 16 * 9, 0xFFFFFF, s);
		if(cpuid.ext_enable){
			Sheet_Draw_Put_String(core, 0, 16 * 10, 0xFFFFFF, "ExtendedCPUID :Enable");
			sprintf(s, "+---Max       :0x%08X", cpuid.ext_max);
			Sheet_Draw_Put_String(core, 0, 16 * 11, 0xFFFFFF, s);
			sprintf(s, "+---Brand     :%s", cpuid.brand_string);
			Sheet_Draw_Put_String(core, 0, 16 * 12, 0xFFFFFF, s);
		} else{
			Sheet_Draw_Put_String(core, 0, 16 * 10, 0xFFFFFF, "ExtendedCPUID :Disable");
		}
	} else{
		Sheet_Draw_Put_String(core, 0, 16 * 7, 0xFFFFFF, "CPUID         :Disable");
	}

	sprintf(s, "RealTimeClock :%2X%02X/%2X/%2X %2X:%2X:%2X", IO_Read_CMOS(0x32), IO_Read_CMOS(0x09), IO_Read_CMOS(0x08), IO_Read_CMOS(0x07), IO_Read_CMOS(0x04), IO_Read_CMOS(0x02), IO_Read_CMOS(0x00));
	Sheet_Draw_Put_String(core, 0, 16 * 13, 0xFFFFFF, s);

	desktop = System_Sheet_Get(boot->scrnx, boot->scrny, 0, 0);
	Sheet_Show(desktop, 0, 0, System_Sheet_Get_Top_Of_Height());
	Sheet_Draw_Fill_Rectangle(desktop, 0x66FF66, 0, 0, desktop->size.x - 1, desktop->size.y - 1);

	InputBox_Initialise(&sys_sheet_ctrl, &sys_mem_ctrl, &console, 8, 16, boot->scrnx - 16, boot->scrny >> 1, 1024, 0xFFFFFF, 0xc6c6c6, System_Sheet_Get_Top_Of_Height());

	taskbar = System_Sheet_Get(boot->scrnx, 32, 0, 0);
	Sheet_Show(taskbar, 0, boot->scrny - 32, System_Sheet_Get_Top_Of_Height());
	Sheet_Draw_Fill_Rectangle(taskbar, 0x6666FF, 0, 0, taskbar->size.x - 1, taskbar->size.y - 1);
	Sheet_Draw_Put_String(taskbar, 0, 0, 0xFFFFFF, "Taskbar");


	InputBox_Put_String(&console, "Welcome to CHNOSProject.\n");
	InputBox_Put_String(&console, "Press [Shift + F10] to show System Console.\n");

	c_timer = Timer_Get(&fifo, 5);
	Timer_Set(c_timer, 50, interval);
	Timer_Run(c_timer);

	core_timer = Timer_Get(&fifo, 10);
	Timer_Set(core_timer, 10, interval);
	Timer_Run(core_timer);

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
			if(focus != 0 && focus != (UI_Sheet *)0xFFFFFFFF){
				Sheet_Slide(focus, focus->position.x + (mouse_cursor.position.x - focus_moveorg.x), focus->position.y + (mouse_cursor.position.y - focus_moveorg.y));
				focus_moveorg.x = mouse_cursor.position.x;
				focus_moveorg.y = mouse_cursor.position.y;
			}
			IO_STIHLT();
		} else{
			i = FIFO32_Get(&fifo);
			if(i < DATA_BYTE){
				if(i == 5){
					InputBox_Change_Cursor_State(&console);
				} else if(i == 10){
					sprintf(s, "Free          :%dByte:%dMB", System_MemoryControl_FreeSize(), System_MemoryControl_FreeSize() >> 20);
					Sheet_Draw_Put_String_With_BackColor(core, 0, 16 * 3, 0xFFFFFF, 0x333333, s);
					sprintf(s, "RealTimeClock :%2X%02X/%2X/%2X %2X:%2X:%2X", IO_Read_CMOS(0x32), IO_Read_CMOS(0x09), IO_Read_CMOS(0x08), IO_Read_CMOS(0x07), IO_Read_CMOS(0x04), IO_Read_CMOS(0x02), IO_Read_CMOS(0x00));
					Sheet_Draw_Put_String_With_BackColor(core, 0, 16 * 13, 0xFFFFFF, 0x333333, s);
					if(cpuid.TSC){
						Read_TSC(tsc);
						sprintf(s, "TSC-Count     :%X%X", tsc[0], tsc[1]);
						Sheet_Draw_Put_String_With_BackColor(core, 0, 16 * 14, 0xFFFFFF, 0x333333, s);
					}
					sprintf(s, "TimerTick     :%u", Timer_Get_Tick());
					Sheet_Draw_Put_String_With_BackColor(core, 0, 16 * 15, 0xFFFFFF, 0x333333, s);
				}
			} else if(DATA_BYTE <= i && i < (DATA_BYTE * 2)){
				Keyboard_Decode(&kinfo, i - DATA_BYTE);
				if(kinfo.make){
					if(kinfo.c != 0){
						if(kinfo.c == '\n'){
							if(console.input_buf[0] != 0x00){
								InputBox_NewLine_No_Prompt(&console);
							}
							if(strcmp(console.input_buf, "cls") == 0){
								InputBox_Clear(&console);
							} else if(strcmp(console.input_buf, "memmap") == 0){
								sprintf(s, "ACPI 0xe820 MemoryMaps:%d\n", boot->ACPI_MemoryMapEntries);
								InputBox_Put_String(&console, s);
								for(j = 0; j < boot->ACPI_MemoryMapEntries; j++){
									sprintf(s, "%02d:[0x%08X%08X](0x%08X%08X) %s 0x%08X\n", j, boot->ACPI_MemoryMap[j].Base.high, boot->ACPI_MemoryMap[j].Base.low, boot->ACPI_MemoryMap[j].Length.high, boot->ACPI_MemoryMap[j].Length.low, ACPI_MemoryMap_Type[boot->ACPI_MemoryMap[j].Type], boot->ACPI_MemoryMap[j].Attribute);
									InputBox_Put_String(&console, s);
								}
							} else if(strcmp(console.input_buf, "systeminfo") == 0){
								sprintf(s, "ACPI 0xe820 MemoryMaps:%d\n", boot->ACPI_MemoryMapEntries);
								InputBox_Put_String(&console, s);
								sprintf(s, "APM-Version:%X.%X\n", boot->APM_Version >> 8, boot->APM_Version & 0x00FF);
								InputBox_Put_String(&console, s);
								sprintf(s, "APM-Flags:0x%04X\n", boot->APM_Flags);
								InputBox_Put_String(&console, s);
								sprintf(s, "VESA-Version:%X.%X\n", boot->VESA_Version >> 8, boot->VESA_Version & 0x00FF);
								InputBox_Put_String(&console, s);
							} else if(strcmp(console.input_buf, "mem") == 0){
							} else if(console.input_buf[0] != 0x00){
								InputBox_Put_String(&console, "Bad Command...");
							}
							InputBox_Reset_Input_Buffer(&console);
							InputBox_NewLine(&console);
						} else{
							InputBox_Put_Character(&console, kinfo.c);
						}
					} else{
						if(Keyboard_Get_KeyShift() != 0 && kinfo.keycode == 0x44){	//Shift + F10
							if(coremode){
								coremode = false;
								Sheet_Remove(core);
							} else{
								coremode = true;
								Sheet_Show(core, 0, 0, System_Sheet_Get_Top_Of_Height());
							}
						}
					}
				}
			} else if((DATA_BYTE * 2) <= i && i < (DATA_BYTE * 3)){
				if(Mouse_Decode(i - DATA_BYTE * 2) != 0){
					Sheet_Draw_Fill_Rectangle(taskbar, 0x6666FF, 0, 0, taskbar->size.x - 1, 15);
					Mouse_Move_Relative(&mouse_cursor, mdecode.move.x, mdecode.move.y);
					sprintf(s, "Mouse Type:0x%02X Button:lcr (%04d, %04d)", mdecode.type, mouse_cursor.position.x, mouse_cursor.position.y);
					if((mdecode.btn & 0x01) != 0){
						s[23] = 'L';
						if(focus == (UI_Sheet *)0xFFFFFFFF){
							focus = Sheet_Get_From_Position(&sys_sheet_ctrl, mouse_cursor.position.x, mouse_cursor.position.y);
							focus_moveorg.x = mouse_cursor.position.x;
							focus_moveorg.y = mouse_cursor.position.y;
							if(focus == desktop || focus == taskbar || focus == core){
								focus = 0;
							} else{
								Sheet_UpDown(focus, System_Sheet_Get_Top_Of_Height());
							}
						}
					} else{
						focus = (UI_Sheet *)0xFFFFFFFF;
					}
					if((mdecode.btn & 0x02) != 0){
						s[25] = 'R';
					}
					if((mdecode.btn & 0x04) != 0){
						s[24] = 'C';
					}
					Sheet_Draw_Put_String(taskbar, 0, 0, 0xFFFFFF, s);
				}
			}
		}
	}
}
