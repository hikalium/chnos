
#include "core.h"

void init_pic(void)
{
	/*
	IRR�F�C���^���v�g���N�G�X�g���W�X�^ 
		.IRQ�s���̏�Ԃ�\���B�|�[�g��ǂ񂾂Ƃ��ɁAIRR�ɂȂ邩ISR�ɂȂ邩�́AOCW3�őI������B
		.1�ɂȂ��Ă���r�b�g�́A���ݗv�������Ă���i�������͏������́j���荞�݁B
	ISR�F�C���T�[�r�X���W�X�^ 
		.���ݏ������̊��荞�݂��ǂ�ł��邩�������B�|�[�g��ǂ񂾂Ƃ��ɁAIRR�ɂȂ邩ISR�ɂȂ邩�́AOCW3�őI������B
		.1�ɂȂ��Ă���r�b�g�́A���ݏ������̊��荞�݁B�������Ƃ����̂́ACPU�ɑ΂���INT���߂𔭍s�������AEOI�i���荞�ݏI�����߁j���󂯎���Ă��Ȃ����荞�݁B
	IMR�F�C���^���v�g�}�X�N���W�X�^ 
		.���ꂪ1�ɂȂ��Ă��銄�荞�݂́AIRR��1�ɂȂ��Ă��Ă��A�������Ȃ��B 
	*/

	io_out8(PIC0_IMR, 0xff);	/*���荞�ݑS�������i�}�X�^�j*/
	io_out8(PIC1_IMR, 0xff);	/*���荞�ݑS�������i�X���[�u�j*/

	io_out8(PIC0_ICW1, 0x11);	/*�G�b�W�g���K���[�h�ɐݒ�i�}�X�^�j*/
	io_out8(PIC0_ICW2, 0x20);	/*���荞�ݔԍ����A20~27�ɐݒ�B�i�}�X�^�j*/
	io_out8(PIC0_ICW3, 1 << 2);	/*00000100 �܂�A�X���[�u��IRQ2�ɂȂ����Ă܂���Ƃ������ƁB*/
	io_out8(PIC0_ICW4, 0x01);	/*�m���o�b�t�@���[�h�i�}�X�^�j*/

	io_out8(PIC1_ICW1, 0x11);	/*�G�b�W�g���K���[�h�ɐݒ�i�X���[�u�j*/
	io_out8(PIC1_ICW2, 0x28);	/*���荞�ݔԍ����A28~2f�ɐݒ�B�i�X���[�u�j*/
	io_out8(PIC1_ICW3, 2);	/*�����̓}�X�^��IRQ2�ԂɂȂ����Ă܂��Ƃ������ƁB*/
	io_out8(PIC1_ICW4, 0x01);	/*�m���o�b�t�@���[�h�i�X���[�u�j*/

	io_out8(PIC0_IMR, 0xfb);	/*11111011�܂�AIRQ2�ԁi�X���[�u�j�������B���Ƃ͖����B�i�}�X�^�j*/	
	io_out8(PIC1_IMR, 0xff);	/*11111111�܂�A�S�Ė���*/

	set_gatedesc(system.io.interrupt.idt+0x27, (int)asm_inthandler27, 2 * 8, AR_INTGATE32);
	io_out8(PIC0_IMR, io_in8(PIC0_IMR) & 0x7f);	/*IRQ-07�΍�*/

	return;
}

void inthandler27(int *esp)
{
	io_out8(PIC0_OCW2, 0x67);	/* IRQ-07��t������PIC�ɒʒm �B0x60+�ԍ��B*/
	return;
}

