
#include "core.h"

uchar *ACPI_MemoryMap_Type[5] = {
	"  USABLE",
	"RESERVED",
	"    ACPI",
	"ACPI_NVS",
	"UNUSABLE"
};

System_CommonData systemdata;

void CHNMain(void)
{
	UI_InputBox *console;
	uint i, j, k;
	int n;
	UI_Timer *c_timer;
	UI_Sheet *taskbar, *desktop;
	UI_Task *mytask;
	uchar s[128];
	Memory *memblock;
	uint memusing;
	UI_Task *task;
	IO_FloppyDisk *fd;
	IO_File type_file;
	UI_Window *win;
	bool clear_screen;

	IO_CLI();

	Initialise_System(&systemdata);

	IO_STI();

	mytask = MultiTask_Get_NowTask();
	clear_screen = false;

	systemdata.keyctrltask = MultiTask_Task_Get("SysKeyCtrlTask");
	systemdata.keyctrltask->tss.ldtr = 0;
	systemdata.keyctrltask->tss.iomap = 0x4000;
	systemdata.keyctrltask->tss.eip = (uint)&CHNOS_KeyboardControlTask;
	systemdata.keyctrltask->tss.eflags = 0x00000202;
	systemdata.keyctrltask->tss.esp = (uint)MemoryBlock_Allocate_System(64 * 1024) + 64 * 1024;
	MemoryBlock_Write_Description((void *)(systemdata.keyctrltask->tss.esp - 64 * 1024), "SysKCT-Stack");
	systemdata.keyctrltask->tss.es = 1 * 8;
	systemdata.keyctrltask->tss.cs = 2 * 8;
	systemdata.keyctrltask->tss.ss = 1 * 8;
	systemdata.keyctrltask->tss.ds = 1 * 8;
	systemdata.keyctrltask->tss.fs = 1 * 8;
	systemdata.keyctrltask->tss.gs = 1 * 8;
	systemdata.keyctrltask->tss.cr3 = (uint)ADR_Paging_Directory;
	MultiTask_Task_Run(systemdata.keyctrltask);

	Mouse_Make_MouseCursor(&systemdata.mouse_cursor, 0, 0, systemdata.bootinfo->scrnx - 1, systemdata.bootinfo->scrny - 1, System_Sheet_Get_Top_Of_Height());
	Mouse_Move_Absolute(&systemdata.mouse_cursor, systemdata.bootinfo->scrnx >> 1, systemdata.bootinfo->scrny >> 1);

	systemdata.mousectrltask = MultiTask_Task_Get("MouseCtrlTask");
	systemdata.mousectrltask->tss.ldtr = 0;
	systemdata.mousectrltask->tss.iomap = 0x4000;
	systemdata.mousectrltask->tss.eip = (uint)&CHNOS_MouseControlTask;
	systemdata.mousectrltask->tss.eflags = 0x00000202;
	systemdata.mousectrltask->tss.esp = (uint)MemoryBlock_Allocate_System(64 * 1024) + 64 * 1024;
	MemoryBlock_Write_Description((void *)(systemdata.mousectrltask->tss.esp - 64 * 1024), "SysMCT-Stack");
	systemdata.mousectrltask->tss.es = 1 * 8;
	systemdata.mousectrltask->tss.cs = 2 * 8;
	systemdata.mousectrltask->tss.ss = 1 * 8;
	systemdata.mousectrltask->tss.ds = 1 * 8;
	systemdata.mousectrltask->tss.fs = 1 * 8;
	systemdata.mousectrltask->tss.gs = 1 * 8;
	systemdata.mousectrltask->tss.cr3 = (uint)ADR_Paging_Directory;
	MultiTask_Task_Run(systemdata.mousectrltask);

	desktop = System_Sheet_Get(systemdata.bootinfo->scrnx, systemdata.bootinfo->scrny, 0, 0);
	Sheet_Show(desktop, 0, 0, System_Sheet_Get_Top_Of_Height());
	Sheet_Draw_Fill_Rectangle(desktop, 0x66FF66, 0, 0, desktop->size.x - 1, desktop->size.y - 1);

	console = InputBox_Initialise(&sys_sheet_ctrl, &sys_mem_ctrl, 8, 16, systemdata.bootinfo->scrnx - 16, systemdata.bootinfo->scrny - 100, 1024, 0xFFFFFF, 0x868686, System_Sheet_Get_Top_Of_Height());
	Sheet_Set_Movable(console->sheet, true);

	taskbar = System_Sheet_Get(systemdata.bootinfo->scrnx, 32, 0, 0);
	Sheet_Show(taskbar, 0, systemdata.bootinfo->scrny - 32, System_Sheet_Get_Top_Of_Height());
	Sheet_Draw_Fill_Rectangle(taskbar, 0x6666FF, 0, 0, taskbar->size.x - 1, taskbar->size.y - 1);
	Sheet_Draw_Put_String(taskbar, 0, 0, 0xFFFFFF, "Taskbar");

	InputBox_Put_String(console, "Welcome to CHNOSProject.");

	c_timer = Timer_Get(&systemdata.sysfifo, 5);
	Timer_Set(c_timer, 50, interval);
	Timer_Run(c_timer);

	InputBox_NewLine(console);
	InputBox_Reset_Input_Buffer(console);

	FIFO32_Put_Arguments(&systemdata.keyboardfifo, 4, 0x01, (uint)&systemdata.sysfifo, 0x00, DATA_BYTE);

	FIFO32_Set_Task(&systemdata.sysfifo, mytask);

	fd = FloppyDisk_Initialise((uint *)ADR_DISKIMG);

	for (;;) {
		if(FIFO32_Status(&systemdata.sysfifo) == 0){
			MultiTask_Task_Sleep(mytask);
		} else{
			i = FIFO32_Get(&systemdata.sysfifo);
			if(i < DATA_BYTE){
				if(i == 5){
					InputBox_Change_Cursor_State(console);
				}
			} else if(DATA_BYTE <= i && i < (DATA_BYTE * 2)){
				i -= DATA_BYTE;
				if(i == '\n'){
					InputBox_Set_Record(console, false);
					if(console->input_buf[0] != 0x00){
						InputBox_NewLine_No_Prompt(console);
					}
					if(strcmp(console->input_buf, "cls") == 0){
						InputBox_Clear(console);
						clear_screen = true;
					} else if(strcmp(console->input_buf, "memmap") == 0){
						sprintf(s, "ACPI 0xe820 MemoryMaps:%d\n", systemdata.bootinfo->ACPI_MemoryMapEntries);
						InputBox_Put_String(console, s);
						for(j = 0; j < systemdata.bootinfo->ACPI_MemoryMapEntries; j++){
							sprintf(s, "%02d:[0x%08X%08X](0x%08X%08X) %s 0x%08X\n", j, systemdata.bootinfo->ACPI_MemoryMap[j].Base.high, systemdata.bootinfo->ACPI_MemoryMap[j].Base.low, systemdata.bootinfo->ACPI_MemoryMap[j].Length.high, systemdata.bootinfo->ACPI_MemoryMap[j].Length.low, ACPI_MemoryMap_Type[systemdata.bootinfo->ACPI_MemoryMap[j].Type], systemdata.bootinfo->ACPI_MemoryMap[j].Attribute);
							InputBox_Put_String(console, s);
						}
					} else if(strcmp(console->input_buf, "memblock") == 0){
						memusing = 0;
						sprintf(s, "MemoryBlocks:%d\n", SystemMemory.size);
						InputBox_Put_String(console, s);
						for(memblock = &SystemMemory; memblock->next != 0; memblock = memblock->next){
							sprintf(s, "[0x%08X](%8u Bytes):%s\n", memblock->next->addr, memblock->next->size, memblock->next->description);
							InputBox_Put_String(console, s);
							memusing += memblock->next->size;
						}
						sprintf(s, "MemoryUsing:%u Bytes + (%u Bytes * %u) = %u(%uM)Bytes.", memusing, sizeof(Memory), SystemMemory.size, memusing + (sizeof(Memory) * SystemMemory.size), (memusing + (sizeof(Memory) * SystemMemory.size)) >> 20);
						InputBox_Put_String(console, s);
					} else if(strcmp(console->input_buf, "mem") == 0){
						memusing = 0;
						sprintf(s, "Memory        :%uByte:%uMB\n", System_MemoryControl_FullSize(), System_MemoryControl_FullSize() >> 20);
						InputBox_Put_String(console, s);
						sprintf(s, "Free          :%uByte:%uMB\n", System_MemoryControl_FreeSize(), System_MemoryControl_FreeSize() >> 20);
						InputBox_Put_String(console, s);
						InputBox_Put_String(console, "Using:\n");
						for(memblock = &SystemMemory; memblock->next != 0; memblock = memblock->next){
							memusing += memblock->next->size;
						}
						sprintf(s, "\tMemoryBlock        :%uByte:%uMB\n", memusing + (sizeof(Memory) * SystemMemory.size), (memusing + (sizeof(Memory) * SystemMemory.size)) >> 20);
						InputBox_Put_String(console, s);
						memusing = 1;
						for(j = 0; j < 1024; j++){
							if((ADR_Paging_Directory[j] & PG_PRESENT) != 0){
								memusing++;
							}
						}
						sprintf(s, "\tPageDirectory+Table:%uByte:%uMB\n", j << 12, j >> 8);
						InputBox_Put_String(console, s);
					} else if(strcmp(console->input_buf, "systeminfo") == 0){
						sprintf(s, "ACPI 0xe820 MemoryMaps:%d\n", systemdata.bootinfo->ACPI_MemoryMapEntries);
						InputBox_Put_String(console, s);
						sprintf(s, "APM-Version:%X.%X\n", systemdata.bootinfo->APM_Version >> 8, systemdata.bootinfo->APM_Version & 0x00FF);
						InputBox_Put_String(console, s);
						sprintf(s, "APM-Flags:0x%04X\n", systemdata.bootinfo->APM_Flags);
						InputBox_Put_String(console, s);
						sprintf(s, "VESA-Version:%X.%X\n", systemdata.bootinfo->VESA_Version >> 8, systemdata.bootinfo->VESA_Version & 0x00FF);
						InputBox_Put_String(console, s);
					} else if(strcmp(console->input_buf, "task") == 0){
						for(task = taskctrl->next; task != 0; task = task->next){
							sprintf(s, "0x%04X (%10u):%s\n", task->selector, task->cputime, task->description);
							InputBox_Put_String(console, s);
						}
					} else if(strcmp(console->input_buf, "window") == 0){
						for(win = systemdata.windowctrl.next; win != 0; win = win->next){
							sprintf(s, "%s\n", win->title);
							InputBox_Put_String(console, s);
						}
					} else if(strcmp(console->input_buf, "testcons") == 0){
						Console_Create((systemdata.bootinfo->scrnx >> 4), (systemdata.bootinfo->scrny >> 5));
					} else if(strncmp(console->input_buf, "type ", 5) == 0){
						n = FloppyDisk_Search_File(fd, &console->input_buf[5]);
						if(n != -1){
							n = FloppyDisk_Load_File(fd, &type_file, n);
							if(n != -1){
								for(k = 0; k < type_file.size; k++){
									InputBox_Put_Character(console, type_file.data[k]);
								}
							} else{
								InputBox_Put_String(console, "type:File load error.\n");
							}
							File_Free(&type_file);
						} else{
							InputBox_Put_String(console, "type:File not found.\n");
						}
					} else if(strcmp(console->input_buf, "dir") == 0){
						for(j = 0; j < 224; j++){
							if(fd->files[j].name[0] == 0x00){
								break;
							}
							if(fd->files[j].name[0] != 0xe5){
								sprintf(s, "FILENAME.EXT %7d %04d/%02d/%02d %02d:%02d:%02d\n", fd->files[j].size, (fd->files[j].updatedate >> 9) + 1980, (fd->files[j].updatedate & 0x01e0) >> 5, fd->files[j].updatedate & 0x001f, fd->files[j].updatetime >> 11, (fd->files[j].updatetime & 0x07e0) >> 5, fd->files[j].updatetime & 0x1f);
								for(k = 0; k < 8; k++){
									s[k] = fd->files[j].name[k];
								}
								for(k = 9; k < 12; k++){
									s[k] = fd->files[j].name[k - 1];
								}
								InputBox_Put_String(console, s);
							}
						}
					} else if(strcmp(console->input_buf, "gdt") == 0){
						for(j = 0; j < 8192; j++){
							if(SegmentDescriptor_Get_Limit(j) != 0){
								sprintf(s, "0x%04X:[0x%08X](0x%08X) ", j, SegmentDescriptor_Get_Base(j), SegmentDescriptor_Get_Limit(j));
								InputBox_Put_String(console, s);
								k = SegmentDescriptor_Get_AccessRight(j);
								if((k & AR_CODE_OR_DATA) != 0){	/*code or data*/
									if((k & 0x08) != 0){	/*code*/
										InputBox_Put_String(console, "Code Execute");
										if((k & 0x02) != 0){	/*Read*/
											InputBox_Put_String(console, "/Read ");
										} else{
											InputBox_Put_String(console, " Only ");
										}
										if((k & 0x04) != 0){	/*Read*/
											InputBox_Put_String(console, "Conforming");
										}
									} else{	/*data*/
										InputBox_Put_String(console, "Data Read");
										if((k & 0x02) != 0){	/*Read*/
											InputBox_Put_String(console, "/Write ");
										} else{
											InputBox_Put_String(console, " Only ");
										}
										if((k & 0x04) != 0){	/*Read*/
											InputBox_Put_String(console, "Expand Down");
										}
									}
								} else{	/*SystemDescriptor*/
									if((k & 0x0f) == 0x02){	/*LDT*/
										InputBox_Put_String(console, "LDT");
									} else if((k & 0x0f) == 0x05){	/*TaskGate*/
										InputBox_Put_String(console, "TaskGate");
									} else{
										if((k & 0x07) == 0x01){
											InputBox_Put_String(console, "TSS-Ready");
										} else if((k & 0x07) == 0x03){
											InputBox_Put_String(console, "TSS-Busy");
										} else if((k & 0x07) == 0x04){
											InputBox_Put_String(console, "CallGate");
										} else if((k & 0x07) == 0x06){
											InputBox_Put_String(console, "INTGate");
										} else if((k & 0x07) == 0x07){
											InputBox_Put_String(console, "TrapGate");
										}
										if((k & 0x08) != 0){	/*32bit*/
											InputBox_Put_String(console, "(32bit)");
										} else{	/*16bit*/
											InputBox_Put_String(console, "(16bit)");
										}
									}
								}
								InputBox_Put_String(console, "\n");
							}
						}
					} else if(console->input_buf[0] != 0x00){
						InputBox_Put_String(console, "Bad Command...");
					}
					InputBox_Set_Record(console, true);
					InputBox_Reset_Input_Buffer(console);
					if(clear_screen){
						InputBox_Put_Prompt(console);
						clear_screen = false;
					} else{
						InputBox_NewLine(console);
					}
				} else{
					InputBox_Put_Character(console, i);
				}
			}
		}
	}
}

