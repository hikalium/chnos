
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
	Sheet_Set_FIFO(cons->win->client, &cons->fifo);

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

	return cons;
}

void Console_MainTask(UI_Console *cons)
{
	UI_Task *mytask;
	uint i;
	bool c_state;

	mytask = MultiTask_Get_NowTask();
	c_state = false;

	InputBox_Put_Prompt(cons->input);

	for(;;){
		if(FIFO32_Status(&cons->fifo) == 0){
			MultiTask_Task_Sleep(mytask);
		} else{
			i = FIFO32_Get(&cons->fifo);
			if(i < DATA_BYTE){	/*キーボード文字データ*/
				InputBox_Put_Character(cons->input, i);
			} else if(i == 0x105){
				InputBox_Change_Cursor_State(cons->input);
				if(c_state){
					c_state = false;
				} else{
					c_state = true;
				}
				Timer_Run(cons->ctimer);
			}
		}
	}
}
