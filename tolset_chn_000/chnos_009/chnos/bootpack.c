
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
	uint i;
	UI_Timer *c_timer;
	UI_Sheet *taskbar, *desktop;
	UI_Task *mytask;
	uchar s[128];

	IO_CLI();

	Initialise_System(&systemdata);

	IO_STI();

	mytask = MultiTask_Get_NowTask();

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
	Sheet_Draw_Fill_Rectangle(desktop, 0x66ff66, 0, 0, desktop->size.x - 1, desktop->size.y - 1);

	taskbar = System_Sheet_Get(systemdata.bootinfo->scrnx, 32, 0, 0);
	Sheet_Show(taskbar, 0, systemdata.bootinfo->scrny - 32, System_Sheet_Get_Top_Of_Height());
	Sheet_Draw_Fill_Rectangle_Gradation_Vertical(taskbar, 0xffffff, 0x6666ff, 0, 0, taskbar->size.x - 1, taskbar->size.y - 1);

	c_timer = Timer_Get(&systemdata.sysfifo, 5);
	Timer_Set(c_timer, 50, interval);
	Timer_Run(c_timer);

	FIFO32_Set_Task(&systemdata.sysfifo, mytask);

	Console_Create((systemdata.bootinfo->scrnx >> 4), (systemdata.bootinfo->scrny >> 5));

	for (;;) {
		if(FIFO32_Status(&systemdata.sysfifo) == 0){
			MultiTask_Task_Sleep(mytask);
		} else{
			i = FIFO32_Get(&systemdata.sysfifo);
			if(i < DATA_BYTE){
				if(i == 5){

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
					if(focus == 0){
						if(systemdata.focus != Sheet_Get_From_Position(&sys_sheet_ctrl, systemdata.mouse_cursor.position.x, systemdata.mouse_cursor.position.y)){
							if(systemdata.focus != 0 && systemdata.focus->MouseEventProcedure != 0){
								e.focus = systemdata.focus;
								e.move.x = 0;
								e.move.y = 0;
								e.button = 0;
								e.button_before = (uint)button_before;
								systemdata.focus->MouseEventProcedure(&e);
							}
						}
						systemdata.focus = Sheet_Get_From_Position(&sys_sheet_ctrl, systemdata.mouse_cursor.position.x, systemdata.mouse_cursor.position.y);
					}
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
					if(key_focus_changed){
						if(key_focus_before != 0 && key_focus_before->fifo != 0 && (key_focus_before->ksignal_flags & SIGNAL_FLAGS_FOCUSINFO) != 0){
							FIFO32_Put_Arguments(key_focus_before->fifo, 2, SIGNAL_WINDOW_FOCUS_LOST, (uint)key_focus_before);
						}
						if(systemdata.key_focus != 0 && systemdata.key_focus->fifo != 0 && (systemdata.key_focus->ksignal_flags & SIGNAL_FLAGS_FOCUSINFO) != 0){
							FIFO32_Put_Arguments(systemdata.key_focus->fifo, 2, SIGNAL_WINDOW_FOCUS_GET, (uint)systemdata.key_focus);
						}
						key_focus_changed = false;
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
