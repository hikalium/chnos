
#include "core.h"

void cons_check_newline(struct WINDOWINFO *win, struct POSITION_2D *p, struct POSITION_2D *prompt);
void cons_put_prompt(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor);
void cons_new_line(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor);
void cons_slide_line(struct WINDOWINFO *win);
void cons_put_str(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, unsigned char *str);
void cons_reset_cmdline(unsigned char *cmdline, unsigned int *cmdlines, bool *cmdline_overflow);
void cons_command_start(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, unsigned char *cmdline);
void cons_new_line_no_prompt(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor);

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
				} else if(i == 0x0a){
					cons_new_line_no_prompt(win, &prompt, &cursor);
					cons_command_start(win, &prompt, &cursor, cmdline);
					cons_reset_cmdline(cmdline, &cmdlines, &cmdline_overflow);
					cons_new_line(win, &prompt, &cursor);
				} else{
					s[0] = (unsigned char)i;
					s[1] = 0x00;
					cons_put_str(win, &prompt, &cursor, s);
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

void cons_command_start(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, unsigned char *cmdline)
{
	cons_put_str(win, prompt, cursor, cmdline);
	return;
}

void cons_put_str(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor, unsigned char *str)
{
	int i;
	unsigned char s[3];

	for(i = 0; i < 128; i++){
		if(str[i] == 0x00) break;
		if(str[i] == '\n')cons_new_line_no_prompt(win, prompt, cursor);
		else{
			s[0] = str[i];
			s[1] = 0x00;
			putfonts_win(win, cursor->x, cursor->y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, s);
			cursor->x += 8;
			cons_check_newline(win, cursor, prompt);
		}
	}
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
	if(cursor->y <= (CONSOLE_YCHARS * 16) - 17){
		prompt->y = cursor->y + 16;
		cons_put_prompt(win, prompt, cursor);
	} else{
		putfonts_win(win, cursor->x, cursor->y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
		cons_slide_line(win);
		prompt->y = (CONSOLE_YCHARS - 1) * 16;
		cons_put_prompt(win, prompt, cursor);
	}
	return;
}

void cons_new_line_no_prompt(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor)
{
	if(cursor->y <= (CONSOLE_YCHARS * 16) - 17){
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
				p->x = (CONSOLE_XCHARS * 8) - 8;
			}
		} else{
			p->y = prompt->y;
			p->x = 8;
		}
	} else if(p->x >= CONSOLE_XCHARS * 8){
		if(p->y <= (CONSOLE_YCHARS * 16) - 17){
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
