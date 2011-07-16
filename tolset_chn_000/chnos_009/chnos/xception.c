
#include "core.h"

/*CPU Exceptions*/
char *cpu_exceptions[0x20] = {
	"Divided by zero.",
	"Reserved.",
	"Nonmaskable interrupt.",
	"Breakpoint.",
	"Overflow.",
	"Outside BOUND.",
	"Invalid opcode.",
	"Disable Device.",
	"Double fault.",
	"Coprocessor Segment Overrun.",
	"Invalid task status segment.",
	"Segment not present.",
	"Stack Segment Fault.",
	"General Protection Exception.",
	"Page fault.",
	"Reserved.",
	"Floating point error.",
	"Alignment Check.",
	"Machine Check.",
	"SIMD floating-point exception.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved.",
	"Reserved."
};

char *cpu_exception_infos[16] = {
	"EDI      ",
	"ESI      ",
	"EBP      ",
	"ESP      ",
	"EBX      ",
	"EDX      ",
	"ECX      ",
	"EAX      ",
	"DS       ",
	"ES       ",
	"ERRORCODE",
	"EIP      ",
	"CS       ",
	"EFLAGS   ",
	"APP's ESP",
	"APP's SS "
};

void CPU_Exception_Abort(int exception, int *esp)
{
	int i;

Emergency_Out_Reset();
Emergency_Out("Exception 0x%02X:%s", exception, (uchar *)cpu_exceptions[exception]);
for(i = 0; i < 8; i++){
	Emergency_Out("%s0x%08X %s0x%08X", cpu_exception_infos[i << 1], esp[i << 1], cpu_exception_infos[(i << 1) + 1], esp[(i << 1) + 1]);
}
Emergency_Out("CR0 = 0x%08X", Load_CR0());
Emergency_Out("CR2 = 0x%08X", Load_CR2());
Emergency_Out("CR3 = 0x%08X", Load_CR3());

	debug("Exception 0x%02X:\n%s\n", exception, (uchar *)cpu_exceptions[exception]);

	debug("#PUSHAD by _asm_CPU_ExceptionHandler\n");
	for(i = 0; i <= 7; i++){
		debug("%s:0x%08X\n", cpu_exception_infos[i], esp[i]);
	}

	debug("#PUSH by _asm_CPU_ExceptionHandler\n");
	for(i = 8; i <= 9; i++){
		debug("%s:0x%08X\n", cpu_exception_infos[i], esp[i]);
	}

	debug("#PUSH by CPU\n");
	for(i = 10; i <= 15; i++){
		debug("%s:0x%08X\n", cpu_exception_infos[i], esp[i]);
	}

	debug("#Control Registers\n");
	debug("CR0 = 0x%08X\n", Load_CR0());
	debug("CR2 = 0x%08X\n", Load_CR2());
	debug("CR3 = 0x%08X\n", Load_CR3());

	for(;;){
		IO_HLT();
	}
}

void CPU_ExceptionHandler00(int *esp)
{
	CPU_Exception_Abort(0x00, esp);
}

void CPU_ExceptionHandler01(int *esp)
{
	CPU_Exception_Abort(0x01, esp);
}

void CPU_ExceptionHandler02(int *esp)
{
	CPU_Exception_Abort(0x02, esp);
}

void CPU_ExceptionHandler03(int *esp)
{
	CPU_Exception_Abort(0x03, esp);
}

void CPU_ExceptionHandler04(int *esp)
{
	CPU_Exception_Abort(0x04, esp);
}

void CPU_ExceptionHandler05(int *esp)
{
	CPU_Exception_Abort(0x05, esp);
}

void CPU_ExceptionHandler06(int *esp)
{
	CPU_Exception_Abort(0x06, esp);
}

void CPU_ExceptionHandler07(int *esp)
{
	CPU_Exception_Abort(0x07, esp);
}

void CPU_ExceptionHandler08(int *esp)
{
	CPU_Exception_Abort(0x08, esp);
}

void CPU_ExceptionHandler09(int *esp)
{
	CPU_Exception_Abort(0x09, esp);
}

void CPU_ExceptionHandler0a(int *esp)
{
	CPU_Exception_Abort(0x0a, esp);
}

void CPU_ExceptionHandler0b(int *esp)
{
	CPU_Exception_Abort(0x0b, esp);
}

void CPU_ExceptionHandler0c(int *esp)
{
	CPU_Exception_Abort(0x0c, esp);
}

void CPU_ExceptionHandler0d(int *esp)
{
	CPU_Exception_Abort(0x0d, esp);
}

void CPU_ExceptionHandler0e(int *esp)
{
	CPU_Exception_Abort(0x0e, esp);
}

void CPU_ExceptionHandler0f(int *esp)
{
	CPU_Exception_Abort(0x0f, esp);
}

void CPU_ExceptionHandler10(int *esp)
{
	CPU_Exception_Abort(0x10, esp);
}

void CPU_ExceptionHandler11(int *esp)
{
	CPU_Exception_Abort(0x11, esp);
}

void CPU_ExceptionHandler12(int *esp)
{
	CPU_Exception_Abort(0x12, esp);
}

void CPU_ExceptionHandler13(int *esp)
{
	CPU_Exception_Abort(0x13, esp);
}

void CPU_ExceptionHandler14(int *esp)
{
	CPU_Exception_Abort(0x14, esp);
}

void CPU_ExceptionHandler15(int *esp)
{
	CPU_Exception_Abort(0x15, esp);
}

void CPU_ExceptionHandler16(int *esp)
{
	CPU_Exception_Abort(0x16, esp);
}

void CPU_ExceptionHandler17(int *esp)
{
	CPU_Exception_Abort(0x17, esp);
}

void CPU_ExceptionHandler18(int *esp)
{
	CPU_Exception_Abort(0x18, esp);
}

void CPU_ExceptionHandler19(int *esp)
{
	CPU_Exception_Abort(0x19, esp);
}

void CPU_ExceptionHandler1a(int *esp)
{
	CPU_Exception_Abort(0x1a, esp);
}

void CPU_ExceptionHandler1b(int *esp)
{
	CPU_Exception_Abort(0x1b, esp);
}

void CPU_ExceptionHandler1c(int *esp)
{
	CPU_Exception_Abort(0x1c, esp);
}

void CPU_ExceptionHandler1d(int *esp)
{
	CPU_Exception_Abort(0x1d, esp);
}

void CPU_ExceptionHandler1e(int *esp)
{
	CPU_Exception_Abort(0x1e, esp);
}

void CPU_ExceptionHandler1f(int *esp)
{
	CPU_Exception_Abort(0x1f, esp);
}
