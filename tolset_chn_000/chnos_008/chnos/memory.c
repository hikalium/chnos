
#include "core.h"

uint memtest(uint start, uint end)
{

	char flg486 = 0;
	uint eflg,cr0,i;

	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT;
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if((eflg & EFLAGS_AC_BIT) != 0) flg486 = 1;
	eflg &= ~EFLAGS_AC_BIT;
	io_store_eflags(eflg);
	if(flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_ALL_CACHE_DISABLE;
		store_cr0(cr0);
		}
	i = memtest_sub(start, end);
	if(flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_ALL_CACHE_DISABLE;
		store_cr0(cr0);		
		}
	return i;

}

void memman_init(IO_MemoryControl *man)
{
	man->frees = 0;
	man->maxfrees = 0;
	man->lostsize = 0;
	man->losts = 0;
	return;
}

uint memman_free_total(IO_MemoryControl *man)
{
	uint i,t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
		}
	return t;
}

void *memman_alloc(IO_MemoryControl *man, uint size)
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
int memman_free(IO_MemoryControl *man, void *addr0, uint size)
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

void *memman_alloc_page(IO_MemoryControl *man)
{
	void *addr, *mem_head_4k, *offset;

	addr = memman_alloc(man, 0x2000);
	if(addr == 0) return 0;
	mem_head_4k = (void *)(((uint)addr + 0xfff) & 0xfffff000);
	(uint)offset = (uint)mem_head_4k - (uint)addr;
	if (offset > 0) {
		memman_free(man, addr, (uint)offset);
	}
	memman_free(man, mem_head_4k + 0x1000, 0x1000 - (uint)offset);

	return mem_head_4k;
}

void sys_memman_init(void)
{
	memman_init(&system.io.mem.ctrl);
}

uint sys_memman_free_total(void)
{
	return memman_free_total(&system.io.mem.ctrl);
}

void *sys_memman_alloc(uint size)
{
	return memman_alloc(&system.io.mem.ctrl, size);
}

int sys_memman_free(void *addr, uint size)
{
	return memman_free(&system.io.mem.ctrl, addr, size);
}

void *sys_memman_alloc_page(void)
{
	return memman_alloc_page(&system.io.mem.ctrl);
}