void CHNOS_KeyboardControlTask(void)
{
	UI_Task *mytask;
	UI_Listener *next, **now, *send;
	UI_KeyInfo kinfo;
	uint i, j;
	uint buf[5];

	mytask = MultiTask_Get_NowTask();
	next = 0;
	systemdata.keycmd_wait = 0;

	FIFO32_Set_Task(&systemdata.keyboardfifo, mytask);

	for (;;) {
		if(FIFO32_Status(&systemdata.keycmdfifo) > 0 && systemdata.keycmd_wait < 0){
			systemdata.keycmd_wait = FIFO32_Get(&systemdata.keycmdfifo);
			Keyboard_Controller_Wait_SendReady();
			IO_Out8(KEYB_DATA, systemdata.keycmd_wait);
		}
		if(FIFO32_Status(&systemdata.keyboardfifo) == 0){
			MultiTask_Task_Sleep(mytask);
		} else{
			i = FIFO32_Get(&systemdata.keyboardfifo);
			if(i < DATA_BYTE){	/*タスクへのコマンド*/
				if(i == 1){	/*リスナー登録 FIFO32_Put_Arguments([fifo], 4, 0x01, [DATA_FIFO*], [flags], [offset]); ([0xFFFFFFFF])*/
					buf[0] = 0x01;
					buf[4] = 0x00;
					for(j = 1; j < 5; j++){
						buf[j] = FIFO32_Get(&systemdata.keyboardfifo);
						if(buf[j] == SIGNAL_ARGUMENTS_END){
							break;
						}
					}
					if(buf[4] == SIGNAL_ARGUMENTS_END){	/*すべて正常に受信した。*/
						for(now = &next; *now != 0; now = &(*now)->next){

						}
						*now = MemoryBlock_Allocate_System(sizeof(UI_Listener));
						MemoryBlock_Write_Description(*now, "UI_Listener");
						(*now)->next = 0;
						(*now)->fifo = (DATA_FIFO *)buf[1];
						(*now)->flags = buf[2];
						(*now)->offset = buf[3];
					}
				}
			} else if(DATA_BYTE <= i && i < (DATA_BYTE * 2)){	/*キーボードからの受信データ*/
				Keyboard_Decode(&kinfo, i - DATA_BYTE);
				if(kinfo.make){
					if(kinfo.c != 0){	/*一般文字*/
						for(send = next; send != 0; send = send->next){
							FIFO32_Put(send->fifo, kinfo.c + send->offset);
						}
						if(systemdata.key_focus != 0 && systemdata.key_focus->fifo != 0){	/*フォーカスはキーデータの取得を希望している*/
							FIFO32_Put(systemdata.key_focus->fifo, kinfo.c);
						}
					} else{	/*特殊文字*/

					}
				}
			}
		}
	}
}

