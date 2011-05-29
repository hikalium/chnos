
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
typedef unsigned char bool;
#define false	0;
#define true	1;
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

struct DATA_CPU_IDENTITY {
	bool enable;
	uint max;
	uchar vendor[13];
	bool ext_enable;
	uint ext_max;
	uchar brand_string[48];
	bool FPU;
	bool VME;
	bool DE;
	bool PSE;
	bool TSC;
	bool MSR;
	bool PAE;
	bool MCE;
	bool CX8;
	bool APIC;
	bool SEP;
	bool MTRR;
	bool PGE;
	bool MCA;
	bool CMOV;
	bool PAT;
	bool PSE36;
	bool PSN;
	bool CLFSH;
	bool DS;
	bool ACPI;
	bool MMX;
	bool FXSR;
	bool SSE;
	bool SSE2;
	bool SS;
	bool HTT;
	bool TM;
	bool PBE;

	bool SSE3;
	bool MONITOR;
	bool DSCPL;
	bool EST;
	bool TM2;
	bool CID;
	bool CX16;
	bool xTPR;
	
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
typedef struct DATA_CPU_IDENTITY	DATA_CPUID;

/*virtual classes*/

/*externs*/
extern uchar hankaku[4096];

/*functions*/
/*bootpack.c 基幹部分*/

/*cpuid.c*/
void CPU_Identify(DATA_CPUID *id);

/*dsctbl.c ディスクリプター・テーブル関連*/
void Initialise_GlobalDescriptorTable(void);
void Initialise_InterruptDescriptorTable(void);
void Set_SegmentDescriptor(IO_SegmentDescriptor *sd, uint limit, int base, int ar);
void Set_GateDescriptor(IO_GateDescriptor *gd, int offset, int selector, int ar);

/*fifo.c FIFOバッファ関連*/
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

/*intrpt.c 割り込み設定とどこにも属さない割り込みハンドラー*/
void Initialise_ProgrammableInterruptController(void);
void InterruptHandler27(int *esp);

/*io.c*/
uint IO_Read_CMOS(uchar addr);

/*keyboard.c*/
void Initialise_Keyboard(DATA_FIFO *sendto, DATA_FIFO *keycmd, uint offset, uint leds, int *keycmd_wait);
void InterruptHandler21(int *esp);
void Keyboard_Decode(UI_KeyInfo *info, uint data);
void Keyboard_KeyLock(uint led);
void Keyboard_Controller_Wait_SendReady(void);
uint Keyboard_Get_KeyShift(void);

/*memory.c メモリ関連*/
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

/*mouse.c マウス関連*/
extern uchar mcursor_pattern0[24][24];
void Initialise_Mouse(DATA_FIFO *sendto, uint offset, UI_MouseInfo *decode);
void InterruptHandler2c(int *esp);
int Mouse_Decode(uint data);
void Mouse_Send_Command(uint data);
void Mouse_Make_MouseCursor(UI_MouseCursor *cursor, int rangex0, int rangey0, int rangex1, int rangey1, uint height);
void Mouse_Draw_MouseCursor(UI_MouseCursor *cursor, mcursor_state state);
void Mouse_Move_Relative(UI_MouseCursor *cursor, int movex, int movey);
void Mouse_Move_Absolute(UI_MouseCursor *cursor, int px, int py);

/*paging.c ページング関連*/
void Initialise_Paging(void *vram, uint xsize, uint ysize, uint bpp);
void Paging_Set_Entry_Directory(uint *dir_entry, uint *table_base, uint attribute, uint available);
void Paging_Set_Entry_Table(uint *table_entry, uint *page_base, uint attribute, uint available);
uint *Paging_Get_Entry_Setting_Address(uint entry);

/*serial.c シリアル通信関連*/
void Initialise_SerialPort(void);
void Send_SerialPort(uchar *s);
int debug(const uchar *format, ...);

/*sheet.c シート関連*/
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
void Sheet_Draw_Put_String_With_BackColor(UI_Sheet *sheet, uint x, uint y, uint c, uint bc, const uchar *s);
void Sheet_Draw_Fill_Rectangle(UI_Sheet *sheet, uint c, uint x0, uint y0, uint x1, uint y1);
void Sheet_Draw_Point(UI_Sheet *sheet, uint c, uint x, uint y);
void System_Sheet_Initialise(void *vram, uint xsize, uint ysize, uint bpp);
UI_Sheet *System_Sheet_Get(uint xsize, uint ysize, uint bpp, uint invcol);
UI_Sheet *Sheet_Get_From_Position(UI_Sheet_Control *ctrl, int x, int y);
uint Sheet_Get_Top_Of_Height(UI_Sheet_Control *ctrl);
uint System_Sheet_Get_Top_Of_Height(void);

/*timer.c タイマー関連*/
void Initialise_ProgrammableIntervalTimer(void);
void InterruptHandler20(int *esp);
uint Timer_Get_Tick(void);
UI_Timer *Timer_Get(DATA_FIFO *fifo, uint data);
void Timer_Set(UI_Timer *timer, uint count, timer_mode mode);
void Timer_Run(UI_Timer *timer);

/*xception.c CPU例外関連*/
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

/*nasfunc0.nas 他の関数に全く依存しないアセンブラ関数群*/
void IO_HLT(void);			//CPUを停止させる。割り込みがあると再開する。特権命令。
void IO_CLI(void);			//CPUへの外部割り込みを停止させる。特権命令。NMIと例外には効かない。
void IO_STI(void);			//CPUへの外部割り込みを再開させる。特権命令。
void IO_STIHLT(void);			//CPUへの外部割り込みを再開させた後、CPUを停止させる。stiとhltの間に発生する割り込みへの対策。。
uint IO_In8(uint port);			//port番のI/Oポートからデータをバイト単位で受け取る。戻り値は受け取ったデータ。I/O特権命令。
void IO_Out8(uint port, uint data);	//port番のI/Oポートにdataをバイト単位で出力する。I/O特権命令。
uint IO_In16(uint port);		//port番のI/Oポートからデータをワード単位で受け取る。戻り値は受け取ったデータ。I/O特権命令。
void IO_Out16(uint port, uint data);	//port番のI/Oポートにdataをワード単位で出力する。I/O特権命令。
uint IO_In32(uint port);		//port番のI/Oポートからデータをダブルワード単位で受け取る。戻り値は受け取ったデータ。I/O特権命令。
void IO_Out32(uint port, uint data);	//port番のI/Oポートにdataをダブルワード単位で出力する。I/O特権命令。
uint IO_Load_EFlags(void);		//EFLAGSレジスタを読み込む。
void IO_Store_EFlags(uint eflags);	//EFLAGSレジスタをeflagsに変更する。
uint Load_CR0(void);			//コントロールレジスタ0を読み込む。特権命令。
void Store_CR0(uint cr0);		//コントロールレジスタ0をcr0に変更する。特権命令。
uint Load_CR2(void);			//コントロールレジスタ2を読み込む。特権命令。
void Store_CR2(uint cr2);		//コントロールレジスタ2をcr2に変更する。特権命令。
uint Load_CR3(void);			//コントロールレジスタ3を読み込む。特権命令。
void Store_CR3(uint cr3);		//コントロールレジスタ3をcr3に変更する。特権命令。
uint Load_CR4(void);			//コントロールレジスタ4を読み込む。特権命令。
void Store_CR4(uint cr4);		//コントロールレジスタ4をcr4に変更する。特権命令。
void Load_GDTR(uint limit, uint addr);	//グローバル・ディスクリプタ・テーブル・レジスタ(GDTR)に、addrからlimitの範囲をGDTとしてロードさせる。特権命令。
void Load_IDTR(uint limit, uint addr);	//インタラプト・ディスクリプタ・テーブル・レジスタ(IDTR)に、addrからlimitの範囲をIDTとしてロードさせる。特権命令。
void Load_TR(uint tr);			//セグメント・セレクタtrを、タスクレジスタに現在のタスクとしてロードさせる。特権命令。
void FarJMP(uint eip, uint cs);		//現在のセグメントとは異なるセグメントcsのオフセットeipにプログラムの制御を移す。
void FarCall(uint eip, uint cs);	//現在のセグメントとは異なるセグメントcsのオフセットeipにプログラムの制御を分岐させる。
void CLTS(void);			//コントロール・レジスタ0内にある、Task-Switchedフラグをクリアする。FPUの制御に利用。特権命令。
void FNSave(uint *addr);		//FPU動作環境をaddrから始まる108バイトに格納し、FPUを初期化する。詳細なチェックは行わない。
void FRStore(uint *addr);		//FPU動作環境をaddrから始まる108バイトから復元する。
void PIT_Beep_On(void);			//ビープ音を開始する。
void PIT_Beep_Off(void);		//ビープ音を停止する。
void PIT_Beep_Set(uint fq);		//ビープ音の周波数を、fqHzに変更する。
					//fqの値
					//C:262 C#:277 D:294 D#:311 E:330 F:349 F#:370 G:392 G#:415 A:440 A#:466 B:494 C:523
void CPUID(void *addr, uint id);	//addr番地のuint[4]に、CPUの識別情報id番をEAX・EBX・EDX・ECXの順番で格納する。
void CPUID2(void *addr, uint id);	//addr番地のuint[4]に、CPUの識別情報id番をEAX・EBX・ECX・EDXの順番で格納する。
					//上記二つの関数は、EFLAGS内のIDフラグ(ビット21)が変更可能な場合のみ実行できる。
void Read_TSC(uint *addr);		//addr番地のuint[2]に、マシン固有レジスタ(MSR)内にある、タイム・スタンプ・カウンタの上位・下位それぞれ32ビットを読み込む。
					//この関数は、cpuidのTSCビットが有効でなければ使用できない。
uint Memory_Test_Sub(uint start, uint end);
void INT_3(void);

/*nasfunc1.nas C言語の関数に依存する関数群。おもに割り込み関係。*/
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
