
#include "core.h"

void CPU_Identify(DATA_CPUID *id)
{
	uint eflags0, eflags1;
	uint cpuidbuf[5];	//EAX-EBX-EDX-ECX-0x00000000
	uchar s[64];

	cpuidbuf[4] = 0x00000000;

	eflags0 = IO_Load_EFlags();
	IO_Store_EFlags(eflags0 | 0x00200000);
	eflags1 = IO_Load_EFlags();
	IO_Store_EFlags(eflags0);

	if((eflags1 | 0xffdfffff) == 0xffffffff){
		id->enable = true;
		CPUID(cpuidbuf, 0);
		id->max = cpuidbuf[0];
		sprintf(id->vendor, "%s", &cpuidbuf[1]);

		if(id->max >= 1){
			CPUID(cpuidbuf, 1);
			if((cpuidbuf[2] & ~1 << 0) != 0){
				id->FPU = true;
			} else{
				id->FPU = false;
			}
			if((cpuidbuf[2] & ~(1 << 1)) != 0){
				id->VME = true;
			} else{
				id->VME = false;
			}
			if((cpuidbuf[2] & ~(1 << 2)) != 0){
				id->DE = true;
			} else{
				id->DE = false;
			}
			if((cpuidbuf[2] & ~(1 << 3)) != 0){
				id->PSE = true;
			} else{
				id->PSE = false;
			}
			if((cpuidbuf[2] & ~(1 << 4)) != 0){
				id->TSC = true;
			} else{
				id->TSC = false;
			}
			if((cpuidbuf[2] & ~(1 << 5)) != 0){
				id->MSR = true;
			} else{
				id->MSR = false;
			}
			if((cpuidbuf[2] & ~(1 << 6)) != 0){
				id->PAE = true;
			} else{
				id->PAE = false;
			}
			if((cpuidbuf[2] & ~(1 << 7)) != 0){
				id->MCE = true;
			} else{
				id->MCE = false;
			}
			if((cpuidbuf[2] & ~(1 << 8)) != 0){
				id->CX8 = true;
			} else{
				id->CX8 = false;
			}
			if((cpuidbuf[2] & ~(1 << 9)) != 0){
				id->APIC = true;
			} else{
				id->APIC = false;
			}
			if((cpuidbuf[2] & ~(1 << 11)) != 0){
				id->SEP = true;
			} else{
				id->SEP = false;
			}
			if((cpuidbuf[2] & ~(1 << 12)) != 0){
				id->MTRR = true;
			} else{
				id->MTRR = false;
			}
			if((cpuidbuf[2] & ~(1 << 13)) != 0){
				id->PGE = true;
			} else{
				id->PGE = false;
			}
			if((cpuidbuf[2] & ~(1 << 14)) != 0){
				id->MCA = true;
			} else{
				id->MCA = false;
			}
			if((cpuidbuf[2] & ~(1 << 15)) != 0){
				id->CMOV = true;
			} else{
				id->CMOV = false;
			}
			if((cpuidbuf[2] & ~(1 << 16)) != 0){
				id->PAT = true;
			} else{
				id->PAT = false;
			}
			if((cpuidbuf[2] & ~(1 << 17)) != 0){
				id->PSE36 = true;
			} else{
				id->PSE36 = false;
				}
			if((cpuidbuf[2] & ~(1 << 18)) != 0){
				id->PSN = true;
			} else{
				id->PSN = false;
			}
			if((cpuidbuf[2] & ~(1 << 19)) != 0){
				id->CLFSH = true;
			} else{
				id->CLFSH = false;
			}
			if((cpuidbuf[2] & ~(1 << 21)) != 0){
				id->DS = true;
			} else{
				id->DS = false;
			}
			if((cpuidbuf[2] & ~(1 << 22)) != 0){
				id->ACPI = true;
			} else{
				id->ACPI = false;
			}
			if((cpuidbuf[2] & ~(1 << 23)) != 0){
				id->MMX = true;
			} else{
				id->MMX = false;
			}
			if((cpuidbuf[2] & ~(1 << 24)) != 0){
				id->FXSR = true;
			} else{
				id->FXSR = false;
			}
			if((cpuidbuf[2] & ~(1 << 25)) != 0){
				id->SSE = true;
			} else{
				id->SSE = false;
			}
			if((cpuidbuf[2] & ~(1 << 26)) != 0){
				id->SSE2 = true;
			} else{
				id->SSE2 = false;
			}
			if((cpuidbuf[2] & ~(1 << 27)) != 0){
				id->SS = true;
			} else{
				id->SS = false;
			}
			if((cpuidbuf[2] & ~(1 << 28)) != 0){
				id->HTT = true;
			} else{
				id->HTT = false;
			}
			if((cpuidbuf[2] & ~(1 << 29)) != 0){
				id->TM = true;
			} else{
				id->TM = false;
			}
			if((cpuidbuf[2] & ~(1 << 31)) != 0){
				id->PBE = true;
			} else{
				id->PBE = false;
			}

			if((cpuidbuf[3] & ~(1 << 0)) != 0){
				id->SSE3 = true;
			} else{
				id->SSE3 = false;
			}
			if((cpuidbuf[3] & ~(1 << 3)) != 0){
				id->MONITOR = true;
			} else{
				id->MONITOR = false;
			}
			if((cpuidbuf[3] & ~(1 << 4)) != 0){
				id->DSCPL = true;
			} else{
				id->DSCPL = false;
			}
			if((cpuidbuf[3] & ~(1 << 7)) != 0){
				id->EST = true;
			} else{
				id->EST = false;
			}
			if((cpuidbuf[3] & ~(1 << 8)) != 0){
				id->TM2 = true;
			} else{
				id->TM2 = false;
			}
			if((cpuidbuf[3] & ~(1 << 10)) != 0){
				id->CID = true;
			} else{
				id->CID = false;
			}
			if((cpuidbuf[3] & ~(1 << 13)) != 0){
				id->CX16 = true;
			} else{
				id->CX16 = false;
			}
			if((cpuidbuf[3] & ~(1 << 14)) != 0){
				id->xTPR = true;
			} else{
				id->xTPR = false;
			}
		}
		CPUID(cpuidbuf, 0x80000000);
		if(cpuidbuf[0] >= 0x80000000){
			id->ext_enable = true;
			id->ext_max = cpuidbuf[0];
			if(cpuidbuf[0] >= 0x80000004){
				CPUID2(&s[0], 0x80000002);
				CPUID2(&s[16], 0x80000003);
				CPUID2(&s[32], 0x80000004);
				sprintf(id->brand_string, "%s", s);
			} else{
				sprintf(id->brand_string, "Null");
			}
		} else{
			id->ext_enable = false;
			id->ext_max = 0;
		}
	} else{
		id->enable = false;
		id->max = 0;
		sprintf(id->vendor, "Null");
	}
	return;
}
