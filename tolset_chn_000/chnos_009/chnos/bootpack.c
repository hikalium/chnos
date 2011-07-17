
#include "core.h"

uchar *ACPI_MemoryMap_Type[5] = {
	"  USABLE",
	"RESERVED",
	"    ACPI",
	"ACPI_NVS",
	"UNUSABLE"
};

void CHNMain(void)
{
	System_CommonData systemdata;
	UI_InputBox console;
	uint i, j, k;
	UI_Timer *c_timer;
	UI_Sheet *taskbar, *desktop;
	uint sendargs[8];
	UI_Task *mytask;
	uchar s[128];
	Memory *memblock;
	uint memusing;
	UI_Task *task;

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
	MultiTask_Task_Arguments(systemdata.keyctrltask, 1, &systemdata);
	MultiTask_Task_Run(systemdata.keyctrltask);

	desktop = System_Sheet_Get(systemdata.bootinfo->scrnx, systemdata.bootinfo->scrny, 0, 0);
//	Sheet_Show(desktop, 0, 0, System_Sheet_Get_Top_Of_Height());
	Sheet_Show(desktop, 0, 0, 0);
	Sheet_Draw_Fill_Rectangle(desktop, 0x66FF66, 0, 0, desktop->size.x - 1, desktop->size.y - 1);

//	InputBox_Initialise(&sys_sheet_ctrl, &sys_mem_ctrl, &console, 8, 16, systemdata.bootinfo->scrnx - 16, systemdata.bootinfo->scrny - 100, 1024, 0xFFFFFF, 0x868686, System_Sheet_Get_Top_Of_Height());
	InputBox_Initialise(&sys_sheet_ctrl, &sys_mem_ctrl, &console, 8, 16, systemdata.bootinfo->scrnx - 16, systemdata.bootinfo->scrny - 100, 1024, 0xFFFFFF, 0x868686, 1);

	taskbar = System_Sheet_Get(systemdata.bootinfo->scrnx, 32, 0, 0);
//	Sheet_Show(taskbar, 0, systemdata.bootinfo->scrny - 32, System_Sheet_Get_Top_Of_Height());
	Sheet_Show(taskbar, 0, systemdata.bootinfo->scrny - 32, 2);
	Sheet_Draw_Fill_Rectangle(taskbar, 0x6666FF, 0, 0, taskbar->size.x - 1, taskbar->size.y - 1);
	Sheet_Draw_Put_String(taskbar, 0, 0, 0xFFFFFF, "Taskbar");

	InputBox_Put_String(&console, "Welcome to CHNOSProject.\n");
	InputBox_Put_String(&console, "Press [Shift + F10] to show System Console.\n");

	c_timer = Timer_Get(&systemdata.sysfifo, 5);
	Timer_Set(c_timer, 50, interval);
	Timer_Run(c_timer);

	InputBox_NewLine(&console);
	InputBox_Reset_Input_Buffer(&console);

	sendargs[0] = 0x01;
	sendargs[1] = (uint)&systemdata.sysfifo;
	sendargs[2] = 0x00;
	sendargs[3] = DATA_BYTE;
	sendargs[4] = 0xFFFFFFFF;
	FIFO32_Put_Arguments(&systemdata.keyboardfifo, sendargs);

	FIFO32_Set_Task(&systemdata.sysfifo, mytask);

	for (;;) {
		IO_CLI();
		if(FIFO32_Status(&systemdata.sysfifo) == 0){
			MultiTask_Task_Sleep(mytask);
			IO_STI();
		} else{
			i = FIFO32_Get(&systemdata.sysfifo);
			if(i < DATA_BYTE){
				if(i == 5){
					InputBox_Change_Cursor_State(&console);
				}
			} else if(DATA_BYTE <= i && i < (DATA_BYTE * 2)){
				i -= DATA_BYTE;
				if(i == '\n'){
					if(console.input_buf[0] != 0x00){
						InputBox_NewLine_No_Prompt(&console);
					}
					if(strcmp(console.input_buf, "cls") == 0){
						InputBox_Clear(&console);
					} else if(strcmp(console.input_buf, "memmap") == 0){
						sprintf(s, "ACPI 0xe820 MemoryMaps:%d\n", systemdata.bootinfo->ACPI_MemoryMapEntries);
						InputBox_Put_String(&console, s);
						for(j = 0; j < systemdata.bootinfo->ACPI_MemoryMapEntries; j++){
							sprintf(s, "%02d:[0x%08X%08X](0x%08X%08X) %s 0x%08X\n", j, systemdata.bootinfo->ACPI_MemoryMap[j].Base.high, systemdata.bootinfo->ACPI_MemoryMap[j].Base.low, systemdata.bootinfo->ACPI_MemoryMap[j].Length.high, systemdata.bootinfo->ACPI_MemoryMap[j].Length.low, ACPI_MemoryMap_Type[systemdata.bootinfo->ACPI_MemoryMap[j].Type], systemdata.bootinfo->ACPI_MemoryMap[j].Attribute);
							InputBox_Put_String(&console, s);
						}
					} else if(strcmp(console.input_buf, "memblock") == 0){
						memusing = 0;
						sprintf(s, "MemoryBlocks:%d\n", SystemMemory.size);
						InputBox_Put_String(&console, s);
						for(memblock = &SystemMemory; memblock->next != 0; memblock = memblock->next){
							sprintf(s, "[0x%08X](%8u Bytes):%s\n", memblock->next->addr, memblock->next->size, memblock->next->description);
							InputBox_Put_String(&console, s);
							memusing += memblock->next->size;
						}
						sprintf(s, "MemoryUsing:%u Bytes + (%u Bytes * %u) = %u(%uM)Bytes.", memusing, sizeof(Memory), SystemMemory.size, memusing + (sizeof(Memory) * SystemMemory.size), (memusing + (sizeof(Memory) * SystemMemory.size)) >> 20);
						InputBox_Put_String(&console, s);
					} else if(strcmp(console.input_buf, "systeminfo") == 0){
						sprintf(s, "ACPI 0xe820 MemoryMaps:%d\n", systemdata.bootinfo->ACPI_MemoryMapEntries);
						InputBox_Put_String(&console, s);
						sprintf(s, "APM-Version:%X.%X\n", systemdata.bootinfo->APM_Version >> 8, systemdata.bootinfo->APM_Version & 0x00FF);
						InputBox_Put_String(&console, s);
						sprintf(s, "APM-Flags:0x%04X\n", systemdata.bootinfo->APM_Flags);
						InputBox_Put_String(&console, s);
						sprintf(s, "VESA-Version:%X.%X\n", systemdata.bootinfo->VESA_Version >> 8, systemdata.bootinfo->VESA_Version & 0x00FF);
						InputBox_Put_String(&console, s);
					} else if(strcmp(console.input_buf, "task") == 0){
						for(task = taskctrl->next; ; task = task->next){
							sprintf(s, "0x%04X:%s\n", task->selector, task->description);
							InputBox_Put_String(&console, s);
							if(task->next == 0){
								break;
							}
						}
					} else if(strcmp(console.input_buf, "gdt") == 0){
						for(j = 0; j < 8192; j++){
							if(SegmentDescriptor_Get_Limit(j) != 0){
								sprintf(s, "0x%04X:[0x%08X](0x%08X) ", j, SegmentDescriptor_Get_Base(j), SegmentDescriptor_Get_Limit(j));
								InputBox_Put_String(&console, s);
								k = SegmentDescriptor_Get_AccessRight(j);
								if((k & AR_CODE_OR_DATA) != 0){	/*code or data*/
									if((k & 0x08) != 0){	/*code*/
										InputBox_Put_String(&console, "Code Execute");
										if((k & 0x02) != 0){	/*Read*/
											InputBox_Put_String(&console, "/Read ");
										} else{
											InputBox_Put_String(&console, " Only ");
										}
										if((k & 0x04) != 0){	/*Read*/
											InputBox_Put_String(&console, "Conforming");
										}
									} else{	/*data*/
										InputBox_Put_String(&console, "Data Read");
										if((k & 0x02) != 0){	/*Read*/
											InputBox_Put_String(&console, "/Write ");
										} else{
											InputBox_Put_String(&console, " Only ");
										}
										if((k & 0x04) != 0){	/*Read*/
											InputBox_Put_String(&console, "Expand Down");
										}
									}
								} else{	/*SystemDescriptor*/
									if((k & 0x0f) == 0x02){	/*LDT*/
										InputBox_Put_String(&console, "LDT");
									} else if((k & 0x0f) == 0x05){	/*TaskGate*/
										InputBox_Put_String(&console, "TaskGate");
									} else{
										if((k & 0x07) == 0x01){
											InputBox_Put_String(&console, "TSS-Ready");
										} else if((k & 0x07) == 0x03){
											InputBox_Put_String(&console, "TSS-Busy");
										} else if((k & 0x07) == 0x04){
											InputBox_Put_String(&console, "CallGate");
										} else if((k & 0x07) == 0x06){
											InputBox_Put_String(&console, "INTGate");
										} else if((k & 0x07) == 0x07){
											InputBox_Put_String(&console, "TrapGate");
										}
										if((k & 0x08) != 0){	/*32bit*/
											InputBox_Put_String(&console, "(32bit)");
										} else{	/*16bit*/
											InputBox_Put_String(&console, "(16bit)");
										}
									}
								}
								InputBox_Put_String(&console, "\n");
							}
						}
					} else if(console.input_buf[0] != 0x00){
						InputBox_Put_String(&console, "Bad Command...");
					}
					InputBox_Reset_Input_Buffer(&console);
					InputBox_NewLine(&console);
				} else{
					InputBox_Put_Character(&console, i);
				}
			}
		}
	}
}