/*CPU Exceptions*/
char *cpu_exceptions[0x20] = {
	"Exception 0x00:Divided by zero.",
	"Exception 0x01:Reserved.",
	"Exception 0x02:Nonmaskable interrupt.",
	"Exception 0x03:Breakpoint.",
	"Exception 0x04:Overflow.",
	"Exception 0x05:Outside BOUND.",
	"Exception 0x06:Invalid opcode.",
	"Exception 0x07:Disable Device.",
	"Exception 0x08:Double fault.",
	"Exception 0x09:Coprocessor Segment Overrun.",
	"Exception 0x0a:Invalid task status segment.",
	"Exception 0x0b:Segment absent.",
	"\nException 0x0c:Stack Segment Fault.\n",
	"\nException 0x0d:General Protection Exception.\n",
	"Exception 0x0e:Page fault.",
	"Exception 0x0f:Reserved.",
	"Exception 0x10:Floating point error.",
	"Exception 0x11:Alignment Check.",
	"Exception 0x12:Machine Check.",
	"Exception 0x13:SIMD floating-point exception.",
	"Exception 0x14:Reserved.",
	"Exception 0x15:Reserved.",
	"Exception 0x16:Reserved.",
	"Exception 0x17:Reserved.",
	"Exception 0x18:Reserved.",
	"Exception 0x19:Reserved.",
	"Exception 0x1a:Reserved.",
	"Exception 0x1b:Reserved.",
	"Exception 0x1c:Reserved.",
	"Exception 0x1d:Reserved.",
	"Exception 0x1e:Reserved.",
	"Exception 0x1f:Reserved."
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

void cpu_exception_abort(int exception, int *esp)
{
	int i;
	char s[32];

//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 0, 0xFFFFFF, (const uchar *)cpu_exceptions[exception]);
//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 16, 0xFFFFFF, "***registers info***");

	sprintf(s, "%s\n\r", (uchar *)cpu_exceptions[exception]);
	send_serial(s);

//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 32, 0xFFFFFF, "#PUSHAD by _asm_inthandler");

	send_serial("#PUSHAD by _asm_inthandler\n\r");

	for(i = 0; i <= 7; i++){
//		sprintf(s, "%s:0x%08X", cpu_exception_infos[i], esp[i]);
//		putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 16 * (i + 3), 0xFFFFFF, s);

		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		send_serial(s);
	}

//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 176, 0xFFFFFF, "#PUSH by _asm_inthandler");
	send_serial("#PUSH by _asm_inthandler\n\r");

	for(i = 8; i <= 9; i++){
//		sprintf(s, "%s:0x%08X", cpu_exception_infos[i], esp[i]);
//		putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 16 * (i + 4), 0xFFFFFF, s);

		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		send_serial(s);
	}

//	putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 224, 0xFFFFFF, "#PUSH by CPU");

	send_serial("#PUSH by CPU\n\r");
	for(i = 10; i <= 15; i++){
//		sprintf(s, "%s:0x%08X", cpu_exception_infos[i], esp[i]);
//		putfonts32_asc(system.data.info.vesa.PhysBasePtr, system.data.info.boot.scrnx, 0, 16 * (i + 5), 0xFFFFFF, s);

		sprintf(s, "%s:0x%08X\n\r", cpu_exception_infos[i], esp[i]);
		send_serial(s);
	}

	send_serial("#Control Registers\n\r");
	sprintf(s, "CR0 = 0x%08X\n\r", load_cr0());
	send_serial(s);
	sprintf(s, "CR2 = 0x%08X\n\r", load_cr2());
	send_serial(s);
	sprintf(s, "CR3 = 0x%08X\n\r", load_cr3());
	send_serial(s);

	for(;;){
		io_hlt();
	}
}

void inthandler00(int *esp)
{
	cpu_exception_abort(0x00, esp);
}

void inthandler01(int *esp)
{
	cpu_exception_abort(0x01, esp);
}

void inthandler02(int *esp)
{
	cpu_exception_abort(0x02, esp);
}

void inthandler03(int *esp)
{
	cpu_exception_abort(0x03, esp);
}

void inthandler04(int *esp)
{
	cpu_exception_abort(0x04, esp);
}

void inthandler05(int *esp)
{
	cpu_exception_abort(0x05, esp);
}

void inthandler06(int *esp)
{
	cpu_exception_abort(0x06, esp);
}

void inthandler07(int *esp)
{
	cpu_exception_abort(0x07, esp);
}

void inthandler08(int *esp)
{
	cpu_exception_abort(0x08, esp);
}

void inthandler09(int *esp)
{
	cpu_exception_abort(0x09, esp);
}

void inthandler0a(int *esp)
{
	cpu_exception_abort(0x0a, esp);
}

void inthandler0b(int *esp)
{
	cpu_exception_abort(0x0b, esp);
}

void inthandler0c(int *esp)
{
	cpu_exception_abort(0x0c, esp);
}

void inthandler0d(int *esp)
{
	cpu_exception_abort(0x0d, esp);
}

void inthandler0e(int *esp)
{
	cpu_exception_abort(0x0e, esp);
}

void inthandler0f(int *esp)
{
	cpu_exception_abort(0x0f, esp);
}

void inthandler10(int *esp)
{
	cpu_exception_abort(0x10, esp);
}

void inthandler11(int *esp)
{
	cpu_exception_abort(0x11, esp);
}

void inthandler12(int *esp)
{
	cpu_exception_abort(0x12, esp);
}

void inthandler13(int *esp)
{
	cpu_exception_abort(0x13, esp);
}

void inthandler14(int *esp)
{
	cpu_exception_abort(0x14, esp);
}

void inthandler15(int *esp)
{
	cpu_exception_abort(0x15, esp);
}

void inthandler16(int *esp)
{
	cpu_exception_abort(0x16, esp);
}

void inthandler17(int *esp)
{
	cpu_exception_abort(0x17, esp);
}

void inthandler18(int *esp)
{
	cpu_exception_abort(0x18, esp);
}

void inthandler19(int *esp)
{
	cpu_exception_abort(0x19, esp);
}

void inthandler1a(int *esp)
{
	cpu_exception_abort(0x1a, esp);
}

void inthandler1b(int *esp)
{
	cpu_exception_abort(0x1b, esp);
}

void inthandler1c(int *esp)
{
	cpu_exception_abort(0x1c, esp);
}

void inthandler1d(int *esp)
{
	cpu_exception_abort(0x1d, esp);
}

void inthandler1e(int *esp)
{
	cpu_exception_abort(0x1e, esp);
}

void inthandler1f(int *esp)
{
	cpu_exception_abort(0x1f, esp);
}
