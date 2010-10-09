
#include "core.h"

void cons_check_newline(struct POSITION_2D *p);

void console_main(struct WINDOWINFO *win)
{
	struct TIMER *timer;
	struct TASK *task = task_now();
	struct POSITION_2D cursor;
	bool cursor_state = true;
	bool cursor_on = false;
	int i, fifobuf[128];
	unsigned int cursor_c;
	unsigned char s[128];

	cursor.x = 8;
	cursor.y = 0;

	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);

	boxfill_win(win, 0x000000, 0, 0, win->xsize, win->ysize);
	putfonts_win(win, 0, 0, 0xFFFFFF, 0x000000, ">");

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
						cursor_c = 0xFFFFFF;
						cursor_state = false;
					} else{
						cursor_c = 0x000000;
						cursor_state = true;
					}
					boxfill_win(win, cursor_c, cursor.x, cursor.y, cursor.x + 8, cursor.y +16);
				}
				timer_settime(timer, 50);
			} else if(i == CONSOLE_FIFO_CURSOR_START){
				cursor_on = true;
			} else if(i == CONSOLE_FIFO_CURSOR_STOP){
				cursor_on = false;
					boxfill_win(win, 0x000000, cursor.x, cursor.y, cursor.x + 8, cursor.y +16);
			} else if(SYS_FIFO_START_KEYB <= i && i <= SYS_FIFO_START_KEYB + DATA_BYTE){
				i -= SYS_FIFO_START_KEYB;
				if(i == 0x0E){
					boxfill_win(win, 0x000000, cursor.x, cursor.y, cursor.x + 8, cursor.y +16);
					cursor.x -= 8;
					cons_check_newline(&cursor);
					boxfill_win(win, 0x000000, cursor.x, cursor.y, cursor.x + 8, cursor.y +16);
				} else{
					s[0] = (unsigned char)i;
					s[1] = 0x00;
					putfonts_win(win, cursor.x, cursor.y, 0xFFFFFF, 0x000000, s);
					cursor.x += 8;
					cons_check_newline(&cursor);
				}
			}
		}
	}
}

void cons_check_newline(struct POSITION_2D *p)
{
	if(p->x <= 0){
		if(p->y != 0){
			if(p->x < 0){
				p->y -= 16;
				p->x = (CONSOLE_XCHARS * 8) & 0xFFFFFFF8;
			}
		} else{
			p->y = 0;
			p->x = 8;
		}
	} else if(p->x >= ((CONSOLE_XCHARS * 8) & 0xFFFFFFF8)){
		if(p->y <= (((CONSOLE_YCHARS * 16) - 15) & 0xFFFFFFEF)){
			p->y += 16;
			p->x = 0;
		} else {
			p->x -= 8;
		}
	} 
	return;
}
