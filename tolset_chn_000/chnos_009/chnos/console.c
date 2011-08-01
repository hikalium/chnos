
#include "core.h"

UI_Console *console_root;

System_CommonData *sysdata;

void Initialise_Console(UI_Console *consctrl, System_CommonData *systemdata)
{
	console_root = consctrl;
	console_root->win = 0;
	console_root->input = 0;
	console_root->next = 0;
	console_root->ctimer = 0;

	sysdata = systemdata;

	return;
}

UI_Console *Console_Create(uint xchars, uint ychars)
{
	UI_Console *cons, *end;
	UI_Task *cons_task;

	cons = MemoryBlock_Allocate_System(sizeof(UI_Console));
	MemoryBlock_Write_Description(cons, "UI_Console");

	cons->input = InputBox_Initialise(&sys_sheet_ctrl, &sys_mem_ctrl, 0, 0, xchars << 3, ychars << 4, 256, 0xffffff, 0x000000, Sheet_Get_Top_Of_Height(&sys_sheet_ctrl));
	cons->win = Window_Create_User("Console", 0, cons->input->sheet);

	for(end = console_root; end->next != 0; end = end->next){

	}
	cons->next = 0;
	end->next = cons;

	FIFO32_Initialise(&cons->fifo, CONSOLE_FIFO_SIZE);
	Sheet_Set_FIFO(cons->win->client, &cons->fifo, SIGNAL_FLAGS_FOCUSINFO);

	cons->ctimer = Timer_Get(&cons->fifo, 0x105);
	Timer_Set(cons->ctimer, 50, once);

	cons_task = MultiTask_Task_Get("ConsoleTask");
	cons_task->tss.ldtr = 0;
	cons_task->tss.iomap = 0x4000;
	cons_task->tss.eip = (uint)&Console_MainTask;
	cons_task->tss.eflags = 0x00000202;
	cons_task->tss.esp = (uint)MemoryBlock_Allocate_System(64 * 1024) + 64 * 1024;
	MemoryBlock_Write_Description((void *)(cons_task->tss.esp - 64 * 1024), "Console-Stack");
	cons_task->tss.es = 1 * 8;
	cons_task->tss.cs = 2 * 8;
	cons_task->tss.ss = 1 * 8;
	cons_task->tss.ds = 1 * 8;
	cons_task->tss.fs = 1 * 8;
	cons_task->tss.gs = 1 * 8;
	cons_task->tss.cr3 = (uint)ADR_Paging_Directory;
	MultiTask_Task_Arguments(cons_task, 1, cons);
	MultiTask_Task_Run(cons_task);

	FIFO32_Set_Task(&cons->fifo, cons_task);
	Timer_Run(cons->ctimer);

	CHNOS_UI_KeyFocus_Change(cons->win->client);

	return cons;
}

