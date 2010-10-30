
#include "core.h"

void sys_memman_init(void)
{
	memman_init(&system.sys.memman);
}

uint sys_memman_free_total(void)
{
	return memman_free_total(&system.sys.memman);
}

void *sys_memman_alloc(uint size)
{
	return memman_alloc_4k(&system.sys.memman, size);
}

int sys_memman_free(void *addr, uint size)
{
	return memman_free_4k(&system.sys.memman, addr, size);
}

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
		cr0 |= CR0_CACHE_DISABLE;
		store_cr0(cr0);
		}
	i = memtest_sub(start, end);
	if(flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;
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

void *memman_alloc_4k(IO_MemoryControl *man, uint size)
{
	void *a;
	size = (size + 0xfff) & 0xfffff000;
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(IO_MemoryControl *man, void *addr, uint size)
{
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
