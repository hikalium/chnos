
#include "core.h"

void init_paging(void)
{
	uint i, j, k, l, m, n, o, cr0;

	j = (system.io.mem.total + 1) >> 22;
	l = ((system.io.mem.total - ((j - 1) << 22)) + 4095) >> 12;

	for(i = 0; i < 1024; i++){
		paging_set_dir(&system.io.mem.paging.dir[i], &system.io.mem.paging.table[i][0], PG_NOTPRESENT, 0x00000000);
		for(k = 0; k < 1024; k++){
			paging_set_table(&system.io.mem.paging.table[i][k], (uint *)((i << 22) + (k * 1024 * 4)), PG_NOTPRESENT, 0x00000000);
		}
	}

	for(i = 0; i < j; i++){
		paging_set_dir(&system.io.mem.paging.dir[i], &system.io.mem.paging.table[i][0], PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
		if(i == j - 1){
			for(k = 0; k < l; k++){
				paging_set_table(&system.io.mem.paging.table[i][k], (uint *)((i << 22) + (k * 1024 * 4)), PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
			}
		} else{
			for(k = 0; k < 1024; k++){
				paging_set_table(&system.io.mem.paging.table[i][k], (uint *)((i << 22) + (k * 1024 * 4)), PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
			}
		}
	}

	m = (uint)system.data.info.vesa.PhysBasePtr >> 22;
	n = system.data.info.boot.scrnx * system.data.info.boot.scrny * (system.data.info.vesa.BitsPerPixel >> 3);
	o = n >> 12;
	if(n != (o << 12)) o++;
	j = (o + 1023) >> 10;
	l = o - ((j - 1) << 10);

	for(i = 0; i < j; i++){
		paging_set_dir(&system.io.mem.paging.dir[m + i], &system.io.mem.paging.table[m + i][0], PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
		if(i == j - 1){
			for(k = 0; k < l; k++){
				paging_set_table(&system.io.mem.paging.table[m + i][k], (uint *)(((m + i) << 22) + (k * 1024 * 4)), PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
			}
		} else{
			for(k = 0; k < 1024; k++){
				paging_set_table(&system.io.mem.paging.table[m + i][k], (uint *)(((m + i) << 22) + (k * 1024 * 4)), PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
			}
		}
	}

	store_cr3((uint)system.io.mem.paging.dir);
	cr0 = load_cr0();
	cr0 |= CR0_PAGING + CR0_PROTECTIONENABLE;
	store_cr0(cr0);
	pipelineflush();

	return;
}

void paging_set_dir(uint *dir_entry, uint *table_base, uint attribute, uint available)
{
	uint entry;

	if(attribute & PG_PRESENT){
		entry = ((uint)table_base & 0xfffff000);
		entry |= (attribute & 0x000001ff);
		entry |= ((available & 0x00000007) << 9);	
	} else{
		entry = (available << 1);
		entry &= 0xfffffffe;
	}
	*dir_entry = entry;
	return;
}

void paging_set_table(uint *table_entry, uint *page_base, uint attribute, uint available)
{
	uint entry;

	if(attribute & PG_PRESENT){
		entry = ((uint)page_base & 0xfffff000);
		entry |= (attribute & 0x000001ff);
		entry |= ((available & 0x00000007) << 9);	
	} else{
		entry = (available << 1);
		entry &= 0xfffffffe;
	}
	*table_entry = entry;
	return;
}
