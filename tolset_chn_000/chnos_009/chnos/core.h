
/*include files*/
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

int rand(void);
#define srand(seed)	(void) (rand_seed = (seed))
extern unsigned int rand_seed;

/*definemacros*/
#define RGB16(r,g,b) ((r)<<11|(g)<<5|(b))

/*settings*/

#include "core_set.h"

/*new object types*/
typedef enum _bool { false, true} bool;
typedef enum _state_alloc { none, initialized, allocated, configured, inuse} state_alloc;
typedef enum _col_text { black, blue, green, skyblue, red, purple, brown, white} col_text;
typedef enum _timer_mode { once, interval} timer_mode;
typedef enum _mcursor_state { normal, wait} mcursor_state;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

/*structures*/
struct SEGMENT_DESCRIPTOR { 
	short limit_low,base_low;
	char base_mid,access_right;
	char limit_high,base_high;
};

struct GATE_DESCRIPTOR { 
	short offset_low,selector;
	char dw_count,access_right;
	short offset_high;
};

struct BOOTINFO { 
	uchar cyls; 
	uchar leds; 
	uchar vmode; 
	uchar reserve;
	ushort scrnx, scrny;
	uchar *vram;
};

struct VESAINFO {/*0xe00--->512byte*/
	ushort	ModeAttributes;
	uchar	WinAAttributes;
	uchar	WinBAttributes;
	ushort	WinGranularity;
	ushort	WinSize;
	ushort	WinASegment;
	ushort	WinBSegment;
	uint	WinFuncPtr;
	ushort	BytesPerScanLine;
	ushort	XResolution;
	ushort	YResolution;
	uchar	XCharSize;
	uchar	YCharSize;
	uchar	NumberOfPlanes;
	uchar	BitsPerPixel;
	uchar	NumberOfBanks;
	uchar	MemoryModel;
	uchar	BankSize;
	uchar	NumberOfImagePages;
	uchar	Reserved;
	uchar	RedMaskSize;
	uchar	RedFieldPosition;
	uchar	GreenMaskSize;
	uchar	GreenFieldPosition;
	uchar	BlueMaskSize;
	uchar	BlueFieldPosition;
	uchar	RsvdMaskSize;
	uchar	RsvdFieldPodition;
	uchar	DirectColorModeInfo;
	uint*	PhysBasePtr;
};

struct FIFO32 {
	uint *buf;
	uint p, q, size, free, flags;
};

struct MEMORY_CONTROL_TAG {
	uint size;
	void *next;
};

struct MEMORY_CONTROL {
	void *start;
	uint size;
	void *next;
};

struct POSITION_2D {
	int x, y;
};

struct KEYINFO {
	uchar c;
	uint keycode;
	bool make;
	bool alphabet;
};

struct TIMER {
	struct TIMER *next;
	struct TIMER *tree;
	uint timeout;
	uint count;
	struct FIFO32 *fifo;
	uint data;
	state_alloc state;
	timer_mode mode;
};

struct TIMER_CONTROL {
	uint count;
	struct TIMER *next;
};

struct SHEET {
	void *vram;
	struct POSITION_2D position;
	struct POSITION_2D size;
	uint bpp;
	uint invcol;
	struct SHEET *next;
	struct SHEET *before;
	void (*Refresh)(struct SHEET *sheet, int px0, int py0, int px1, int py1);
	void (*WriteMap)(struct SHEET *sheet, int x0, int y0, int x1, int y1);
	bool visible;
	struct SHEET_CONTROL *myctrl;
};

struct SHEET_CONTROL {
	void *mainvram;
	uint *map;
	struct POSITION_2D mainvramsize;
	uint mainvrambpp;
	struct SHEET base;
	uint sheets;
	struct MEMORY_CONTROL *memctrl;
};

struct UI_INPUTBOX {
	struct SHEET *sheet;
//	void *vram;
	uint forecol, backcol;
	uchar *input_buf;
	uint input_buf_size;
//	uint vxsize;
	uint input_count;
	struct POSITION_2D cursor;
	struct POSITION_2D prompt;
//	struct POSITION_2D size;
//	struct POSITION_2D position;
	bool cursor_state;
};

struct MOUSE_DECODE {
	uint buf[4], scrool;
	struct POSITION_2D move;
	int btn, type;
	uchar phase; 
};

