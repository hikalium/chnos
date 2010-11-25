
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
