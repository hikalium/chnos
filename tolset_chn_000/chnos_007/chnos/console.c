
#include "core.h"
#include <string.h>

void console_main(struct WINDOWINFO *win)
{
	struct TIMER *timer;
	struct TASK *task = task_now();
	struct POSITION_2D cursor;
	struct POSITION_2D prompt;
	bool cursor_state = true;
	bool cursor_on = false;
	int i;
	unsigned int fifobuf[CONSOLE_FIFO_BUF_SIZE];
	unsigned int cursor_c;
	unsigned char s[128];
	unsigned char cmdline[CONSOLE_CMDLINE_BUF_SIZE];
	unsigned int cmdlines;
	bool cmdline_overflow;

	prompt.x = 0;
	prompt.y = 0;

	*((int *) 0x0fec) = (int) win;
	*((int *) 0x0fe8) = (int) &prompt;
	*((int *) 0x0fe4) = (int) &cursor;

	fifo32_init(&task->fifo, CONSOLE_FIFO_BUF_SIZE, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);

	boxfill_win(win, CONSOLE_COLOR_BACKGROUND, 0, 0, win->xsize, win->ysize);
	cons_put_prompt(win, &prompt, &cursor);

	cons_reset_cmdline(cmdline, &cmdlines, &cmdline_overflow);

	for(;;){
		io_cli();
		if(fifo32_status(&task->fifo) == 0){
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
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
					boxfill_win(win, cursor_c, cursor.x, cursor.y, cursor.x + 8, cursor.y +16);
				}
				timer_settime(timer, 50);
			} else if(i == CONSOLE_FIFO_CURSOR_START){
				cursor_on = true;
			} else if(i == CONSOLE_FIFO_CURSOR_STOP){
				cursor_on = false;
				boxfill_win(win, CONSOLE_COLOR_BACKGROUND, cursor.x, cursor.y, cursor.x + 8, cursor.y +16);
			} else if(SYS_FIFO_START_KEYB <= i && i <= SYS_FIFO_START_KEYB + DATA_BYTE){
				i -= SYS_FIFO_START_KEYB;
				if(i == 0x0e){
					putfonts_win(win, cursor.x, cursor.y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
					cursor.x -= 8;
					cons_check_newline(win, &cursor, &prompt);
					putfonts_win(win, cursor.x, cursor.y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
					if(cmdlines != 0){
						cmdlines--;
						cmdline[cmdlines] = 0x00;
					}
				} else if(i == 0x0a){
					cons_command_start(win, &prompt, &cursor, cmdline, &cmdlines, &cmdline_overflow);
				} else{
					s[0] = (unsigned char)i;
					s[1] = 0x00;
					cons_put_str(win, &prompt, &cursor, s);
					if(cmdlines >= CONSOLE_CMDLINE_BUF_SIZE){
						cmdline_overflow = true;
						cmdlines = 0;
					}
					cmdline[cmdlines] = (unsigned char)i;
					cmdline[cmdlines + 1] = 0x00;
					cmdlines++;
				}
			}
		}
	}
}

void cons_reset_cmdline(unsigned char *cmdline, unsigned int *cmdlines, bool *cmdline_overflow)
{
	*cmdlines = 0;
	*cmdline_overflow = false;
	cmdline[*cmdlines] = 0x00;
	return;
}