void Console_MainTask(UI_Console *cons)
{
	UI_Task *mytask;
	uint i, j;
	uint buf[3];
	bool clear_screen;

	mytask = MultiTask_Get_NowTask();
	clear_screen = false;

	InputBox_Put_Prompt(cons->input);

	for(;;){
		if(FIFO32_Status(&cons->fifo) == 0){
			MultiTask_Task_Sleep(mytask);
		} else{
			i = FIFO32_Get(&cons->fifo);
			if(i < DATA_BYTE){	/*キーボード文字データ*/
				if(i == '\n'){
					InputBox_Set_Record(cons->input, false);
					if(cons->input->input_buf[0] != 0x00){
						InputBox_NewLine_No_Prompt(cons->input);
					}
					if(strcmp(cons->input->input_buf, "cls") == 0){
						InputBox_Clear(cons->input);
						clear_screen = true;
					} else if(strcmp(cons->input->input_buf, "memmap") == 0){
						Console_Command_memmap(cons);
					} else if(strcmp(cons->input->input_buf, "memblock") == 0){
						Console_Command_memblock(cons);
					} else if(strcmp(cons->input->input_buf, "mem") == 0){
						Console_Command_mem(cons);
					} else if(strcmp(cons->input->input_buf, "systeminfo") == 0){
						Console_Command_systeminfo(cons);
					} else if(strcmp(cons->input->input_buf, "task") == 0){
						Console_Command_task(cons);
					} else if(strcmp(cons->input->input_buf, "window") == 0){
						Console_Command_window(cons);
					} else if(strcmp(cons->input->input_buf, "console") == 0){
						Console_Command_console(cons);
					} else if(strncmp(cons->input->input_buf, "type ", 5) == 0){
						Console_Command_type(cons, &cons->input->input_buf[5]);
					} else if(strcmp(cons->input->input_buf, "dir") == 0){
						Console_Command_dir(cons);
					} else if(strcmp(cons->input->input_buf, "gdt") == 0){
						Console_Command_gdt(cons);
					} else if(cons->input->input_buf[0] != 0x00){
						InputBox_Put_String(cons->input, "Bad Command...");
					}
					InputBox_Set_Record(cons->input, true);
					InputBox_Reset_Input_Buffer(cons->input);
					if(clear_screen){
						InputBox_Put_Prompt(cons->input);
						clear_screen = false;
					} else{
						InputBox_NewLine(cons->input);
					}
				} else{
					InputBox_Put_Character(cons->input, i);
				}
			} else if(i == 0x105){
				InputBox_Change_Cursor_State(cons->input);
				Timer_Run(cons->ctimer);
			} else if(i > SIGNAL_START){
				buf[0] = i;
				for(j = 1; j < 3; j++){
					buf[j] = 0x00;
				}
				for(j = 1; j < 3; j++){
					buf[j] = FIFO32_Get(&cons->fifo);
					if(buf[j] == SIGNAL_ARGUMENTS_END){
						break;
					}
				}
				if(i == SIGNAL_WINDOW_FOCUS_GET){
					if(buf[2] == SIGNAL_ARGUMENTS_END){	/*すべて正常に受信した*/
						Timer_Run(cons->ctimer);
					}
				} else if(i == SIGNAL_WINDOW_FOCUS_LOST){
					if(buf[2] == SIGNAL_ARGUMENTS_END){	/*すべて正常に受信した*/
						Timer_Cancel(cons->ctimer);
						if(cons->input->cursor_state){	/*プロンプトが表示されていたら消す*/
							InputBox_Change_Cursor_State(cons->input);
						}
					}
				}
			}
		}
	}
}

void Console_Command_memmap(UI_Console *cons)
{
	DATA_BootInfo *bootinfo = (DATA_BootInfo *)ADR_BOOTINFO;
	uchar s[64];
	uint i;

	sprintf(s, "ACPI 0xe820 MemoryMaps:%d\n", bootinfo->ACPI_MemoryMapEntries);
	InputBox_Put_String(cons->input, s);
	for(i = 0; i < bootinfo->ACPI_MemoryMapEntries; i++){
		sprintf(s, "%02d:[0x%08X%08X](0x%08X%08X) %s 0x%08X\n", i, bootinfo->ACPI_MemoryMap[i].Base.high, bootinfo->ACPI_MemoryMap[i].Base.low, bootinfo->ACPI_MemoryMap[i].Length.high, bootinfo->ACPI_MemoryMap[i].Length.low, ACPI_MemoryMap_Type[bootinfo->ACPI_MemoryMap[i].Type], bootinfo->ACPI_MemoryMap[i].Attribute);
		InputBox_Put_String(cons->input, s);
	}

	return;
}

void Console_Command_memblock(UI_Console *cons)
{
	uint memusing;
	uchar s[64];
	Memory *memblock;

	memusing = 0;
	sprintf(s, "MemoryBlocks:%d\n", SystemMemory.size);
	InputBox_Put_String(cons->input, s);
	for(memblock = &SystemMemory; memblock->next != 0; memblock = memblock->next){
		sprintf(s, "[0x%08X](%8u Bytes):%s\n", memblock->next->addr, memblock->next->size, memblock->next->description);
		InputBox_Put_String(cons->input, s);
		memusing += memblock->next->size;
	}
	sprintf(s, "MemoryUsing:%u Bytes + (%u Bytes * %u) = %u(%uM)Bytes.", memusing, sizeof(Memory), SystemMemory.size, memusing + (sizeof(Memory) * SystemMemory.size), (memusing + (sizeof(Memory) * SystemMemory.size)) >> 20);
	InputBox_Put_String(cons->input, s);

	return;
}

