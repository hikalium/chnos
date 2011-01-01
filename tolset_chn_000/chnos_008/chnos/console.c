
#include "core.h"
#include <string.h>

void console_main(UI_Console *cons)
{
	UI_Timer *timer;
	bool cursor_state = true;
	bool cursor_on = false;
	int i;
	uint fifobuf[CONSOLE_FIFO_BUF_SIZE];
	uint cursor_c;
	uchar s[128];
	uchar cmdline[CONSOLE_CMDLINE_BUF_SIZE];
	uint cmdlines;
	bool cmdline_overflow;

	cons->prompt.x = 0;
	cons->prompt.y = 0;

	fifo32_init(&cons->task->fifo, CONSOLE_FIFO_BUF_SIZE, fifobuf, cons->task);
	timer = timer_alloc();
	timer_init(timer, &cons->task->fifo, 1);
	timer_settime(timer, 50);

	boxfill_win(cons->win, CONSOLE_COLOR_BACKGROUND, 0, 0, cons->win->xsize, cons->win->ysize);
	cons_put_prompt(cons);

	cons_reset_cmdline(cmdline, &cmdlines, &cmdline_overflow);

	for(;;){
		io_cli();
		if(fifo32_status(&cons->task->fifo) == 0){
			task_sleep(cons->task);
			io_sti();
		} else {
			i = fifo32_get(&cons->task->fifo);
			io_sti();
			if(i == 1){
				if(cursor_on){
					if(cursor_state){
						cursor_c = CONSOLE_COLOR_CHAR;
						cursor_state = false;
					} else{
						cursor_c = CONSOLE_COLOR_BACKGROUND;
						cursor_state = true;
					}
					boxfill_win(cons->win, cursor_c, cons->cursor.x, cons->cursor.y, cons->cursor.x + 8, cons->cursor.y +16);
				}
				timer_settime(timer, 50);
			} else if(i == CONSOLE_FIFO_CURSOR_START){
				cursor_on = true;
			} else if(i == CONSOLE_FIFO_CURSOR_STOP){
				cursor_on = false;
				boxfill_win(cons->win, CONSOLE_COLOR_BACKGROUND, cons->cursor.x, cons->cursor.y, cons->cursor.x + 8, cons->cursor.y +16);
			} else if(CONSOLE_FIFO_START_KEYB <= i && i <= CONSOLE_FIFO_START_KEYB + DATA_BYTE){
				i -= CONSOLE_FIFO_START_KEYB;
				if(i == 0x0e){
					putfonts_win(cons->win, cons->cursor.x, cons->cursor.y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
					cons->cursor.x -= 8;
					cons_check_newline(cons);
					putfonts_win(cons->win, cons->cursor.x, cons->cursor.y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
					if(cmdlines != 0){
						cmdlines--;
						cmdline[cmdlines] = 0x00;
					}
				} else if(i == 0x0a){
					cons_command_start(cons, cmdline, &cmdlines, &cmdline_overflow);
				} else{
					s[0] = (uchar)i;
					s[1] = 0x00;
					cons_put_str(cons, s);
					if(cmdlines >= CONSOLE_CMDLINE_BUF_SIZE){
						cmdline_overflow = true;
						cmdlines = 0;
					}
					cmdline[cmdlines] = (uchar)i;
					cmdline[cmdlines + 1] = 0x00;
					cmdlines++;
				}
			}
		}
	}
}

void cons_reset_cmdline(uchar *cmdline, uint *cmdlines, bool *cmdline_overflow)
{
	*cmdlines = 0;
	*cmdline_overflow = false;
	cmdline[*cmdlines] = 0x00;
	return;
}

void cons_command_start(UI_Console *cons, uchar *cmdline, uint *cmdlines, bool *cmdline_overflow)
{
	uchar s[128], t[7];
	uint i;
	UUID uuid;

	i = 0;

	if(cmdline[0] != 0x00){
		cons_new_line_no_prompt(cons);
	}
	if(strcmp(cmdline, "mem") == 0){
		cons_command_mem(cons);
	} else if(strcmp(cmdline, "cls") == 0){
		cons->prompt.x = 0;
		cons->prompt.y = 0;
		boxfill_win(cons->win, CONSOLE_COLOR_BACKGROUND, 0, 0, cons->win->xsize, cons->win->ysize);
		cons_reset_cmdline(cmdline, cmdlines, cmdline_overflow);
		cons_put_prompt(cons);
		goto end;
	} else if(strcmp(cmdline, "dir") == 0){
		cons_command_dir(cons);
	} else if(strcmp(cmdline, "reset") == 0){
		reset_cpu();
	} else if(strcmp(cmdline, "uuid") == 0){
		gen_UUID(&uuid);
		sprintf(s, "{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}\r\n", uuid.data[0], uuid.data[1], uuid.data[2], uuid.data[3], uuid.data[4], uuid.data[5], uuid.data[6], uuid.data[7], uuid.data[8], uuid.data[9], uuid.data[10], uuid.data[11], uuid.data[12], uuid.data[13], uuid.data[14], uuid.data[15]);
		cons_put_str(cons, s);
	} else if(strcmp(cmdline, "date") == 0){
		readrtc(t);
		sprintf(s, "%02X%02X.%02X.%02X %02X:%02X:%02X\n", t[6], t[5], t[4], t[3], t[2], t[1], t[0]);
		cons_put_str(cons, s);
	} else if(strncmp(cmdline, "fdc", 3) == 0){
		cons_command_fdc(cons, cmdline);
	} else if(strncmp(cmdline, "cpuid ", 5) == 0){
		cons_command_cpuid(cons, cmdline);
	} else if(strncmp(cmdline, "systeminfo", 10) == 0){
		cons_command_systeminfo(cons, cmdline);
	} else if(strncmp(cmdline, "type ", 5) == 0){
		cons_command_type(cons, cmdline);
	} else if(cmdline[0] != 0x00){
		for(i = 0; i < 11 && cmdline[i] != 0x00; i++){
			if(cmdline[i] <= ' ' || cmdline[i] == '.') break;
		}
		cmdline[i] = 0x00;
		if(cons_app_hrb_start(cons, cmdline) == 0xFFFFFFFF){
			cons_put_str(cons, "Bad Command...\n");
		}
	}
	cons_reset_cmdline(cmdline, cmdlines, cmdline_overflow);
	cons_new_line(cons);
end:
	return;
}

void cons_command_mem(UI_Console *cons)
{
	uchar s[32];

	sprintf(s, "ÒÓØ°:%dMB\n", system.io.mem.total >> 20);
	cons_put_str(cons, s);
	sprintf(s, "±·:%dKB\n", sys_memman_free_total() >> 10);
	cons_put_str(cons, s);
	return;
}

void cons_command_dir(UI_Console *cons)
{
	uchar s[64];
	int i, j;

	for(i = 0; i < 0xe0; i++){
		if(system.io.file.list[i].name[0] == 0x00) break;
		if(system.io.file.list[i].name[0] != 0xe5){
			if((system.io.file.list[i].type & 0x18) == 0){
				sprintf(s, "FILENAME.EXT %7d %04d/%02d/%02d-%02d:%02d:%02d\n", system.io.file.list[i].size, (system.io.file.list[i].date >> 9) + 1980, (system.io.file.list[i].date & 0x01e0) >> 5, system.io.file.list[i].date & 0x001f, system.io.file.list[i].time >> 11, (system.io.file.list[i].time & 0x07e0) >> 5, system.io.file.list[i].time & 0x1f);
				for(j = 0; j < 8; j++){
					s[j] = system.io.file.list[i].name[j];
				}
				s[ 9] = system.io.file.list[i].ext[0];
				s[10] = system.io.file.list[i].ext[1];
				s[11] = system.io.file.list[i].ext[2];
				cons_put_str(cons, s);
			}
		}
	}
	return;
}

void cons_command_fdc(UI_Console *cons, uchar *cmdline)
{
	int i;

	cons_put_str(cons, "FDCconfig...\n");
	if(cmdline[3] == 0x00){
		i = io_in8(0x03f2);
		if((i & 0x10) != 0) cons_put_str(cons, "FD0 Motor-ON\n");
		else cons_put_str(cons, "FD0 Motor-OFF\n");
		if((i & 0x20) != 0) cons_put_str(cons, "FD1 Motor-ON\n");
		else cons_put_str(cons, "FD1 Motor-OFF\n");
		if((i & 0x40) != 0) cons_put_str(cons, "FD2 Motor-ON\n");
		else cons_put_str(cons, "FD2 Motor-OFF\n");
		if((i & 0x80) != 0) cons_put_str(cons, "FD3 Motor-ON\n");
		else cons_put_str(cons, "FD3 Motor-OFF\n");
	}
	if(cmdline[3] == ' '){
		if(cmdline[4] == '0'){
			if(cmdline[5] == ' ' && cmdline[6] == 'o' && cmdline[7] == 'n' && cmdline[8] == 0x00){
				fdc_motor_on(0);
			} else if(cmdline[5] == ' ' && cmdline[6] == 'o' && cmdline[7] == 'f' && cmdline[8] == 'f' && cmdline[9] == 0x00){
				fdc_motor_off(0);
			}
		} else if(cmdline[4] == '1'){
			if(cmdline[5] == ' ' && cmdline[6] == 'o' && cmdline[7] == 'n' && cmdline[8] == 0x00){
				fdc_motor_on(1);
			} else if(cmdline[5] == ' ' && cmdline[6] == 'o' && cmdline[7] == 'f' && cmdline[8] == 'f' && cmdline[9] == 0x00){
				fdc_motor_off(1);
			}
		} else if(cmdline[4] == '2'){
			if(cmdline[5] == ' ' && cmdline[6] == 'o' && cmdline[7] == 'n' && cmdline[8] == 0x00){
				fdc_motor_on(2);
			} else if(cmdline[5] == ' ' && cmdline[6] == 'o' && cmdline[7] == 'f' && cmdline[8] == 'f' && cmdline[9] == 0x00){
				fdc_motor_off(2);
			}
		} else if(cmdline[4] == '3'){
			if(cmdline[5] == ' ' && cmdline[6] == 'o' && cmdline[7] == 'n' && cmdline[8] == 0x00){
				fdc_motor_on(3);
			} else if(cmdline[5] == ' ' && cmdline[6] == 'o' && cmdline[7] == 'f' && cmdline[8] == 'f' && cmdline[9] == 0x00){
				fdc_motor_off(3);
			}
		}
	}
	return;
}

void cons_command_type(UI_Console *cons, uchar *cmdline)
{
	int i, j;
	uchar s[32];
	uchar *p;

	j = 0;
	for(i = 5; cmdline[i] != 0x00; i++){
		s[i - 5] = cmdline[i];
		s[i - 4] = 0x00;
	}
	i = search_file(s);
	if(i != 0xFFFFFFFF){
		j = system.io.file.list[i].size;
		p = sys_memman_alloc(j); 
		load_file(i, p);
		for(i = 0; i < j; i++){
			s[0] = p[i];
			s[1] = 0x00;
			cons_put_str(cons, s);
		}
		sys_memman_free(p, j);
	} else{
		cons_put_str(cons, "File not found...\n");
	}
	return;
}

void cons_command_cpuid(UI_Console *cons, uchar *cmdline)
{
	uint i, j;
	bool cpuide;
	uchar s[32];
	uchar idbuf[32];

	i = io_load_eflags();
	io_store_eflags((i | 0x00200000));
	j = io_load_eflags();	
	if(i != j){
		cpuide = true;
		cons_put_str(cons, "CPUID is Enable.\n");
	} else{
		cpuide = false;
		cons_put_str(cons, "CPUID is Disable.\n");
	}
	io_store_eflags((j & ~0x00200000));

	idbuf[16] = '\n';
	idbuf[17] = 0x00;

	if(cpuide){
		if(strcmp((uchar *)(cmdline + 6), "0x00000000") == 0){
			s[16] = 0x00;
			cpuid(idbuf, 0x00000000);
			sprintf(s, "MAX=0x%02X%02X%02X%02X\n", idbuf[0], idbuf[1], idbuf[2], idbuf[3]);
			cons_put_str(cons, s);
			cons_put_str(cons, (uchar *)(idbuf + 4));
		}
	}

	return;
}

void cons_command_systeminfo(UI_Console *cons, uchar *cmdline)
{
	uchar s[64];

	sprintf(s, "CHNOSProject ver.Beta008\n Compiled at %s %s\n", __DATE__, __TIME__);
	cons_put_str(cons, s);
	return;
}

uint cons_app_hrb_start(UI_Console *cons, uchar *cmdline)
{
	uint i, j;
	uchar *p, *q;
	UI_Task *task = task_now();
	FORMAT_Haribote *head;

	i = search_file(cmdline);
	if(i == 0xFFFFFFFF){
		strcat(cmdline, ".hrb");
		i = search_file(cmdline);
	}
	if(i != 0xFFFFFFFF){
		j = system.io.file.list[i].size;
		p = sys_memman_alloc(j);
		head = (FORMAT_Haribote *) p;
		load_file(i, p);
		if(j >= 36 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00){
			q = sys_memman_alloc(head->DataSegmentSize);
			cons->app_ds_base = q;
			cons->app_cs_base = p;
			set_segmdesc(system.io.mem.segment.gdt + 1003, j - 1, (int)p, AR_CODE32_ER + AR_APP);
			set_segmdesc(system.io.mem.segment.gdt + 1004, head->DataSegmentSize - 1, (int)q, AR_DATA32_RW + AR_APP);
			for(i = 0; i < head->DataSegmentSize; i++){
				q[head->DefaultESP + i] = p[head->OriginDataSection + i];
			}
			start_app(0x1b, 1003 * 8, head->DefaultESP, 1004 * 8, &(task->tss.esp0));
			sys_memman_free(q, head->DataSegmentSize);
		} else{
			cons_put_str(cons, ".hrbÌ§²ÙÌ«°Ï¯Ä´×°\n");
		}
		sys_memman_free(p, j);
		return i;
	}
	return 0xFFFFFFFF;
}

void cons_put_str(UI_Console *cons, uchar *str)
{
	int i;
	uchar s[3];

	for(i = 0; i < 128; i++){
		if(str[i] == 0x00){
			break;
		} else if(str[i] == '\r'){

		} else if(str[i] == '\n'){
			cons_new_line_no_prompt(cons);
		} else if(str[i] == '\t'){
			for(;;){
				putfonts_win(cons->win, cons->cursor.x, cons->cursor.y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, " ");
				cons->cursor.x += 8;
				cons_check_newline(cons);
				if((cons->cursor.x & 0x1f) == 0 && cons->cursor.x != 0) break;
			}
		} else{
			s[0] = str[i];
			s[1] = 0x00;
			putfonts_win(cons->win, cons->cursor.x, cons->cursor.y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, s);
			cons->cursor.x += 8;
			cons_check_newline(cons);
		}
	}
	return;
}

void cons_put_char(UI_Console *cons, uchar c)
{
	uchar s[2];

	s[0] = c;
	s[1] = 0x00;
	cons_put_str(cons, s);
	return;
}

void cons_put_prompt(UI_Console *cons)
{
	putfonts_win(cons->win, cons->cursor.x, cons->cursor.y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
	putfonts_win(cons->win, cons->prompt.x, cons->prompt.y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, ">");
	cons->cursor.x = cons->prompt.x + 8;
	cons->cursor.y = cons->prompt.y;
	return;
}

void cons_new_line(UI_Console *cons)
{
	if(cons->cursor.y <= system.ui.console.org_ysize - 17){
		cons->prompt.y = cons->cursor.y + 16;
		cons_put_prompt(cons);
	} else{
		putfonts_win(cons->win, cons->cursor.x, cons->cursor.y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
		cons_slide_line(cons);
		cons->prompt.y = (system.ui.console.org_ychars - 1) * 16;
		cons_put_prompt(cons);
	}
	return;
}

void cons_new_line_no_prompt(UI_Console *cons)
{
	if(cons->cursor.y <= system.ui.console.org_ysize - 17){
		putfonts_win(cons->win, cons->cursor.x, cons->cursor.y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
		cons->cursor.x = 0;
		cons->cursor.y = cons->cursor.y + 16;
	} else{
		putfonts_win(cons->win, cons->cursor.x, cons->cursor.y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
		cons_slide_line(cons);
		cons->cursor.x = 0;
	}
	return;
}

void cons_slide_line(UI_Console *cons)
{
	scrool_win(cons->win);
	refresh_window(cons->win);
	return;
}

void cons_check_newline(UI_Console *cons)
{
	if(cons->cursor.x <= cons->prompt.x){
		if(cons->cursor.y != cons->prompt.y){
			if(cons->cursor.x < cons->prompt.x){
				cons->cursor.y -= 16;
				cons->cursor.x = system.ui.console.org_xsize - 8;
			}
		} else{
			cons->cursor.y = cons->prompt.y;
			cons->cursor.x = 8;
		}
	} else if(cons->cursor.x >= system.ui.console.org_xsize){
		if(cons->cursor.y <= system.ui.console.org_ysize - 17){
			cons->cursor.x = 0;
			cons->cursor.y += 16;
		} else{
			cons_slide_line(cons);
			cons->cursor.x = 0;
			if(cons->prompt.y > 0) cons->prompt.y -= 16;
			else{
				cons->prompt.y = 0;
				cons->prompt.x = 0;
			}
		}
	} 
	return;
}
