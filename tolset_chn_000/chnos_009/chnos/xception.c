
#include "core.h"

/*CPU Exceptions*/
char *cpu_exceptions[0x20] = {
	"Exception 0x00:\n\rDivided by zero.",
	"Exception 0x01:\n\rReserved.",
	"Exception 0x02:\n\rNonmaskable interrupt.",
	"Exception 0x03:\n\rBreakpoint.",
	"Exception 0x04:\n\rOverflow.",
	"Exception 0x05:\n\rOutside BOUND.",
	"Exception 0x06:\n\rInvalid opcode.",
	"Exception 0x07:\n\rDisable Device.",
	"Exception 0x08:\n\rDouble fault.",
	"Exception 0x09:\n\rCoprocessor Segment Overrun.",
	"Exception 0x0a:\n\rInvalid task status segment.",
	"Exception 0x0b:\n\rSegment absent.",
	"Exception 0x0c:\n\rStack Segment Fault.",
	"Exception 0x0d:\n\rGeneral Protection Exception.",
	"Exception 0x0e:\n\rPage fault.",
	"Exception 0x0f:\n\rReserved.",
	"Exception 0x10:\n\rFloating point error.",
	"Exception 0x11:\n\rAlignment Check.",
	"Exception 0x12:\n\rMachine Check.",
	"Exception 0x13:\n\rSIMD floating-point exception.",
	"Exception 0x14:\n\rReserved.",
	"Exception 0x15:\n\rReserved.",
	"Exception 0x16:\n\rReserved.",
	"Exception 0x17:\n\rReserved.",
	"Exception 0x18:\n\rReserved.",
	"Exception 0x19:\n\rReserved.",
	"Exception 0x1a:\n\rReserved.",
	"Exception 0x1b:\n\rReserved.",
	"Exception 0x1c:\n\rReserved.",
	"Exception 0x1d:\n\rReserved.",
	"Exception 0x1e:\n\rReserved.",
	"Exception 0x1f:\n\rReserved."
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

	sprintf(s, "%s\n\r", (uchar *)cpu_exceptions[exception]);
	Send_SerialPort(s);

	Send_SerialPort("#PUSHAD by _asm_CPU_ExceptionHandler\n\r");
	for(i = 0; i <= 7; i++){
		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		Send_SerialPort(s);
	}

	Send_SerialPort("#PUSH by _asm_CPU_ExceptionHandler\n\r");
	for(i = 8; i <= 9; i++){
		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		Send_SerialPort(s);
	}

	Send_SerialPort("#PUSH by CPU\n\r");
	for(i = 10; i <= 15; i++){
		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		Send_SerialPort(s);
	}

	Send_SerialPort("#Control Registers\n\r");
	sprintf(s, "CR0 = 0x%08X\n\r", Load_CR0());
	Send_SerialPort(s);
	sprintf(s, "CR2 = 0x%08X\n\r", Load_CR2());
	Send_SerialPort(s);
	sprintf(s, "CR3 = 0x%08X\n\r", Load_CR3());
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
