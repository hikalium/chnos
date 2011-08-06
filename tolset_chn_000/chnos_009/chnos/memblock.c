
#include "core.h"

Memory SystemMemory;

void Initialise_MemoryBlock(IO_MemoryControl *mainctrl)
{
	SystemMemory.addr = 0;
	SystemMemory.size = 0;
	SystemMemory.next = 0;
	SystemMemory.ctrl = mainctrl;
	SystemMemory.description[0] = 0x00;
	return;
}

void *MemoryBlock_Allocate_System(uint size)
{
	Memory *block = MemoryControl_Allocate(SystemMemory.ctrl, sizeof(Memory));
	uint *retaddr;

	retaddr = &size;

	if(block == 0){
		Emergency_Out("[0x%08X]SysMemAlloc0 Failed.(%dBytes)", *(retaddr - 1), size);
		debug("[0x%08X]SysMemAlloc0 Failed.(%dBytes)", *(retaddr - 1), size);
		for(;;){
			IO_HLT();
		}
	}

	block->addr = MemoryControl_Allocate(SystemMemory.ctrl, size);

	if(block->addr == 0){
		Emergency_Out("[0x%08X]SysMemAlloc1 Failed.(%dBytes)", *(retaddr - 1), size);
		debug("[0x%08X]SysmMemAlloc1 Failed.(%dBytes)", *(retaddr - 1), size);
		for(;;){
			IO_HLT();
		}
	}

	block->size = size;
	block->next = SystemMemory.next;
	SystemMemory.next = block;
	SystemMemory.size++;
	block->ctrl = SystemMemory.ctrl;
	block->description[0] = 0x00;
	return block->addr;
}

void *MemoryBlock_Allocate_User(uint size, IO_MemoryControl *ctrl)
{
	Memory *block = MemoryControl_Allocate(SystemMemory.ctrl, sizeof(Memory));
	uint *retaddr;

	retaddr = &size;

	if(block == 0){
		Emergency_Out("[0x%08X]UsrMemAlloc0 Failed.(%dBytes)", *(retaddr - 1), size);
		debug("[0x%08X]UsrMemAlloc0 Failed.(%dBytes)", *(retaddr - 1), size);
		for(;;){
			IO_HLT();
		}
	}

	block->addr = MemoryControl_Allocate(ctrl, size);

	if(block->addr == 0){
		Emergency_Out("[0x%08X]UsrMemAlloc1 Failed.(%dBytes)", *(retaddr - 1), size);
		debug("[0x%08X]UsrMemAlloc1 Failed.(%dBytes)", *(retaddr - 1), size);
		for(;;){
			IO_HLT();
		}
	}

	block->size = (size + 7) & ~7;
	block->next = SystemMemory.next;
	SystemMemory.next = block;
	SystemMemory.size++;
	block->ctrl = ctrl;
	block->description[0] = 0x00;
	return block->addr;
}

Memory *MemoryBlock_Verify(void *addr)
{
	Memory *next;

	for(next = &SystemMemory; next->next != 0; next = next->next){
		if(next->next->addr == addr){
			return next->next;
		}
	}
	return 0;
}

int MemoryBlock_Write_Description(void *addr, const uchar *description)
{
	uint i;
	Memory *block;

	block = MemoryBlock_Verify(addr);

	if(block){
		for(i = 0; i < (MEMORY_DESCRIPTION_LENGTH - 1); i++){
			if(description[i] == 0x00){
				break;
			}
			block->description[i] = description[i];
		}
		block->description[i] = 0x00;
		return i;
	}
	return -1;
}

int MemoryBlock_Free(void *addr)
{
	Memory *next, *block;

	if(addr == 0){
		return -1;
	}

	for(next = &SystemMemory; next->next != 0; next = next->next){
		if(next->next->addr == addr){
			block = next->next;
			next->next = block->next;
			if(MemoryControl_Free(block->ctrl, block->addr, block->size) != 0){
				return -2;
			}
			if(MemoryControl_Free(SystemMemory.ctrl, block, sizeof(Memory)) != 0){
				return -3;
			}
			SystemMemory.size--;
			return 0;
		}
	}
	return -1;
}
