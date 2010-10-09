
#include "core.h"

void cons_check_newline(struct POSITION_2D *p, struct POSITION_2D *prompt);
void put_prompt(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor);
void new_line(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor);

void console_main(struct WINDOWINFO *win)
{
	struct TIMER *timer;
	struct TASK *task = task_now();
	struct POSITION_2D cursor;
	struct POSITION_2D prompt;
	bool cursor_state = true;
	bool cursor_on = false;
	int i, fifobuf[128];
	unsigned int cursor_c;
	unsigned char s[128];

	prompt.x = 0;
	prompt.y = 0;

	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);

	boxfill_win(win, CONSOLE_COLOR_BACKGROUND, 0, 0, win->xsize, win->ysize);
	put_prompt(win, &prompt, &cursor);

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
					cons_check_newline(&cursor, &prompt);
					putfonts_win(win, cursor.x, cursor.y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
				} else if(i == 0x0a){
					new_line(win, &prompt, &cursor);
				} else{
					s[0] = (unsigned char)i;
					s[1] = 0x00;
					putfonts_win(win, cursor.x, cursor.y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, s);
					cursor.x += 8;
					cons_check_newline(&cursor, &prompt);
				}
			}
		}
	}
}

void put_prompt(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor)
{
	putfonts_win(win, cursor->x, cursor->y, CONSOLE_COLOR_BACKGROUND, CONSOLE_COLOR_BACKGROUND, " ");
	putfonts_win(win, prompt->x, prompt->y, CONSOLE_COLOR_CHAR, CONSOLE_COLOR_BACKGROUND, ">");
	cursor->x = prompt->x + 8;
	cursor->y = prompt->y;
	return;
}

void new_line(struct WINDOWINFO *win, struct POSITION_2D *prompt, struct POSITION_2D *cursor)
{
	if(prompt->y + 16 >= (CONSOLE_YCHARS * 16) - 15){

	} else{
		prompt->y = cursor->y + 16;
		put_prompt(win, prompt, cursor);
	}
	return;
}

void cons_check_newline(struct POSITION_2D *p, struct POSITION_2D *prompt)
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
		if(p->y <= (CONSOLE_YCHARS * 16) - 15){
			p->y += 16;
			p->x = 0;
		} else {
			p->x -= 8;
		}
	} 
	return;
}
