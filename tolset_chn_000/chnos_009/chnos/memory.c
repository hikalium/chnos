
#include "core.h"

IO_MemoryControl sys_mem_ctrl;
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

void MemoryControl_Initialise(IO_MemoryControl *ctrl, void *start, uint size)
{
	ctrl->start = start;
	ctrl->size = size;
	ctrl->next = start;
	((IO_MemoryControlTag *)ctrl->next)->size = ctrl->size;
	((IO_MemoryControlTag *)ctrl->next)->next = 0;
	return;
}

uint MemoryControl_FreeSize(IO_MemoryControl *ctrl)
{
	void *tag;
	uint size;

	size = 0;
	tag = ctrl->next;
	for(;;){
		size += ((IO_MemoryControlTag *)tag)->size;
		if(((IO_MemoryControlTag *)tag)->next == 0){
			break;
		}
		tag = ((IO_MemoryControlTag *)tag)->next;
	}
	return size;
}

void *MemoryControl_Allocate(IO_MemoryControl *ctrl, uint size)
{
	void **before;

	size = (size + 7) & ~7;

	before = &ctrl->next;
	for(;;){
		if((((IO_MemoryControlTag *)*before)->size - 8) > size){
			break;
		}
		if(((IO_MemoryControlTag *)*before)->next == 0){
			return 0;
		}
		before = &((IO_MemoryControlTag *)*before)->next;
	}
	((IO_MemoryControlTag *)(*before + size))->size = ((IO_MemoryControlTag *)*before)->size - size;
	((IO_MemoryControlTag *)(*before + size))->next = ((IO_MemoryControlTag *)*before)->next;
	*before = *before + size;
	return *before - size;
}

int MemoryControl_Free(IO_MemoryControl *ctrl, void *addr0, uint size)
{
	void **before;

	size = (size + 7) & ~7;

	before = &ctrl->next;
	for(;;){
		if((uint)((IO_MemoryControlTag *)*before) == (uint)(addr0 + size)){		//‰ð•ú‚µ‚½‚¢ƒƒ‚ƒŠ‚ÌŒã‘±‚É‹ó‚«‚ª‚ ‚éê‡B
			((IO_MemoryControlTag *)addr0)->size = ((IO_MemoryControlTag *)(*before))->size + size;
			((IO_MemoryControlTag *)addr0)->next = ((IO_MemoryControlTag *)(*before))->next;
			*before = addr0;
			return 0;
		}
		if((uint)((IO_MemoryControlTag *)*before)->next >= (uint)(addr0 + size)){
			Send_SerialPort("Have to Free!1 or 2\r\n");
			return 0;
		}
		if(((IO_MemoryControlTag *)*before)->next == 0){
			Send_SerialPort("Have to Free!0\r\n");
			return -1;
		}
		before = ((IO_MemoryControlTag *)*before)->next;
	}
}

void *MemoryControl_Allocate_Page(IO_MemoryControl *ctrl)
{
	void *addr, *mem_head_4k, *offset;

	addr = MemoryControl_Allocate(ctrl, 0x2000);
	if(addr == 0) return 0;
	mem_head_4k = (void *)(((uint)addr + 0xfff) & 0xfffff000);
	(uint)offset = (uint)mem_head_4k - (uint)addr;
	if (offset > 0) {
		MemoryControl_Free(ctrl, addr, (uint)offset);
	}
	MemoryControl_Free(ctrl, mem_head_4k + 0x1000, 0x1000 - (uint)offset);

	return mem_head_4k;
}

void MemoryControl_Output_Info(IO_MemoryControl *ctrl)
{
	void *tag;
	uchar s[128];

	Send_SerialPort("Memory Free Info.\r\n");

	tag = ctrl->next;
	for(;;){
		sprintf(s, "Addr:0x%08X Size:0x%08X\r\n", (uint)tag, ((IO_MemoryControlTag *)tag)->size);
		Send_SerialPort(s);
		if(((IO_MemoryControlTag *)tag)->next == 0){
			break;
		}
		tag = ((IO_MemoryControlTag *)tag)->next;
	}
	return;
}

void System_MemoryControl_Initialise(void)
{
	sys_mem_size = Memory_Test(0x00400000, 0xbfffffff) & 0xFFFFF000;
	MemoryControl_Initialise(&sys_mem_ctrl, (void *)0x00400000, sys_mem_size - 0x00400000);
	return;
}

uint System_MemoryControl_FullSize(void)
{
	return sys_mem_size;
}

uint System_MemoryControl_FreeSize(void)
{
	return MemoryControl_FreeSize(&sys_mem_ctrl);
}

void *System_MemoryControl_Allocate(uint size)
{
	return MemoryControl_Allocate(&sys_mem_ctrl, size);
}

int System_MemoryControl_Free(void *addr0, uint size)
{
	return MemoryControl_Free(&sys_mem_ctrl, addr0, size);
}

void *System_MemoryControl_Allocate_Page(void)
{
	return MemoryControl_Allocate_Page(&sys_mem_ctrl);
}

void System_MemoryControl_Output_Info(void)
{
	MemoryControl_Output_Info(&sys_mem_ctrl);
	return;
}
