
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

Memory *MemoryBlock_Allocate_System(uint size)
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
	return block;
}

Memory *MemoryBlock_Allocate_User(uint size, IO_MemoryControl *ctrl)
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

	block->size = size;
	block->next = SystemMemory.next;
	SystemMemory.next = block;
	SystemMemory.size++;
	block->ctrl = ctrl;
	block->description[0] = 0x00;
	return block;
}

bool MemoryBlock_Verify(Memory *block)
{
	Memory *next;

	for(next = &SystemMemory; next->next != 0; next = next->next){
		if(next->next == block){
			return true;
		}
	}
	return false;
}

int MemoryBlock_Write_Description(Memory *block, const uchar *s)
{
	uint i;

	if(MemoryBlock_Verify(block)){
		for(i = 0; i < (MEMORY_DESCRIPTION_LENGTH - 1); i++){
			if(s[i] == 0x00){
				break;
			}
			block->description[i] = s[i];
		}
		block->description[i] = 0x00;
		return i;
	}
	return -1;
}

int MemoryBlock_Free(Memory *block)
{
	Memory *next;

	for(next = &SystemMemory; next->next != 0; next = next->next){
		if(next->next == block){
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
