
#include "core.h"

extern UI_Sheet_Control sys_sheet_ctrl;
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
	uint i, x, y;
	UI_Timer *c_timer, *core_timer;
	UI_Sheet *testsheet, *testsheet2, *taskbar, *desktop, *focus, *core;
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

	testsheet = System_Sheet_Get(256, 256, 32, 0);
	for(y = 0; y < 256; y++){
		for(x = 0; x < 256; x++){
			((uint *)testsheet->vram)[256 * y + x] =  (y << 16) + (x << 8);
		}
	}
	Sheet_Show(testsheet, 10, 10, System_Sheet_Get_Top_Of_Height());
	Sheet_Draw_Put_String(testsheet, 0, 0, 0xFFFFFF, "TestSheet");

	testsheet2 = System_Sheet_Get(100, 100, 32, 0);
	for(y = 0; y < 100; y++){
		for(x = 0; x < 100; x++){
			((uint *)testsheet2->vram)[100 * y + x] = (653 * y + 242 * x + y) * 1024;
		}
	}
	Sheet_Show(testsheet2, 250, 250, System_Sheet_Get_Top_Of_Height());
	Sheet_Draw_Put_String(testsheet2, 0, 0, 0xFFFFFF, "TestSheet2");

	Sheet_Remove(testsheet2);

	Sheet_Show(testsheet2, 250, 250, System_Sheet_Get_Top_Of_Height());

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
				}
			} else if(DATA_BYTE <= i && i < (DATA_BYTE * 2)){
				Keyboard_Decode(&kinfo, i - DATA_BYTE);
				if(kinfo.make){
					if(kinfo.c != 0){
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

void CPU_Identify(DATA_CPUID *id)
{
	uint eflags0, eflags1;
	uint cpuidbuf[5];	//EAX-EBX-EDX-ECX-0x00000000
	uchar s[64];

	cpuidbuf[4] = 0x00000000;

	eflags0 = IO_Load_EFlags();
	IO_Store_EFlags(eflags0 | 0x00200000);
	eflags1 = IO_Load_EFlags();
	IO_Store_EFlags(eflags0);

	if((eflags1 | 0xffdfffff) == 0xffffffff){
		id->enable = true;
		CPUID(cpuidbuf, 0);
		id->max = cpuidbuf[0];
		sprintf(id->vendor, "%s", &cpuidbuf[1]);

		if(id->max >= 1){
			CPUID(cpuidbuf, 1);
			if((cpuidbuf[2] & ~1 << 0) != 0){
				id->FPU = true;
			} else{
				id->FPU = false;
			}
			if((cpuidbuf[2] & ~(1 << 1)) != 0){
				id->VME = true;
			} else{
				id->VME = false;
			}
			if((cpuidbuf[2] & ~(1 << 2)) != 0){
				id->DE = true;
			} else{
				id->DE = false;
			}
			if((cpuidbuf[2] & ~(1 << 3)) != 0){
				id->PSE = true;
			} else{
				id->PSE = false;
			}
			if((cpuidbuf[2] & ~(1 << 4)) != 0){
				id->TSC = true;
			} else{
				id->TSC = false;
			}
			if((cpuidbuf[2] & ~(1 << 5)) != 0){
				id->MSR = true;
			} else{
				id->MSR = false;
			}
			if((cpuidbuf[2] & ~(1 << 6)) != 0){
				id->PAE = true;
			} else{
				id->PAE = false;
			}
			if((cpuidbuf[2] & ~(1 << 7)) != 0){
				id->MCE = true;
			} else{
				id->MCE = false;
			}
			if((cpuidbuf[2] & ~(1 << 8)) != 0){
				id->CX8 = true;
			} else{
				id->CX8 = false;
			}
			if((cpuidbuf[2] & ~(1 << 9)) != 0){
				id->APIC = true;
			} else{
				id->APIC = false;
			}
			if((cpuidbuf[2] & ~(1 << 11)) != 0){
				id->SEP = true;
			} else{
				id->SEP = false;
			}
			if((cpuidbuf[2] & ~(1 << 12)) != 0){
				id->MTRR = true;
			} else{
				id->MTRR = false;
			}
			if((cpuidbuf[2] & ~(1 << 13)) != 0){
				id->PGE = true;
			} else{
				id->PGE = false;
			}
			if((cpuidbuf[2] & ~(1 << 14)) != 0){
				id->MCA = true;
			} else{
				id->MCA = false;
			}
			if((cpuidbuf[2] & ~(1 << 15)) != 0){
				id->CMOV = true;
			} else{
				id->CMOV = false;
			}
			if((cpuidbuf[2] & ~(1 << 16)) != 0){
				id->PAT = true;
			} else{
				id->PAT = false;
			}
			if((cpuidbuf[2] & ~(1 << 17)) != 0){
				id->PSE36 = true;
			} else{
				id->PSE36 = false;
				}
			if((cpuidbuf[2] & ~(1 << 18)) != 0){
				id->PSN = true;
			} else{
				id->PSN = false;
			}
			if((cpuidbuf[2] & ~(1 << 19)) != 0){
				id->CLFSH = true;
			} else{
				id->CLFSH = false;
			}
			if((cpuidbuf[2] & ~(1 << 21)) != 0){
				id->DS = true;
			} else{
				id->DS = false;
			}
			if((cpuidbuf[2] & ~(1 << 22)) != 0){
				id->ACPI = true;
			} else{
				id->ACPI = false;
			}
			if((cpuidbuf[2] & ~(1 << 23)) != 0){
				id->MMX = true;
			} else{
				id->MMX = false;
			}
			if((cpuidbuf[2] & ~(1 << 24)) != 0){
				id->FXSR = true;
			} else{
				id->FXSR = false;
			}
			if((cpuidbuf[2] & ~(1 << 25)) != 0){
				id->SSE = true;
			} else{
				id->SSE = false;
			}
			if((cpuidbuf[2] & ~(1 << 26)) != 0){
				id->SSE2 = true;
			} else{
				id->SSE2 = false;
			}
			if((cpuidbuf[2] & ~(1 << 27)) != 0){
				id->SS = true;
			} else{
				id->SS = false;
			}
			if((cpuidbuf[2] & ~(1 << 28)) != 0){
				id->HTT = true;
			} else{
				id->HTT = false;
			}
			if((cpuidbuf[2] & ~(1 << 29)) != 0){
				id->TM = true;
			} else{
				id->TM = false;
			}
			if((cpuidbuf[2] & ~(1 << 31)) != 0){
				id->PBE = true;
			} else{
				id->PBE = false;
			}

			if((cpuidbuf[3] & ~(1 << 0)) != 0){
				id->SSE3 = true;
			} else{
				id->SSE3 = false;
			}
			if((cpuidbuf[3] & ~(1 << 3)) != 0){
				id->MONITOR = true;
			} else{
				id->MONITOR = false;
			}
			if((cpuidbuf[3] & ~(1 << 4)) != 0){
				id->DSCPL = true;
			} else{
				id->DSCPL = false;
			}
			if((cpuidbuf[3] & ~(1 << 7)) != 0){
				id->EST = true;
			} else{
				id->EST = false;
			}
			if((cpuidbuf[3] & ~(1 << 8)) != 0){
				id->TM2 = true;
			} else{
				id->TM2 = false;
			}
			if((cpuidbuf[3] & ~(1 << 10)) != 0){
				id->CID = true;
			} else{
				id->CID = false;
			}
			if((cpuidbuf[3] & ~(1 << 13)) != 0){
				id->CX16 = true;
			} else{
				id->CX16 = false;
			}
			if((cpuidbuf[3] & ~(1 << 14)) != 0){
				id->xTPR = true;
			} else{
				id->xTPR = false;
			}
		}
		CPUID(cpuidbuf, 0x80000000);
		if(cpuidbuf[0] >= 0x80000000){
			id->ext_enable = true;
			id->ext_max = cpuidbuf[0];
			if(cpuidbuf[0] >= 0x80000004){
				CPUID2(&s[0], 0x80000002);
				CPUID2(&s[16], 0x80000003);
				CPUID2(&s[32], 0x80000004);
				sprintf(id->brand_string, "%s", s);
			} else{
				sprintf(id->brand_string, "Null");
			}
		} else{
			id->ext_enable = false;
			id->ext_max = 0;
		}
	} else{
		id->enable = false;
		id->max = 0;
		sprintf(id->vendor, "Null");
	}
	return;
}
