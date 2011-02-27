
#include "core.h"

void FIFO32_Initialise(DATA_FIFO *fifo, uint size, uint *buf)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size;	/*freesize*/
	fifo->flags = 0;
	fifo->p = 0;	/*write*/
	fifo->q = 0;	/*read*/
	return;
}

int FIFO32_Put(DATA_FIFO *fifo, uint data)
{
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

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
	IO_Store_EFlags(eflags);

	return 0;
}

uint FIFO32_Get(DATA_FIFO *fifo)
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

uint FIFO32_Status(DATA_FIFO *fifo)
{
	return fifo->size - fifo->free;
}
