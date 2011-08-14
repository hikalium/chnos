
#include "core.h"

UI_Console *console_root;

System_CommonData *sysdata;

uchar *MIDI_Notes[12] = {
	"C ",
	"C#",
	"D ",
	"D#",
	"E ",
	"F ",
	"F#",
	"G ",
	"G#",
	"A ",
	"A#",
	"B "
};

ushort ToneTable[12] = {
	262,
	277,
	294,
	311,
	330,
	349,
	370,
	392,
	415,
	440,
	466,
	494
};

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
	uchar s[64];

	mytask = MultiTask_Get_NowTask();
	mytask->cons = cons;
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
					} else if(strcmp(cons->input->input_buf, "test") == 0){
						sprintf(s, "taskaddr:0x%08X\n", mytask);
						InputBox_Put_String(cons->input, s);
						IO_Beep(440, 5000000);
					} else if(strncmp(cons->input->input_buf, "midi ", 5) == 0){
						Console_Command_midi(cons, &cons->input->input_buf[5]);
					} else if(cons->input->input_buf[0] != 0x00){
						Console_Execute(cons);
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
		if(System_SegmentDescriptor_Get_Limit(i) != 0){
			sprintf(s, "0x%04X:[0x%08X](0x%08X) ", i, System_SegmentDescriptor_Get_Base(i), System_SegmentDescriptor_Get_Limit(i));
			InputBox_Put_String(cons->input, s);
			ar = System_SegmentDescriptor_Get_AccessRight(i);
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

void Console_Command_midi(UI_Console *cons, const uchar filename[])
{
	IO_File file;
	int n;
	uint p, q, tracksize, r, datalength, delta;
	uchar s[128];
	uint DeltaTimePerQuarterNote, microsTimePerQuarterNote, microsTimePerDeltaTime;

	DeltaTimePerQuarterNote = 480;
	microsTimePerQuarterNote = 500000;
	microsTimePerDeltaTime = microsTimePerQuarterNote / DeltaTimePerQuarterNote;
	
	n = FloppyDisk_Search_File(sysdata->fd_boot, filename);
	if(n != -1){
		n = FloppyDisk_Load_File(sysdata->fd_boot, &file, n);
		if(n != -1){
			p = 0;
			if(strncmp(&file.data[p + 0], "MThd", 4) == 0){
				p += 4;
				p += 4;

				sprintf(s, "SMF Format%d ", (file.data[p + 0] << 8) | file.data[p + 1]);
				InputBox_Put_String(cons->input, s);
				p += 2;

				sprintf(s, "Tracks:%d\n", (file.data[p + 0] << 8) | file.data[p + 1]);
				InputBox_Put_String(cons->input, s);
				p += 2;

				DeltaTimePerQuarterNote = (file.data[p + 0] << 8) | file.data[p + 1];
				microsTimePerDeltaTime = microsTimePerQuarterNote / DeltaTimePerQuarterNote;
				sprintf(s, "DeltaTime(per quarter note):%d\n", DeltaTimePerQuarterNote);
				InputBox_Put_String(cons->input, s);
				p += 2;

				for(; p < file.size; ){
					if(strncmp(&file.data[p + 0], "MTrk", 4) == 0){
						InputBox_Put_String(cons->input, "Track:\n");
						p += 4;

						tracksize = (file.data[p + 0] << 24) | (file.data[p + 1] << 16) | (file.data[p + 2] << 8) | file.data[p + 3];
						sprintf(s, "\tsize:0x%X\n", tracksize);
						InputBox_Put_String(cons->input, s);
						p += 4;

						for(q = 0; q < tracksize; ){
							delta = Console_Command_midi_Convert_VariableLengthValue(&file.data[p + 0], &q);
							sprintf(s, "\t%8d:", delta);
							InputBox_Put_String(cons->input, s);

							IO_Wait(microsTimePerDeltaTime * delta);

							if(file.data[p + q + 0] == 0xff){
								InputBox_Put_String(cons->input, "Meta ");
								q++;
								r = file.data[p + q + 0];
								q++;
								datalength = Console_Command_midi_Convert_VariableLengthValue(&file.data[p + 0], &q);
								if(r == 0x00){		//シーケンス番号
									sprintf(s, "SequenceNumber size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x01){	//テキスト
									sprintf(s, "TEXT size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x02){	//著作権表示
									sprintf(s, "Copyright size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x03){	//トラック名
									sprintf(s, "TrackName size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x04){	//楽器名
									sprintf(s, "InstrumentName size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x05){	//歌詞
									sprintf(s, "Lyrics size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x06){	//マーカー
									sprintf(s, "Marker size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x07){	//キューポイント
									sprintf(s, "QueuePoint size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x08){	//音色（プログラム）名
									sprintf(s, "ProgramName size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x09){	//音源（デバイス）名
									sprintf(s, "DeviceName size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x20){	//MIDIチャンネルプリフィックス
									sprintf(s, "MIDIChannelPrefix size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x21){	//ポート指定
									sprintf(s, "OutPutPort size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x2f){	//End of Track
									sprintf(s, "End of Track size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x51){	//テンポ設定（4分音符あたりのマイクロ秒）
									microsTimePerQuarterNote = (file.data[p + q + 0] << 16) | (file.data[p + q + 1] << 8) | file.data[p + q + 2];
									sprintf(s, "Tempo %dmicrosec per quarter note.\n", datalength);
									microsTimePerDeltaTime = microsTimePerQuarterNote / DeltaTimePerQuarterNote;
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x54){	//SMPTEオフセット
									sprintf(s, "SMPTE_Offset size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x58){	//拍子の設定
									sprintf(s, "Beat size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x59){	//調の設定
									sprintf(s, "Tone size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else if(r == 0x7f){	//シーケンサ特定メタイベント
									sprintf(s, "SequencerSpecialMetaEvent size:%d\n", datalength);
									InputBox_Put_String(cons->input, s);
									q += datalength;
								} else{
									InputBox_Put_String(cons->input, "midi:Unknown Meta Event.\n");
									break;
								}
							} else if((file.data[p + q + 0] & 0xf0) == 0x80){	//note off
								sprintf(s, "Note:Off ch:%d note:%d%s\n", (file.data[p + q + 0] & 0x0f), (int)(file.data[p + q + 1] / 12) - 1, MIDI_Notes[file.data[p + q + 1] % 12]);
								InputBox_Put_String(cons->input, s);
								PIT_Beep_Off();
								q += 3;
							} else if((file.data[p + q + 0] & 0xf0) == 0x90){	//note on or off
								if(file.data[p + q + 2] == 0x00){	//note off
									sprintf(s, "Note:Off ch:%d note:%d%s\n", (file.data[p + q + 0] & 0x0f), (int)(file.data[p + q + 1] / 12) - 1, MIDI_Notes[file.data[p + q + 1] % 12]);
									PIT_Beep_Off();
								} else{
									sprintf(s, "Note:On  ch:%d note:%d%s vel:%d\n", (file.data[p + q + 0] & 0x0f), (int)(file.data[p + q + 1] / 12) - 1, MIDI_Notes[file.data[p + q + 1] % 12], file.data[p + q + 2]);
									PIT_Beep_On();
									Console_Command_midi_Beep_Set_NoteNumber(file.data[p + q + 1]);
								}
								InputBox_Put_String(cons->input, s);
								q += 3;
							} else if((file.data[p + q + 0] & 0xf0) == 0xa0){	//Polyphonic Key Pressure
								q += 3;
							} else if((file.data[p + q + 0] & 0xf0) == 0xb0){	//Control Change
								q += 3;
							} else if((file.data[p + q + 0] & 0xf0) == 0xc0){	//Program Change
								q += 2;
							} else if((file.data[p + q + 0] & 0xf0) == 0xd0){	//Channel Pressure
								q += 2;
							} else if((file.data[p + q + 0] & 0xf0) == 0xe0){	//Pitch Bend
								q += 3;
							} else{
								break;
							}
						}
						PIT_Beep_Off();
						p += tracksize;
					} else{
						InputBox_Put_String(cons->input, "midi:Unknown Track.\n");
					}
				}	
			} else{
				InputBox_Put_String(cons->input, "midi:Unknown header.\n");
			}
		} else{
			InputBox_Put_String(cons->input, "midi:File load error.\n");
		}
		File_Free(&file);
	} else{
		InputBox_Put_String(cons->input, "midi:File not found.\n");
	}

	return;
}

uint Console_Command_midi_Convert_VariableLengthValue(uchar *base, uint *offset)
{
	uint i, r;

	r = 0;

	for(i = 0; i < 4; i++){
		r = r << 7;
		r += (base[*offset + i] & 0x7f);
		if((base[*offset + i] & 0x80) == 0){
			i++;
			break;
		}
	}

	*offset += i;

	return r;
}

void Console_Command_midi_Beep_Set_NoteNumber(uchar n)
{
	uint fq, oct;

	fq = 0;
	oct = 0;

	fq = ToneTable[n % 12];
	oct = n / 12;

	if(n > 71){	//4オクターブより上
		oct = oct - 5;
		fq = fq << oct;
	} else if(n < 60){	//4オクターブより下
		oct = 5 - oct;
		fq = fq >> oct;
	}

	PIT_Beep_Set(fq);

	return;
}

void Console_Execute(UI_Console *cons)
{
	int n;
	uchar *ext, appname[11];
	uint i;

	ext = 0;
	for(i = 0; cons->input->input_buf[i] != 0x00; i++){
		if(cons->input->input_buf[i] == '.'){
			ext = &cons->input->input_buf[i];
		}
	}

	n = FloppyDisk_Search_File(sysdata->fd_boot, cons->input->input_buf);
	if(n != -1){
		if(ext != 0){
			if(strcmp(ext, ".chn") == 0){
				Console_Execute_CHNOSProject(cons, n);
			} else if(strcmp(ext, ".hrb") == 0){
				Console_Execute_haribote(cons, n);
			} else{
				InputBox_Put_String(cons->input, "Console:Unknown file type.\n");
			}
		} else{
			InputBox_Put_String(cons->input, "Console:Unknown file type.\n");
		}
	} else{
		if(ext == 0){	/*ファイルが存在せず、かつ拡張子がなかった場合、.chnであると仮定する。*/
			sprintf(appname, "        .CHN");
			for(i = 0; i < 8; i++){
				if(cons->input->input_buf[i] == 0x00){
					break;
				}
				appname[i] = cons->input->input_buf[i];
			}
		}
		n = FloppyDisk_Search_File(sysdata->fd_boot, appname);
		if(n != -1){
			Console_Execute_CHNOSProject(cons, n);
		} else{
			InputBox_Put_String(cons->input, "Console:No such File or Application.\n");
		}
	}
	return;
}

void Console_Execute_CHNOSProject(UI_Console *cons, int n)
{
	UI_Task *mytask;

	mytask = MultiTask_Get_NowTask();
	n = FloppyDisk_Load_File(sysdata->fd_boot, &cons->app_codefile, n);
	if(n != -1){
		cons->app_cs = System_SegmentDescriptor_Set(cons->app_codefile.size - 1, (uint)cons->app_codefile.data, AR_CODE32_ER | AR_USER);
		cons->app_ds = System_SegmentDescriptor_Set(64 * 1024 - 1, (uint)MemoryBlock_Allocate_System(64 * 1024), AR_DATA32_RW | AR_USER);
		if(cons->app_codefile.size >= 8 && strncmp(cons->app_codefile.data + 4, "CHNP", 4) == 0){
			cons->app_codefile.data[0] = 0xe8;
			cons->app_codefile.data[1] = 0x16;
			cons->app_codefile.data[2] = 0x00;
			cons->app_codefile.data[3] = 0x00;
			cons->app_codefile.data[4] = 0x00;
			cons->app_codefile.data[5] = 0xcb;
		}
		APP_Run(0, cons->app_cs << 3, System_SegmentDescriptor_Get_Limit(cons->app_ds) + 1, cons->app_ds << 3, &(mytask->tss.esp0));
		System_SegmentDescriptor_Set_Absolute(cons->app_ds, 0, 0, 0);
		cons->app_ds = 0;
		MemoryBlock_Free((void *)System_SegmentDescriptor_Get_Base(cons->app_ds));
		System_SegmentDescriptor_Set_Absolute(cons->app_cs, 0, 0, 0);
		cons->app_cs = 0;
	} else{
		InputBox_Put_String(cons->input, "Console:Execute.chn:File load error.\n");
	}
	File_Free(&cons->app_codefile);
	return;
}

void Console_Execute_haribote(UI_Console *cons, int n)
{
	n = FloppyDisk_Load_File(sysdata->fd_boot, &cons->app_codefile, n);
	if(n != -1){
		cons->app_cs = System_SegmentDescriptor_Set(cons->app_codefile.size - 1, (uint)cons->app_codefile.data, AR_CODE32_ER);
		if(cons->app_codefile.size >= 8 && strncmp(cons->app_codefile.data + 4, "Hari", 4) == 0){
			cons->app_codefile.data[0] = 0xe8;
			cons->app_codefile.data[1] = 0x16;
			cons->app_codefile.data[2] = 0x00;
			cons->app_codefile.data[3] = 0x00;
			cons->app_codefile.data[4] = 0x00;
			cons->app_codefile.data[5] = 0xcb;
		}
		FarCall(0, cons->app_cs << 3);
		System_SegmentDescriptor_Set_Absolute(cons->app_cs, 0, 0, 0);
		cons->app_cs = 0;
	} else{
		InputBox_Put_String(cons->input, "Console:Execute.hrb:File load error.\n");
	}
	File_Free(&cons->app_codefile);
	return;
}
