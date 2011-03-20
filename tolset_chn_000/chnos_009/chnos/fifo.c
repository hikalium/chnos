
#include "core.h"

int FIFO32_Initialise(DATA_FIFO *fifo, uint size)
{
	fifo->size = size;
	fifo->buf = System_MemoryControl_Allocate(size * 4);
	if(fifo->buf == 0){
		return -1;
	}
	fifo->free = size;	/*freesize*/
	fifo->flags = 0;
	fifo->p = 0;	/*write*/
	fifo->q = 0;	/*read*/
	return 0;
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

int FIFO32_Free(DATA_FIFO *fifo)
{
	if(fifo->buf == 0){
		return -1;
	}
	fifo->size = 0;
	fifo->free = 0;
	fifo->flags = 0;
	fifo->p = 0;
	fifo->q = 0;
	return System_MemoryControl_Free(fifo->buf, fifo->size * 4);
}
