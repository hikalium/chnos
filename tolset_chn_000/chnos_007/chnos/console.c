#include "core.h"

void console_main(struct WINDOWINFO *win)
{
	struct FIFO32 fifo;
	struct TIMER *timer;
	struct TASK *task = task_now();
	struct POSITION_2D cursor;
	bool cursor_state = true;
	int i, fifobuf[128];
	unsigned int cursor_c;

	cursor.x = 0;
	cursor.y = 0;

	fifo32_init(&fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &fifo, 1);
	timer_settime(timer, 50);

	boxfill_win(win, 0x000000, 0, 0, win->xsize, win->ysize);

	for(;;){
		io_cli();
		if(fifo32_status(&fifo) == 0){
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if(i == 1){
				if(cursor_state){
					cursor_c = 0xFFFFFF;
					cursor_state = false;
				} else{
					cursor_c = 0x000000;
					cursor_state = true;
				}
				timer_settime(timer, 50);
				boxfill_win(win, cursor_c, cursor.x, cursor.y, cursor.x + 8, cursor.y +16);
			}
		}
	}
}