struct UI_MOUSE_CURSOR {
	struct SHEET *sheet;
	struct POSITION_2D position;
	struct POSITION_2D move_range0;
	struct POSITION_2D move_range1;
	mcursor_state state;
};

/*typedef structures*/
typedef struct SEGMENT_DESCRIPTOR	IO_SegmentDescriptor;
typedef struct GATE_DESCRIPTOR		IO_GateDescriptor;
typedef struct BOOTINFO			DATA_BootInfo;
typedef struct VESAINFO			DATA_VESAInfo;
typedef struct FIFO32			DATA_FIFO;
typedef struct MEMORY_CONTROL_TAG	IO_MemoryControlTag;
typedef struct MEMORY_CONTROL		IO_MemoryControl;
typedef struct POSITION_2D		DATA_Position2D;
typedef struct UI_INPUTBOX		UI_InputBox;
typedef struct KEYINFO			UI_KeyInfo;
typedef struct TIMER_CONTROL		UI_TimerControl;
typedef struct TIMER			UI_Timer;
typedef struct SHEET_CONTROL		UI_Sheet_Control;
typedef struct SHEET			UI_Sheet;
typedef struct MOUSE_DECODE		UI_MouseInfo;
typedef struct UI_MOUSE_CURSOR		UI_MouseCursor;

/*virtual classes*/

/*externs*/
extern uchar hankaku[4096];

/*functions*/
/*bootpack.c �����*/

/*dsctbl.c �f�B�X�N���v�^�[�E�e�[�u���֘A*/
void Initialise_GlobalDescriptorTable(void);
void Initialise_InterruptDescriptorTable(void);
void Set_SegmentDescriptor(IO_SegmentDescriptor *sd, uint limit, int base, int ar);
void Set_GateDescriptor(IO_GateDescriptor *gd, int offset, int selector, int ar);

/*fifo.c FIFO�o�b�t�@�֘A*/
int FIFO32_Initialise(DATA_FIFO *fifo, uint size);
int FIFO32_Put(DATA_FIFO *fifo, uint data);
uint FIFO32_Get(DATA_FIFO *fifo);
uint FIFO32_Status(DATA_FIFO *fifo);
int FIFO32_Free(DATA_FIFO *fifo);

