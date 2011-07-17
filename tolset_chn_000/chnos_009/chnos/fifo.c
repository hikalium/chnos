
#include "core.h"

int FIFO32_Initialise(DATA_FIFO *fifo, uint size)
{
	fifo->size = size;
	fifo->buf = MemoryBlock_Allocate_System(size * 4);
	MemoryBlock_Write_Description(fifo->buf, "FIFO_BUFFER");
	fifo->free = size;	/*freesize*/
	fifo->flags = 0;
	fifo->p = 0;	/*write*/
	fifo->q = 0;	/*read*/
	fifo->task = 0;
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
	if(fifo->task != 0){
		if(fifo->task->state != inuse){
			MultiTask_Task_Run(fifo->task);
		}
	}
	IO_Store_EFlags(eflags);

	return 0;
}

int FIFO32_Put_Arguments(DATA_FIFO *fifo, uint args[])
{
	uint i;
	uint eflags;

	eflags = IO_Load_EFlags();
	IO_CLI();

	for(i = 0; i < 128; i++){
		if(FIFO32_Put(fifo, args[i]) == -1){
			i = -1;
			break;
		}
		if(args[i] == 0xFFFFFFFF){
			break;
		}
	}
	IO_Store_EFlags(eflags);
	return i;
}

void FIFO32_Set_Task(DATA_FIFO *fifo, UI_Task *task)
{
	fifo->task = task;
	return;
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
	return MemoryBlock_Free(fifo->buf);
}