void Console_Command_mem(UI_Console *cons)
{
	uint memusing, i;
	uchar s[64];
	Memory *memblock;

	memusing = 0;
	sprintf(s, "Memory        :%uByte:%uMB\n", System_MemoryControl_FullSize(), System_MemoryControl_FullSize() >> 20);
	InputBox_Put_String(cons->input, s);
	sprintf(s, "Free          :%uByte:%uMB\n", System_MemoryControl_FreeSize(), System_MemoryControl_FreeSize() >> 20);
	InputBox_Put_String(cons->input, s);
	InputBox_Put_String(cons->input, "Using:\n");
	for(memblock = &SystemMemory; memblock->next != 0; memblock = memblock->next){
		memusing += memblock->next->size;
	}
	sprintf(s, "\tMemoryBlock        :%uByte:%uMB\n", memusing + (sizeof(Memory) * SystemMemory.size), (memusing + (sizeof(Memory) * SystemMemory.size)) >> 20);
	InputBox_Put_String(cons->input, s);
	memusing = 1;
	for(i = 0; i < 1024; i++){
		if((ADR_Paging_Directory[i] & PG_PRESENT) != 0){
			memusing++;
		}
	}
	sprintf(s, "\tPageDirectory+Table:%uByte:%uMB\n", i << 12, i >> 8);
	InputBox_Put_String(cons->input, s);

	return;
}

void Console_Command_systeminfo(UI_Console *cons)
{
	DATA_BootInfo *bootinfo = (DATA_BootInfo *)ADR_BOOTINFO;
	uchar s[64];

	sprintf(s, "ACPI 0xe820 MemoryMaps:%d\n", bootinfo->ACPI_MemoryMapEntries);
	InputBox_Put_String(cons->input, s);
	sprintf(s, "APM-Version:%X.%X\n", bootinfo->APM_Version >> 8, bootinfo->APM_Version & 0x00FF);
	InputBox_Put_String(cons->input, s);
	sprintf(s, "APM-Flags:0x%04X\n", bootinfo->APM_Flags);
	InputBox_Put_String(cons->input, s);
	sprintf(s, "VESA-Version:%X.%X\n", bootinfo->VESA_Version >> 8, bootinfo->VESA_Version & 0x00FF);
	InputBox_Put_String(cons->input, s);

	return;
}

void Console_Command_task(UI_Console *cons)
{
	UI_Task *task;
	uchar s[64];

	for(task = taskctrl->next; task != 0; task = task->next){
		sprintf(s, "0x%04X (%10u):%s\n", task->selector, task->cputime, task->description);
		InputBox_Put_String(cons->input, s);
	}

	return;
}

void Console_Command_window(UI_Console *cons)
{
	UI_Window *win;
	uchar s[64];

	for(win = sysdata->windowctrl.next; win != 0; win = win->next){
		sprintf(s, "%s\n", win->title);
		InputBox_Put_String(cons->input, s);
	}

	return;
}

void Console_Command_console(UI_Console *cons)
{
	DATA_BootInfo *bootinfo = (DATA_BootInfo *)ADR_BOOTINFO;

	Console_Create((bootinfo->scrnx >> 4), (bootinfo->scrny >> 5));
	return;
}

void Console_Command_type(UI_Console *cons, const uchar filename[])
{
	IO_File file;
	int n;
	uint i;

	n = FloppyDisk_Search_File(sysdata->fd_boot, filename);
	if(n != -1){
		n = FloppyDisk_Load_File(sysdata->fd_boot, &file, n);
		if(n != -1){
			for(i = 0; i < file.size; i++){
				InputBox_Put_Character(cons->input, file.data[i]);
			}
		} else{
			InputBox_Put_String(cons->input, "type:File load error.\n");
		}
		File_Free(&file);
	} else{
		InputBox_Put_String(cons->input, "type:File not found.\n");
	}

	return;
}