void CHNOS_MouseControlTask(void)
{
	UI_Task *mytask;
	UI_Sheet *focus;
	DATA_Position2D focus_moveorg;
	uint i;
	int button_before;
	bool key_focus_changed;
	UI_Sheet *key_focus_before;

	UI_MouseEventArguments e;

	mytask = MultiTask_Get_NowTask();
	focus = 0;
	button_before = 0;
	key_focus_changed = false;
	key_focus_before = 0;

	FIFO32_Set_Task(&systemdata.mousefifo, mytask);

	for (;;) {
		if(FIFO32_Status(&systemdata.mousefifo) == 0){
			if(focus != 0){
				Sheet_Slide(focus, focus->position.x + (systemdata.mouse_cursor.position.x - focus_moveorg.x), focus->position.y + (systemdata.mouse_cursor.position.y - focus_moveorg.y));
				focus_moveorg.x = systemdata.mouse_cursor.position.x;
				focus_moveorg.y = systemdata.mouse_cursor.position.y;
			}
			MultiTask_Task_Sleep(mytask);
		} else{
			i = FIFO32_Get(&systemdata.mousefifo);
			if(i < DATA_BYTE){	/*タスクへのコマンド*/

			} else if(DATA_BYTE <= i && i < (DATA_BYTE * 2)){	/*マウスからの受信データ*/
				if(Mouse_Decode(i - DATA_BYTE) != 0){
					Mouse_Move_Relative(&systemdata.mouse_cursor, systemdata.mousedecode.move.x, systemdata.mousedecode.move.y);
					systemdata.focus = Sheet_Get_From_Position(&sys_sheet_ctrl, systemdata.mouse_cursor.position.x, systemdata.mouse_cursor.position.y);
					if((systemdata.mousedecode.btn & MOUSE_BUTTON_L) != 0 && (button_before & MOUSE_BUTTON_L) == 0){	/*L down*/
						if(systemdata.key_focus != systemdata.focus){
							key_focus_changed = true;
							key_focus_before = systemdata.key_focus;
						}
						systemdata.key_focus = systemdata.focus;
						focus = systemdata.focus;
						focus_moveorg.x = systemdata.mouse_cursor.position.x;
						focus_moveorg.y = systemdata.mouse_cursor.position.y;
						if(!focus->mouse_movable){	/*マウスによる移動不可*/
							focus = 0;
						} else{
							Sheet_UpDown(focus, System_Sheet_Get_Top_Of_Height());
						}
					} else if((systemdata.mousedecode.btn & MOUSE_BUTTON_L) == 0 && (button_before & MOUSE_BUTTON_L) != 0){	/*L up*/
						focus = 0;
					}
					if((systemdata.mousedecode.btn & MOUSE_BUTTON_R) != 0){	/*R*/

					}
					if((systemdata.mousedecode.btn & MOUSE_BUTTON_C) != 0){	/*C*/

					}
					if(systemdata.focus != 0 && systemdata.focus->MouseEventProcedure != 0){	/*フォーカスはシートで、イベントの取得を希望している*/
						e.focus = systemdata.focus;
						e.move.x = systemdata.mousedecode.move.x;
						e.move.y = systemdata.mousedecode.move.y;
						e.position_local.x = systemdata.mouse_cursor.position.x - focus->position.x;
						e.position_local.y = systemdata.mouse_cursor.position.y - focus->position.y;
						e.button = (uint)systemdata.mousedecode.btn;
						e.button_before = (uint)button_before;
						systemdata.focus->MouseEventProcedure(&e);
					}
					if(key_focus_changed){
						if(key_focus_before != 0 && key_focus_before->fifo != 0 && (key_focus_before->ksignal_flags & SIGNAL_FLAGS_FOCUSINFO) != 0){
							FIFO32_Put_Arguments(key_focus_before->fifo, 2, SIGNAL_WINDOW_FOCUS_LOST, (uint)key_focus_before);
						}
						if(systemdata.key_focus != 0 && systemdata.key_focus->fifo != 0 && (systemdata.key_focus->ksignal_flags & SIGNAL_FLAGS_FOCUSINFO) != 0){
							FIFO32_Put_Arguments(systemdata.key_focus->fifo, 2, SIGNAL_WINDOW_FOCUS_GET, (uint)systemdata.key_focus);
						}
						key_focus_changed = false;
					}
					key_focus_before = systemdata.key_focus;
					button_before = systemdata.mousedecode.btn;
				}
			}
		}
	}
}

void CHNOS_UI_KeyFocus_Change(UI_Sheet *focus_new)
{
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	if(systemdata.key_focus != 0 && systemdata.key_focus->fifo != 0 && (systemdata.key_focus->ksignal_flags & SIGNAL_FLAGS_FOCUSINFO) != 0){
		FIFO32_Put_Arguments(systemdata.key_focus->fifo, 2, SIGNAL_WINDOW_FOCUS_LOST, systemdata.key_focus);
	}
	if(focus_new != 0 && focus_new->fifo != 0 && (focus_new->ksignal_flags & SIGNAL_FLAGS_FOCUSINFO) != 0){
		FIFO32_Put_Arguments(focus_new->fifo, 2, SIGNAL_WINDOW_FOCUS_GET, (uint)focus_new);
	}
	systemdata.key_focus = focus_new;

	IO_Store_EFlags(eflags);
	return;
}