/*grap_08.c*/
void Draw_Put_Font_08(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font);
void Draw_Put_String_08(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s);
void Draw_Fill_Rectangle_08(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void Draw_Slide_Line_08(void *vram, uint xsize, uint ysize, uint vxsize, uint px, uint py);
void Draw_Initialise_Palette(void);
void Draw_Set_Palette(int start, int end, uchar *rgb);
/*grap_16.c*/
void Draw_Put_Font_16(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font);
void Draw_Put_String_16(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s);
void Draw_Fill_Rectangle_16(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void Draw_Slide_Line_16(void *vram, uint xsize, uint ysize, uint vxsize, uint px, uint py);
/*grap_32.c*/
void Draw_Put_Font_32(void *vram, uint xsize, uint x, uint y, uint c, const uchar *font);
void Draw_Put_String_32(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s);
void Draw_Fill_Rectangle_32(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
void Draw_Slide_Line_32(void *vram, uint xsize, uint ysize, uint vxsize, uint px, uint py);
/*graphic.c*/
void Initialise_Graphic(uint bpp);
uchar RGB_32_To_08(uint c32);
uchar RGB_32_To_08_xy(uint c32, int x, int y);
ushort RGB_32_To_16(uint c32);
extern void (*Draw_Put_String)(void *vram, uint xsize, uint x, uint y, uint c, const uchar *s);
extern void (*Draw_Fill_Rectangle)(void *vram, uint xsize, uint c, uint x0, uint y0, uint x1, uint y1);
extern void (*Draw_Slide_Line)(void *vram, uint xsize, uint ysize, uint vxsize, uint px, uint py);

/*init.c*/
void Initialise_System(DATA_FIFO *fifo, DATA_FIFO *keycmd, uint *keycmd_wait, UI_MouseInfo *decode);

/*inputbox.c*/
void InputBox_Initialise(UI_Sheet_Control *sheetctrl, IO_MemoryControl *memctrl, UI_InputBox *box, uint x, uint y, uint xsize, uint ysize, uint txtbufsize, uint forecol, uint backcol, uint height);
int InputBox_Put_String(UI_InputBox *box, const uchar *s);
int InputBox_Put_Character(UI_InputBox *box, uchar c);
void InputBox_Put_String_Main(UI_InputBox *box, const uchar *str);
void InputBox_Check_NewLine(UI_InputBox *box);
void InputBox_NewLine_No_Prompt(UI_InputBox *box);
void InputBox_NewLine(UI_InputBox *box);
void InputBox_Slide_Line(UI_InputBox *box);
void InputBox_Slide_Line(UI_InputBox *box);
void InputBox_Put_Prompt(UI_InputBox *box);
void InputBox_Reset_Input_Buffer(UI_InputBox *box);
void InputBox_Change_Cursor_State(UI_InputBox *box);

/*intrpt.c ���荞�ݐݒ�Ƃǂ��ɂ������Ȃ����荞�݃n���h���[*/
void Initialise_ProgrammableInterruptController(void);
void InterruptHandler27(int *esp);

/*keyboard.c*/
void Initialise_Keyboard(DATA_FIFO *sendto, DATA_FIFO *keycmd, uint offset, uint leds, int *keycmd_wait);
void InterruptHandler21(int *esp);
void Keyboard_Decode(UI_KeyInfo *info, uint data);
void Keyboard_KeyLock(uint led);
void Keyboard_Controller_Wait_SendReady(void);
uint Keyboard_Get_KeyShift(void);

/*memory.c �������֘A*/
uint Memory_Test(uint start, uint end);
void MemoryControl_Initialise(IO_MemoryControl *ctrl, void *start, uint size);
uint MemoryControl_FreeSize(IO_MemoryControl *ctrl);
void *MemoryControl_Allocate(IO_MemoryControl *ctrl, uint size);
int MemoryControl_Free(IO_MemoryControl *ctrl, void *addr0, uint size);
void *MemoryControl_Allocate_Page(IO_MemoryControl *ctrl);
void MemoryControl_Output_Info(IO_MemoryControl *ctrl);
void System_MemoryControl_Initialise(void);
uint System_MemoryControl_FullSize(void);
uint System_MemoryControl_FreeSize(void);
void *System_MemoryControl_Allocate(uint size);
int System_MemoryControl_Free(void *addr0, uint size);
void *System_MemoryControl_Allocate_Page(void);
void System_MemoryControl_Output_Info(void);

/*mouse.c �}�E�X�֘A*/
extern uchar mcursor_pattern0[24][24];
void Initialise_Mouse(DATA_FIFO *sendto, uint offset, UI_MouseInfo *decode);
void InterruptHandler2c(int *esp);
int Mouse_Decode(uint data);
void Mouse_Send_Command(uint data);
void Mouse_Make_MouseCursor(UI_MouseCursor *cursor, int rangex0, int rangey0, int rangex1, int rangey1, uint height);
void Mouse_Draw_MouseCursor(UI_MouseCursor *cursor, mcursor_state state);
void Mouse_Move_Relative(UI_MouseCursor *cursor, int movex, int movey);
void Mouse_Move_Absolute(UI_MouseCursor *cursor, int px, int py);

/*paging.c �y�[�W���O�֘A*/
void Initialise_Paging(void *vram, uint xsize, uint ysize, uint bpp);
void Paging_Set_Entry_Directory(uint *dir_entry, uint *table_base, uint attribute, uint available);
void Paging_Set_Entry_Table(uint *table_entry, uint *page_base, uint attribute, uint available);
uint *Paging_Get_Entry_Setting_Address(uint entry);

/*serial.c �V���A���ʐM�֘A*/
void Initialise_SerialPort(void);
void Send_SerialPort(uchar *s);

/*sheet.c �V�[�g�֘A*/
void Sheet_Initialise(UI_Sheet_Control *sheetctrl, IO_MemoryControl *memctrl, void *vram, uint xsize, uint ysize, uint bpp);
UI_Sheet *Sheet_Get(UI_Sheet_Control *ctrl, uint xsize, uint ysize, uint bpp, uint invcol);
uint Sheet_Show(UI_Sheet *sheet, int px, int py, uint height);
void Sheet_Slide(UI_Sheet *sheet, int px, int py);
uint Sheet_UpDown(UI_Sheet *sheet, uint height);
void Sheet_Remove(UI_Sheet *sheet);
void Sheet_Refresh_Map(UI_Sheet *sheet, int x0, int y0, int x1, int y1);
void Sheet_Write_Map_32(UI_Sheet *sheet, int x0, int y0, int x1, int y1);
void Sheet_Write_Map_16(UI_Sheet *sheet, int x0, int y0, int x1, int y1);
void Sheet_Write_Map_08(UI_Sheet *sheet, int x0, int y0, int x1, int y1);
void Sheet_Write_Map_NoInvisible(UI_Sheet *sheet, int x0, int y0, int x1, int y1);
void Sheet_Refresh_All(UI_Sheet *sheet0, UI_Sheet *sheet1, int x0, int y0, int x1, int y1);
void Sheet_Refresh_32from32(UI_Sheet *sheet, int px0, int py0, int px1, int py1);
void Sheet_Refresh_16from32(UI_Sheet *sheet, int px0, int py0, int px1, int py1);
void Sheet_Refresh_08from32(UI_Sheet *sheet, int px0, int py0, int px1, int py1);
void Sheet_Refresh_16from16(UI_Sheet *sheet, int px0, int py0, int px1, int py1);
void Sheet_Refresh_08from08(UI_Sheet *sheet, int px0, int py0, int px1, int py1);
void Sheet_Refresh_Invalid(UI_Sheet *sheet, int px0, int py0, int px1, int py1);
void Sheet_Draw_Put_String(UI_Sheet *sheet, uint x, uint y, uint c, const uchar *s);
void Sheet_Draw_Fill_Rectangle(UI_Sheet *sheet, uint c, uint x0, uint y0, uint x1, uint y1);
void Sheet_Draw_Point(UI_Sheet *sheet, uint c, uint x, uint y);
void System_Sheet_Initialise(void *vram, uint xsize, uint ysize, uint bpp);
UI_Sheet *System_Sheet_Get(uint xsize, uint ysize, uint bpp, uint invcol);
UI_Sheet *Sheet_Get_From_Position(UI_Sheet_Control *ctrl, int x, int y);
uint Sheet_Get_Top_Of_Height(UI_Sheet_Control *ctrl);
uint System_Sheet_Get_Top_Of_Height(void);

/*timer.c �^�C�}�[�֘A*/
void Initialise_ProgrammableIntervalTimer(void);
void InterruptHandler20(int *esp);
uint Timer_Get_Tick(void);
UI_Timer *Timer_Get(DATA_FIFO *fifo, uint data);
void Timer_Set(UI_Timer *timer, uint count, timer_mode mode);
void Timer_Run(UI_Timer *timer);

/*xception.c CPU��O�֘A*/
void CPU_Exception_Abort(int exception, int *esp);
void CPU_ExceptionHandler00(int *esp);
void CPU_ExceptionHandler01(int *esp);
void CPU_ExceptionHandler02(int *esp);
void CPU_ExceptionHandler03(int *esp);
void CPU_ExceptionHandler04(int *esp);
void CPU_ExceptionHandler05(int *esp);
void CPU_ExceptionHandler06(int *esp);
void CPU_ExceptionHandler07(int *esp);
void CPU_ExceptionHandler08(int *esp);
void CPU_ExceptionHandler09(int *esp);
void CPU_ExceptionHandler0a(int *esp);
void CPU_ExceptionHandler0b(int *esp);
void CPU_ExceptionHandler0c(int *esp);
void CPU_ExceptionHandler0d(int *esp);
void CPU_ExceptionHandler0e(int *esp);
void CPU_ExceptionHandler0f(int *esp);
void CPU_ExceptionHandler10(int *esp);
void CPU_ExceptionHandler11(int *esp);
void CPU_ExceptionHandler12(int *esp);
void CPU_ExceptionHandler13(int *esp);
void CPU_ExceptionHandler14(int *esp);
void CPU_ExceptionHandler15(int *esp);
void CPU_ExceptionHandler16(int *esp);
void CPU_ExceptionHandler17(int *esp);
void CPU_ExceptionHandler18(int *esp);
void CPU_ExceptionHandler19(int *esp);
void CPU_ExceptionHandler1a(int *esp);
void CPU_ExceptionHandler1b(int *esp);
void CPU_ExceptionHandler1c(int *esp);
void CPU_ExceptionHandler1d(int *esp);
void CPU_ExceptionHandler1e(int *esp);
void CPU_ExceptionHandler1f(int *esp);

/*nasfunc0.nas ���̊֐��ɑS���ˑ����Ȃ��A�Z���u���֐��Q*/
void IO_HLT(void);			//CPU���~������B���荞�݂�����ƍĊJ����B�������߁B
void IO_CLI(void);			//CPU�ւ̊O�����荞�݂��~������B�������߁BNMI�Ɨ�O�ɂ͌����Ȃ��B
void IO_STI(void);			//CPU�ւ̊O�����荞�݂��ĊJ������B�������߁B
void IO_STIHLT(void);			//CPU�ւ̊O�����荞�݂��ĊJ��������ACPU���~������Bsti��hlt�̊Ԃɔ������銄�荞�݂ւ̑΍�B�B
uint IO_In8(uint port);			//port�Ԃ�I/O�|�[�g����f�[�^���o�C�g�P�ʂŎ󂯎��B�߂�l�͎󂯎�����f�[�^�BI/O�������߁B
void IO_Out8(uint port, uint data);	//port�Ԃ�I/O�|�[�g��data���o�C�g�P�ʂŏo�͂���BI/O�������߁B
uint IO_In16(uint port);		//port�Ԃ�I/O�|�[�g����f�[�^�����[�h�P�ʂŎ󂯎��B�߂�l�͎󂯎�����f�[�^�BI/O�������߁B
void IO_Out16(uint port, uint data);	//port�Ԃ�I/O�|�[�g��data�����[�h�P�ʂŏo�͂���BI/O�������߁B
uint IO_In32(uint port);		//port�Ԃ�I/O�|�[�g����f�[�^���_�u�����[�h�P�ʂŎ󂯎��B�߂�l�͎󂯎�����f�[�^�BI/O�������߁B
void IO_Out32(uint port, uint data);	//port�Ԃ�I/O�|�[�g��data���_�u�����[�h�P�ʂŏo�͂���BI/O�������߁B
uint IO_Load_EFlags(void);		//EFLAGS���W�X�^��ǂݍ��ށB
void IO_Store_EFlags(uint eflags);	//EFLAGS���W�X�^��eflags�ɕύX����B
uint Load_CR0(void);			//�R���g���[�����W�X�^0��ǂݍ��ށB�������߁B
void Store_CR0(uint cr0);		//�R���g���[�����W�X�^0��cr0�ɕύX����B�������߁B
uint Load_CR2(void);			//�R���g���[�����W�X�^2��ǂݍ��ށB�������߁B
void Store_CR2(uint cr2);		//�R���g���[�����W�X�^2��cr2�ɕύX����B�������߁B
uint Load_CR3(void);			//�R���g���[�����W�X�^3��ǂݍ��ށB�������߁B
void Store_CR3(uint cr3);		//�R���g���[�����W�X�^3��cr3�ɕύX����B�������߁B
uint Load_CR4(void);			//�R���g���[�����W�X�^4��ǂݍ��ށB�������߁B
void Store_CR4(uint cr4);		//�R���g���[�����W�X�^4��cr4�ɕύX����B�������߁B
void Load_GDTR(uint limit, uint addr);	//�O���[�o���E�f�B�X�N���v�^�E�e�[�u���E���W�X�^(GDTR)�ɁAaddr����limit�͈̔͂�GDT�Ƃ��ă��[�h������B�������߁B
void Load_IDTR(uint limit, uint addr);	//�C���^���v�g�E�f�B�X�N���v�^�E�e�[�u���E���W�X�^(IDTR)�ɁAaddr����limit�͈̔͂�IDT�Ƃ��ă��[�h������B�������߁B
void Load_TR(uint tr);			//�Z�O�����g�E�Z���N�^tr���A�^�X�N���W�X�^�Ɍ��݂̃^�X�N�Ƃ��ă��[�h������B�������߁B
void FarJMP(uint eip, uint cs);		//���݂̃Z�O�����g�Ƃ͈قȂ�Z�O�����gcs�̃I�t�Z�b�geip�Ƀv���O�����̐�����ڂ��B
void FarCall(uint eip, uint cs);	//���݂̃Z�O�����g�Ƃ͈قȂ�Z�O�����gcs�̃I�t�Z�b�geip�Ƀv���O�����̐���𕪊򂳂���B
void CLTS(void);			//�R���g���[���E���W�X�^0���ɂ���ATask-Switched�t���O���N���A����BFPU�̐���ɗ��p�B�������߁B
void FNSave(uint *addr);		//FPU�������addr����n�܂�108�o�C�g�Ɋi�[���AFPU������������B�ڍׂȃ`�F�b�N�͍s��Ȃ��B
void FRStore(uint *addr);		//FPU�������addr����n�܂�108�o�C�g���畜������B
void PIT_Beep_On(void);			//�r�[�v�����J�n����B
void PIT_Beep_Off(void);		//�r�[�v�����~����B
void PIT_Beep_Set(uint fq);		//�r�[�v���̎��g�����AfqHz�ɕύX����B
					//fq�̒l
					//C:262 C#:277 D:294 D#:311 E:330 F:349 F#:370 G:392 G#:415 A:440 A#:466 B:494 C:523
void CPUID(void *addr, uint id);	//addr�Ԓn��uint[4]�ɁACPU�̎��ʏ��id�Ԃ�EAX�EEBX�EEDX�EECX�̏��ԂŊi�[����B
void CPUID2(void *addr, uint id);	//addr�Ԓn��uint[4]�ɁACPU�̎��ʏ��id�Ԃ�EAX�EEBX�EECX�EEDX�̏��ԂŊi�[����B
					//��L��̊֐��́AEFLAGS����ID�t���O(�r�b�g21)���ύX�\�ȏꍇ�̂ݎ��s�ł���B
void Read_TSC(uint *addr);		//addr�Ԓn��uint[2]�ɁA�}�V���ŗL���W�X�^(MSR)���ɂ���A�^�C���E�X�^���v�E�J�E���^�̏�ʁE���ʂ��ꂼ��32�r�b�g��ǂݍ��ށB
uint Memory_Test_Sub(uint start, uint end);
void INT_3(void);

/*nasfunc1.nas C����̊֐��Ɉˑ�����֐��Q�B�����Ɋ��荞�݊֌W�B*/
void asm_CPU_ExceptionHandler00(void);
void asm_CPU_ExceptionHandler01(void);
void asm_CPU_ExceptionHandler02(void);
void asm_CPU_ExceptionHandler03(void);
void asm_CPU_ExceptionHandler04(void);
void asm_CPU_ExceptionHandler05(void);
void asm_CPU_ExceptionHandler06(void);
void asm_CPU_ExceptionHandler07(void);
void asm_CPU_ExceptionHandler08(void);
void asm_CPU_ExceptionHandler09(void);
void asm_CPU_ExceptionHandler0a(void);
void asm_CPU_ExceptionHandler0b(void);
void asm_CPU_ExceptionHandler0c(void);
void asm_CPU_ExceptionHandler0d(void);
void asm_CPU_ExceptionHandler0e(void);
void asm_CPU_ExceptionHandler0f(void);
void asm_CPU_ExceptionHandler10(void);
void asm_CPU_ExceptionHandler11(void);
void asm_CPU_ExceptionHandler12(void);
void asm_CPU_ExceptionHandler13(void);
void asm_CPU_ExceptionHandler14(void);
void asm_CPU_ExceptionHandler15(void);
void asm_CPU_ExceptionHandler16(void);
void asm_CPU_ExceptionHandler17(void);
void asm_CPU_ExceptionHandler18(void);
void asm_CPU_ExceptionHandler19(void);
void asm_CPU_ExceptionHandler1a(void);
void asm_CPU_ExceptionHandler1b(void);
void asm_CPU_ExceptionHandler1c(void);
void asm_CPU_ExceptionHandler1d(void);
void asm_CPU_ExceptionHandler1e(void);
void asm_CPU_ExceptionHandler1f(void);
void asm_InterruptHandler20(void);
void asm_InterruptHandler21(void);
void asm_InterruptHandler27(void);
void asm_InterruptHandler2c(void);