void Console_Command_dir(UI_Console *cons)
{
	uint i, j;
	uchar s[64];

	for(i = 0; i < 224; i++){
		if(sysdata->fd_boot->files[i].name[0] == 0x00){
			break;
		}
		if(sysdata->fd_boot->files[i].name[0] != 0xe5){
			sprintf(s, "FILENAME.EXT %7d %04d/%02d/%02d %02d:%02d:%02d\n", sysdata->fd_boot->files[i].size, (sysdata->fd_boot->files[i].updatedate >> 9) + 1980, (sysdata->fd_boot->files[i].updatedate & 0x01e0) >> 5, sysdata->fd_boot->files[i].updatedate & 0x001f, sysdata->fd_boot->files[i].updatetime >> 11, (sysdata->fd_boot->files[i].updatetime & 0x07e0) >> 5, sysdata->fd_boot->files[i].updatetime & 0x1f);
			for(j = 0; j < 8; j++){
				s[j] = sysdata->fd_boot->files[i].name[j];
			}
			for(j = 9; j < 12; j++){
				s[j] = sysdata->fd_boot->files[i].name[j - 1];
			}
			InputBox_Put_String(cons->input, s);
		}
	}

	return;
}

void Console_Command_gdt(UI_Console *cons)
{
	uint i, ar;
	uchar s[64];

	for(i = 0; i < 8192; i++){
		if(SegmentDescriptor_Get_Limit(i) != 0){
			sprintf(s, "0x%04X:[0x%08X](0x%08X) ", i, SegmentDescriptor_Get_Base(i), SegmentDescriptor_Get_Limit(i));
			InputBox_Put_String(cons->input, s);
			ar = SegmentDescriptor_Get_AccessRight(i);
			if((ar & AR_CODE_OR_DATA) != 0){	/*code or data*/
				if((ar & 0x08) != 0){	/*code*/
					InputBox_Put_String(cons->input, "Code Execute");
					if((ar & 0x02) != 0){	/*Read*/
						InputBox_Put_String(cons->input, "/Read ");
					} else{
						InputBox_Put_String(cons->input, " Only ");
					}
					if((ar & 0x04) != 0){	/*Read*/
						InputBox_Put_String(cons->input, "Conforming");
					}
				} else{	/*data*/
					InputBox_Put_String(cons->input, "Data Read");
					if((ar & 0x02) != 0){	/*Read*/
						InputBox_Put_String(cons->input, "/Write ");
					} else{
						InputBox_Put_String(cons->input, " Only ");
					}
					if((ar & 0x04) != 0){	/*Read*/
						InputBox_Put_String(cons->input, "Expand Down");
					}
				}
			} else{	/*SystemDescriptor*/
				if((ar & 0x0f) == 0x02){	/*LDT*/
					InputBox_Put_String(cons->input, "LDT");
				} else if((ar & 0x0f) == 0x05){	/*TaskGate*/
					InputBox_Put_String(cons->input, "TaskGate");
				} else{
					if((ar & 0x07) == 0x01){
						InputBox_Put_String(cons->input, "TSS-Ready");
					} else if((ar & 0x07) == 0x03){
						InputBox_Put_String(cons->input, "TSS-Busy");
					} else if((ar & 0x07) == 0x04){
						InputBox_Put_String(cons->input, "CallGate");
					} else if((ar & 0x07) == 0x06){
						InputBox_Put_String(cons->input, "INTGate");
					} else if((ar & 0x07) == 0x07){
						InputBox_Put_String(cons->input, "TrapGate");
					}
					if((ar & 0x08) != 0){	/*32bit*/
						InputBox_Put_String(cons->input, "(32bit)");
					} else{	/*16bit*/
						InputBox_Put_String(cons->input, "(16bit)");
					}
				}
			}
			InputBox_Put_String(cons->input, "\n");
		}
	}
}
