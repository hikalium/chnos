
#include "core.h"

/*CPU Exceptions*/
char *cpu_exceptions[0x20] = {
	"Exception 0x00:\r\nDivided by zero.",
	"Exception 0x01:\r\nReserved.",
	"Exception 0x02:\r\nNonmaskable interrupt.",
	"Exception 0x03:\r\nBreakpoint.",
	"Exception 0x04:\r\nOverflow.",
	"Exception 0x05:\r\nOutside BOUND.",
	"Exception 0x06:\r\nInvalid opcode.",
	"Exception 0x07:\r\nDisable Device.",
	"Exception 0x08:\r\nDouble fault.",
	"Exception 0x09:\r\nCoprocessor Segment Overrun.",
	"Exception 0x0a:\r\nInvalid task status segment.",
	"Exception 0x0b:\r\nSegment absent.",
	"Exception 0x0c:\r\nStack Segment Fault.",
	"Exception 0x0d:\r\nGeneral Protection Exception.",
	"Exception 0x0e:\r\nPage fault.",
	"Exception 0x0f:\r\nReserved.",
	"Exception 0x10:\r\nFloating point error.",
	"Exception 0x11:\r\nAlignment Check.",
	"Exception 0x12:\r\nMachine Check.",
	"Exception 0x13:\r\nSIMD floating-point exception.",
	"Exception 0x14:\r\nReserved.",
	"Exception 0x15:\r\nReserved.",
	"Exception 0x16:\r\nReserved.",
	"Exception 0x17:\r\nReserved.",
	"Exception 0x18:\r\nReserved.",
	"Exception 0x19:\r\nReserved.",
	"Exception 0x1a:\r\nReserved.",
	"Exception 0x1b:\r\nReserved.",
	"Exception 0x1c:\r\nReserved.",
	"Exception 0x1d:\r\nReserved.",
	"Exception 0x1e:\r\nReserved.",
	"Exception 0x1f:\r\nReserved."
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
	uchar s[32];

	sprintf(s, "%s\r\n", (uchar *)cpu_exceptions[exception]);
	Send_SerialPort(s);

	Send_SerialPort("#PUSHAD by _asm_CPU_ExceptionHandler\r\n");
	for(i = 0; i <= 7; i++){
		sprintf(s, "%s:0x%08X\r\n", cpu_exception_infos[i], esp[i]);
		Send_SerialPort(s);
	}

	Send_SerialPort("#PUSH by _asm_CPU_ExceptionHandler\r\n");
	for(i = 8; i <= 9; i++){
		sprintf(s, "%s:0x%08X\r\n", cpu_exception_infos[i], esp[i]);
		Send_SerialPort(s);
	}

	Send_SerialPort("#PUSH by CPU\r\n");
	for(i = 10; i <= 15; i++){
		sprintf(s, "%s:0x%08X\r\n", cpu_exception_infos[i], esp[i]);
		Send_SerialPort(s);
	}

	Send_SerialPort("#Control Registers\r\n");
	sprintf(s, "CR0 = 0x%08X\r\n", Load_CR0());
	Send_SerialPort(s);
	sprintf(s, "CR2 = 0x%08X\r\n", Load_CR2());
	Send_SerialPort(s);
	sprintf(s, "CR3 = 0x%08X\r\n", Load_CR3());
	Send_SerialPort(s);

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