void CHNOS_KeyboardControlTask(System_CommonData *systemdata)
{
	UI_Task *mytask;
	UI_Listener *next, **now, *send;
	UI_KeyInfo kinfo;
	uint i, j;
	uint buf[5];

	mytask = MultiTask_Get_NowTask();
	next = 0;
	systemdata->keycmd_wait = 0;

	FIFO32_Set_Task(&systemdata->keyboardfifo, mytask);

	for (;;) {
		if(FIFO32_Status(&systemdata->keycmdfifo) > 0 && systemdata->keycmd_wait < 0){
			systemdata->keycmd_wait = FIFO32_Get(&systemdata->keycmdfifo);
			Keyboard_Controller_Wait_SendReady();
			IO_Out8(KEYB_DATA, systemdata->keycmd_wait);
		}
		IO_CLI();
		if(FIFO32_Status(&systemdata->keyboardfifo) == 0){
			MultiTask_Task_Sleep(mytask);
			IO_STI();
		} else{
			i = FIFO32_Get(&systemdata->keyboardfifo);
			if(i < DATA_BYTE){	/*タスクへのコマンド*/
				if(i == 1){	/*リスナー登録 [0x00000001][DATA_FIFO*][flags][offset][0xFFFFFFFF]*/
					buf[0] = 0x01;
					for(j = 1; j < 5; j++){
						buf[j] = FIFO32_Get(&systemdata->keyboardfifo);
					}
					if(buf[4] == 0xFFFFFFFF){
						for(now = &next; *now != 0; now = &(*now)->next){

						}
						*now = MemoryBlock_Allocate_System(sizeof(UI_Listener));
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
					} else{	/*特殊文字*/

					}
				}
			}
		}
	}
}
