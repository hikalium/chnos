#include "core.h"

void fifo32_init(struct FIFO32 *fifo, int size, unsigned int *buf)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size;	/*freesize*/
	fifo->flags = 0;
	fifo->p = 0;	/*write*/
	fifo->q = 0;	/*read*/
	return;
}

int fifo32_put(struct FIFO32 *fifo, unsigned int data)
{
	if (fifo->free == 0 ) {
		fifo->flags |= FIFO32_PUT_OVER;
		return -1;
		}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if(fifo->p == fifo->size) {
		fifo->p = 0;
		}
	fifo->free--;
	return 0;

}

int fifo32_get(struct FIFO32 *fifo)
{
	int data;
	if (fifo->free == fifo->size) {
		return -1;
		}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q =0;
		}
	fifo->free++;
	return data;
}

int fifo32_status(struct FIFO32 *fifo)
{
	return fifo->size - fifo->free;
}
