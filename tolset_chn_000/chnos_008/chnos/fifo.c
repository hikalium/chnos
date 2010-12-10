
#include "core.h"

void fifo32_init(DATA_FIFO *fifo, uint size, uint *buf, UI_Task *task)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size;	/*freesize*/
	fifo->flags = 0;
	fifo->p = 0;	/*write*/
	fifo->q = 0;	/*read*/
	fifo->task = task;
	return;
}

int fifo32_put(DATA_FIFO *fifo, uint data)
{
	int eflags;

	eflags = io_load_eflags();
	io_cli();

	if (fifo->free == 0 ) {
		fifo->flags |= FIFO32_PUT_OVERFLOW;
		return -1;
		}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if(fifo->p == fifo->size) {
		fifo->p = 0;
		}
	fifo->free--;
	if(fifo->task != 0){
		if(fifo->task->flags != inuse) task_run(fifo->task, -1, 0);
	}

	io_store_eflags(eflags);

	return 0;

}

uint fifo32_get(DATA_FIFO *fifo)
{
	int data;
	if (fifo->free == fifo->size) {
		if(fifo->q == 0) return fifo->buf[fifo->size];
		return fifo->buf[fifo->q - 1];
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

uint fifo32_status(DATA_FIFO *fifo)
{
	return fifo->size - fifo->free;
}
