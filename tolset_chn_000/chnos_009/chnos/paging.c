
#include "core.h"

uint *ADR_Paging_Directory;

void Initialise_Paging(void *vram, uint xsize, uint ysize, uint bpp)
{
	uint i, j, k, l, m, n, o, cr0;
	uint *page;

	j = (System_MemoryControl_FullSize() + 0x003FFFFF) >> 22;
	l = (((j << 22) - System_MemoryControl_FullSize()) + 0xFFF) >> 12;
	ADR_Paging_Directory = (uint *)System_MemoryControl_Allocate_Page();

	for(i = 0; i < 1024; i++){
		page = (uint *)System_MemoryControl_Allocate_Page();
		Paging_Set_Entry_Directory(&ADR_Paging_Directory[i], page, PG_NOTPRESENT, 0x00000000);
		for(k = 0; k < 1024; k++){
			Paging_Set_Entry_Table(&(Paging_Get_Entry_Setting_Address(ADR_Paging_Directory[i])[k]), (uint *)((i << 22) + (k * 1024 * 4)), PG_NOTPRESENT, 0x00000000);
		}
	}

Emergency_Out("Mem-Paging:Tables-Initialized");

	for(i = 0; i < j; i++){
		Paging_Set_Entry_Directory(&ADR_Paging_Directory[i], Paging_Get_Entry_Setting_Address(ADR_Paging_Directory[i]), PG_PRESENT | PG_WRITABLE | PG_USER | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
		if(i == j - 1 && l != 0){
			for(k = 0; k < l; k++){
				Paging_Set_Entry_Table(&(Paging_Get_Entry_Setting_Address(ADR_Paging_Directory[i])[k]), (uint *)((i << 22) + (k * 1024 * 4)), PG_PRESENT | PG_WRITABLE | PG_USER | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
			}
		} else{
			for(k = 0; k < 1024; k++){
				Paging_Set_Entry_Table(&(Paging_Get_Entry_Setting_Address(ADR_Paging_Directory[i])[k]), (uint *)((i << 22) + (k * 1024 * 4)), PG_PRESENT | PG_WRITABLE | PG_USER | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
			}
		}
	}

Emergency_Out("Mem-Paging:PhisMemory-Initialized");

	m = (uint)vram >> 22;
	n = xsize * ysize * (bpp >> 3);
	o = n >> 12;
	if(n != (o << 12)) o++;
	j = (o + 1023) >> 10;
	l = o - ((j - 1) << 10);

	for(i = 0; i < j; i++){
		Paging_Set_Entry_Directory(&ADR_Paging_Directory[m + i], Paging_Get_Entry_Setting_Address(ADR_Paging_Directory[m + i]), PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
		if(i == j - 1 && l != 0){
			for(k = 0; k < l; k++){
				Paging_Set_Entry_Table(&(Paging_Get_Entry_Setting_Address(ADR_Paging_Directory[m + i])[k]), (uint *)(((m + i) << 22) + (k * 1024 * 4)), PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
			}
		} else{
			for(k = 0; k < 1024; k++){
				Paging_Set_Entry_Table(&(Paging_Get_Entry_Setting_Address(ADR_Paging_Directory[m + i])[k]), (uint *)(((m + i) << 22) + (k * 1024 * 4)), PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR | PG_WRITEBACK | PG_CACHE_ENABLE | PG_NOTACCESSED | PG_NOTWRITTEN | PG_4KBPAGE | PG_NOTGLOBAL, 0x00000000);
			}
		}
	}

Emergency_Out("Mem-Paging:VideoMemory-Initialized");

	Store_CR3((uint)ADR_Paging_Directory);
	cr0 = Load_CR0();
	cr0 |= CR0_PAGING + CR0_PROTECTIONENABLE;
	Store_CR0(cr0);

Emergency_Out("Mem-Paging:Paging-Enabled");

	return;
}

void Paging_Set_Entry_Directory(uint *dir_entry, uint *table_base, uint attribute, uint available)
{
	uint entry;

	if(attribute & PG_PRESENT){
		entry = ((uint)table_base & 0xfffff000);
		entry |= (attribute & 0x000001ff);
		entry |= ((available & 0x00000007) << 9);
	} else{
		entry = ((uint)table_base & 0xfffff000);
		entry |= ((available & 0x00000007) << 9);
		entry &= 0xfffffffe;
	}
	*dir_entry = entry;

	return;
}

void Paging_Set_Entry_Table(uint *table_entry, uint *page_base, uint attribute, uint available)
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

uint *Paging_Get_Entry_Setting_Address(uint entry)
{
	return (uint *)(entry & 0xFFFFF000);
}