void cons_command_start(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, unsigned char *cmdline, unsigned int *cmdlines, bool *cmdline_overflow)
{
	unsigned char s[128], t[7];
	unsigned int i, j;
	unsigned char *p;

	if(cmdline[0] != 0x00){
		cons_new_line_no_prompt(win, prompt, cursor);
	}
	if(strcmp(cmdline, "mem") == 0){
		sprintf(s, "ÒÓØ°:%dMB\n", system.sys.memtotal >> 20);
		cons_put_str(win, prompt, cursor, s);
		sprintf(s, "±·:%dKB", system.io.mem.free_total() >> 10);
		cons_put_str(win, prompt, cursor, s);
	} else if(strcmp(cmdline, "cls") == 0){
		prompt->x = 0;
		prompt->y = 0;
		boxfill_win(win, CONSOLE_COLOR_BACKGROUND, 0, 0, win->xsize, win->ysize);
		cons_reset_cmdline(cmdline, cmdlines, cmdline_overflow);
		cons_put_prompt(win, prompt, cursor);
		goto end;
	} else if(strcmp(cmdline, "dir") == 0){
		for(i = 0; i < 0xe0; i++){
			if(system.file.list[i].name[0] == 0x00) break;
			if(system.file.list[i].name[0] != 0xe5){
				if((system.file.list[i].type & 0x18) == 0){
					sprintf(s, "FILENAME.EXT %7d %04d/%02d/%02d-%02d:%02d:%02d\n", system.file.list[i].size, (system.file.list[i].date >> 9) + 1980, (system.file.list[i].date & 0x01e0) >> 5, system.file.list[i].date & 0x001f, system.file.list[i].time >> 11, (system.file.list[i].time & 0x07e0) >> 5, system.file.list[i].time & 0x1f);
					for(j = 0; j < 8; j++){
						s[j] = system.file.list[i].name[j];
					}
					s[ 9] = system.file.list[i].ext[0];
					s[10] = system.file.list[i].ext[1];
					s[11] = system.file.list[i].ext[2];
					cons_put_str(win, prompt, cursor, s);
				}
			}
		}
	} else if(strcmp(cmdline, "reset") == 0){
		reset_cpu();
	} else if(strcmp(cmdline, "date") == 0){
		readrtc(t);
		sprintf(s, "%02X%02X.%02X.%02X %02X:%02X:%02X\n", t[6], t[5], t[4], t[3], t[2], t[1], t[0]);
		cons_put_str(win, prompt, cursor, s);
	} else if(strncmp(cmdline, "type ", 5) == 0){
		j = 0;
		for(i = 5; cmdline[i] != 0x00; i++){
			s[i - 5] = cmdline[i];
			s[i - 4] = 0x00;
		}
		i = search_file(s);
		if(i != 0xFFFFFFFF){
			j = system.file.list[i].size;
			p = system.io.mem.alloc(j); 
			load_file(i, p);
			for(i = 0; i < j; i++){
				s[0] = p[i];
				s[1] = 0x00;
				cons_put_str(win, prompt, cursor, s);
			}
			system.io.mem.free(p, j);
		} else{
			cons_put_str(win, prompt, cursor, "File not found...\n");
		}
	} else if(cmdline[0] != 0x00){
		strcat(cmdline, ".hrb");
		i = search_file(cmdline);
		if(i != 0xFFFFFFFF){
			j = system.file.list[i].size;
			p = system.io.mem.alloc(j);
			*((int *) 0x0fe0) = (int) p;
			load_file(i, p);
			if(j >= 8 && strncmp(p + 4, "Hari", 4) == 0){
				p[0] = 0xe8;
				p[1] = 0x16;
				p[2] = 0x00;
				p[3] = 0x00;
				p[4] = 0x00;
				p[5] = 0xcb;
			}
			set_segmdesc(system.sys.gdt + 1003, j - 1, (int)p, AR_CODE32_ER);
			farcall(0, 1003 * 8);
			system.io.mem.free(p, j);
		} else{
			i = search_file(cmdline);
			if(i != 0xFFFFFFFF){
				j = system.file.list[i].size;
				p = system.io.mem.alloc(j);
				*((int *) 0x0fe0) = (int) p;
				load_file(i, p);
				if(j >= 8 && strncmp(p + 4, "Hari", 4) == 0){
					p[0] = 0xe8;
					p[1] = 0x16;
					p[2] = 0x00;
					p[3] = 0x00;
					p[4] = 0x00;
					p[5] = 0xcb;
				}
				set_segmdesc(system.sys.gdt + 1003, j - 1, (int)p, AR_CODE32_ER);
				farcall(0, 1003 * 8);
				system.io.mem.free(p, j);
			} else{
				cons_put_str(win, prompt, cursor, "Bad command...\n");
			}
		}
	}
	cons_reset_cmdline(cmdline, cmdlines, cmdline_overflow);
	cons_new_line(win, prompt, cursor);
end:
	return;
}

void cons_put_str(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, unsigned char *str)
{
	int i;
	unsigned char s[3];

	for(i = 0; i < 128; i++){
		if(str[i] == 0x00){
			break;
		} else if(str[i] == '\r'){

		} else if(str[i] == '\n'){
			cons_new_line_no_prompt(win, prompt, cursor);
		} else if(str[i] == '\t'){
			for(;;){
				putfonts_win(win, cursor->x, cursor->y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, " ");
				cursor->x += 8;
				cons_check_newline(win, cursor, prompt);
				if((cursor->x & 0x1f) == 0 && cursor->x != 0) break;
			}
		} else{
			s[0] = str[i];
			s[1] = 0x00;
			putfonts_win(win, cursor->x, cursor->y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, s);
			cursor->x += 8;
			cons_check_newline(win, cursor, prompt);
		}
	}
	return;
}

void cons_put_char(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, unsigned char c)
{
	unsigned char s[2];
	s[0] = c;
	s[1] = 0x00;
	cons_put_str(win, prompt, cursor, s);
	return;
}

void cons_put_prompt(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor)
{
	putfonts_win(win, cursor->x, cursor->y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
	putfonts_win(win, prompt->x, prompt->y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, ">");
	cursor->x = prompt->x + 8;
	cursor->y = prompt->y;
	return;
}

void cons_new_line(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor)
{
	if(cursor->y <= system.sys.cons.org_ysize - 17){
		prompt->y = cursor->y + 16;
		cons_put_prompt(win, prompt, cursor);
	} else{
		putfonts_win(win, cursor->x, cursor->y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
		cons_slide_line(win);
		prompt->y = (system.sys.cons.org_ychars - 1) * 16;
		cons_put_prompt(win, prompt, cursor);
	}
	return;
}

void cons_new_line_no_prompt(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor)
{
	if(cursor->y <= system.sys.cons.org_ysize - 17){
		putfonts_win(win, cursor->x, cursor->y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
		cursor->x = 0;
		cursor->y = cursor->y + 16;
	} else{
		putfonts_win(win, cursor->x, cursor->y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
		cons_slide_line(win);
		cursor->x = 0;
	}
	return;
}

void cons_slide_line(struct WINDOWINFO *win)
{
	scrool_win(win);
	refresh_window(win);
	return;
}

void cons_check_newline(struct WINDOWINFO *win, struct POSITION_2D *p, struct POSITION_2D *prompt)
{
	if(p->x <= prompt->x){
		if(p->y != prompt->y){
			if(p->x < prompt->x){
				p->y -= 16;
				p->x = system.sys.cons.org_xsize - 8;
			}
		} else{
			p->y = prompt->y;
			p->x = 8;
		}
	} else if(p->x >= system.sys.cons.org_xsize){
		if(p->y <= system.sys.cons.org_ysize - 17){
			p->x = 0;
			p->y += 16;
		} else{
			cons_slide_line(win);
			p->x = 0;
			if(prompt->y > 0) prompt->y -= 16;
			else{
				prompt->y = 0;
				prompt->x = 0;
			}
		}
	} 
	return;
}
