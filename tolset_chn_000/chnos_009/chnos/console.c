
#include "core.h"

UI_Console *console_root;

void Initialise_Console(UI_Console *consctrl)
{
	console_root = consctrl;
	console_root->win = 0;
	console_root->input = 0;
	console_root->next = 0;
	console_root->ctimer = 0;
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
