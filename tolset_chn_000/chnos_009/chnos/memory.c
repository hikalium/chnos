
#include "core.h"

IO_MemoryControl *sys_mem_ctrl;
uint sys_mem_size;

uint Memory_Test(uint start, uint end)
{
	uchar flg486 = 0;
	uint eflg, cr0, i;

	eflg = IO_Load_EFlags();
	eflg |= EFLAGS_AC_BIT;
	IO_Store_EFlags(eflg);

	eflg = IO_Load_EFlags();
	if((eflg & EFLAGS_AC_BIT) != 0){
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT;
	IO_Store_EFlags(eflg);

	if(flg486 != 0){
		cr0 = Load_CR0();
		cr0 |= CR0_ALL_CACHE_DISABLE;
		Store_CR0(cr0);
	}
	i = Memory_Test_Sub(start, end);
	if(flg486 != 0){
		cr0 = Load_CR0();
		cr0 &= ~CR0_ALL_CACHE_DISABLE;
		Store_CR0(cr0);		
	}
	return i;
}

void MemoryControl_Initialise(IO_MemoryControl *man)
{
	man->frees = 0;
	man->maxfrees = 0;
	man->lostsize = 0;
	man->losts = 0;
	return;
}

uint MemoryControl_FreeSize(IO_MemoryControl *man)
{
	uint i,t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
		}
	return t;
}

void *MemoryControl_Allocate(IO_MemoryControl *man, uint size)
{
	uint i,a;
	for(i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
		a = man->free[i].addr;
		man->free[i].addr += size;
		man->free[i].size -= size;
		if(man->free[i].size == 0) {
			man->frees--;
			for (; i < man->frees; i++) {
				man->free[i] = man->free[i+1];
				}
			}
			return (void *)a;
		}
	}
	return 0;
}

int MemoryControl_Free(IO_MemoryControl *man, void *addr0, uint size)
{
	int i, j;
	uint addr;
	addr = (uint)addr0;
	for(i = 0; i < man->frees; i++){
		if(man->free[i].addr > addr) break;
	}
	if(i > 0) {
		if(man->free[i-1].addr + man->free[i-1].size == addr){
			man->free[i-1].size += size;
			if(i < man->frees){
				if(addr + size == man->free[i].addr){
					man->free[i-1].size += man->free[i].size;
					man->frees--;
					for (;i < man->frees; i++){
						man->free[i] = man->free[i+1];
					}
				}
			}
			return 0;
		}
	}
	if(i < man->frees){
		if(addr + size == man->free[i].addr){
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0;
		}
	}
	if(man->frees < MEMMAN_FREES){
		for(j = man->frees;j>i;j--) {
			man->free[j] = man->free[j-1];

		}
		man->frees++;
		if(man->maxfrees < man->frees) man->maxfrees = man->frees;
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0;

	}
	man->losts++;
	man->lostsize += size;
	return -1;
}

void *MemoryControl_Allocate_Page(IO_MemoryControl *man)
{
	void *addr, *mem_head_4k, *offset;

	addr = MemoryControl_Allocate(man, 0x2000);
	if(addr == 0) return 0;
	mem_head_4k = (void *)(((uint)addr + 0xfff) & 0xfffff000);
	(uint)offset = (uint)mem_head_4k - (uint)addr;
	if (offset > 0) {
		MemoryControl_Free(man, addr, (uint)offset);
	}
	MemoryControl_Free(man, mem_head_4k + 0x1000, 0x1000 - (uint)offset);

	return mem_head_4k;
}

void System_MemoryControl_Initialise(void)
{
	sys_mem_size = Memory_Test(0x00400000, 0xbfffffff) & 0xFFFFF000;
	sys_mem_ctrl = (IO_MemoryControl *)(sys_mem_size - sizeof(IO_MemoryControl));
	MemoryControl_Initialise(sys_mem_ctrl);
	System_MemoryControl_Free((void *)0x00400000, sys_mem_size - (sizeof(IO_MemoryControl) + 0x00400000));
	return;
}

uint System_MemoryControl_FullSize(void)
{
	return sys_mem_size;
}

uint System_MemoryControl_FreeSize(void)
{
	return MemoryControl_FreeSize(sys_mem_ctrl);
}

void *System_MemoryControl_Allocate(uint size)
{
	return MemoryControl_Allocate(sys_mem_ctrl, size);
}

int System_MemoryControl_Free(void *addr0, uint size)
{
	return MemoryControl_Free(sys_mem_ctrl, addr0, size);
}

void *System_MemoryControl_Allocate_Page(void)
{
	return MemoryControl_Allocate_Page(sys_mem_ctrl);
}

