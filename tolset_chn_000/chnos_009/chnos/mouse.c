
#include "core.h"

uint offset_data_m;
DATA_FIFO *sendto_m;
UI_MouseInfo *decode_m;

uchar mcursor_pattern0[24][24] = {
	".**.....................",
	"*O**....................",
	"*OO**...................",
	"*OOO**..................",
	"*OOOO**.................",
	"*OOOOO**................",
	"*OOOOOO**...............",
	"*OOOOOOO**..............",
	"*OOOOOOOO**.............",
	"*OOOOOOOOO**............",
	"*OOOOOOOOOO**...........",
	"*OOOOOOOOOOO**..........",
	"*OOOOOOOOOOOO**.........",
	"*OOOOOOOOOOOOO**........",
	"*OOOOOOOOOOOOOO**.......",
	"*OOOOOOOOOOOOOOO**......",
	"*OOOOOOOOOOOOOOOO**.....",
	"*OOOOOO*************....",
	"*OOOOO**................",
	"*OOOO**.................",
	"*OOO**..................",
	"*OO**...................",
	"*O**....................",
	"***.....................",
};

void Initialise_Mouse(DATA_FIFO *sendto, uint offset, UI_MouseInfo *decode)
{
	sendto_m = sendto;
	offset_data_m = offset;
	decode_m = decode;

	decode->phase = 0;
	Mouse_Send_Command(MOUSECMD_RESET);
	GateDescriptor_Set(0x2c, (uint)asm_InterruptHandler2c, 0x02, AR_INTGATE32);
	IO_Out8(PIC1_IMR, IO_In8(PIC1_IMR) & 0xef);

	return;
}

void InterruptHandler2c(int *esp)
{
	int data;

	data = IO_In8(KEYB_DATA);
	IO_Out8(PIC1_OCW2, 0x64);
	IO_Out8(PIC0_OCW2, 0x62);
	FIFO32_Put(sendto_m, data + offset_data_m);
	return;
}

int Mouse_Decode(uint data)
{
	switch (decode_m->phase){
		case 0:
			if(data == 0xfa){
				decode_m->phase++;
			}
			break;
		case 1:
			if(data == 0xaa){
				Mouse_Send_Command(MOUSECMD_SET_SAMPLE_RATE);
				decode_m->phase++;
			}
			break;
		case 2:
			if(data == 0xfa){
				Mouse_Send_Command(200);
				decode_m->phase++;
			}
			break;
		case 3:
			if(data == 0xfa){
				Mouse_Send_Command(MOUSECMD_SET_SAMPLE_RATE);
				decode_m->phase++;
			}
			break;
		case 4:
			if(data == 0xfa){
				Mouse_Send_Command(100);
				decode_m->phase++;
			}
			break;
		case 5:
			if(data == 0xfa){
				Mouse_Send_Command(MOUSECMD_SET_SAMPLE_RATE);
				decode_m->phase++;
			}
			break;
		case 6:
			if(data == 0xfa){
				Mouse_Send_Command(80);
				decode_m->phase++;
			}
			break;
		case 7:
			if(data == 0xfa){
				Mouse_Send_Command(MOUSECMD_GET_DEVICE_ID);
				decode_m->phase++;
			}
			break;
		case 8:
			if(data == 0xfa){
				break;
			}
			if(data == 0x00){
				decode_m->type = threebtn;
				Mouse_Send_Command(MOUSECMD_ENABLE_DATA_REPORTING);
				decode_m->phase = 10;
			} else if(data == 0x03){
				decode_m->type = threebtn_scroll;
				Mouse_Send_Command(MOUSECMD_ENABLE_DATA_REPORTING);
				decode_m->phase = 20;
			} else{
				Mouse_Send_Command(MOUSECMD_RESET);
				decode_m->phase = 0;
			}
			break;

		case 10:
			if(data == 0xfa){
				decode_m->phase++;
			}
			break;
		case 11:
			if((data & 0xc8) == 0x08) {
				decode_m->buf[0] = data;
				decode_m->phase++;
			}
			break;
		case 12:
			decode_m->buf[1] = data;
			decode_m->phase++;
			break;
		case 13:
			decode_m->buf[2] = data;
			decode_m->phase -= 2;
			decode_m->btn = decode_m->buf[0];
			decode_m->move.x = decode_m->buf[1];
			decode_m->move.y = decode_m->buf[2];
			if(decode_m->buf[0] & 0x10){
				decode_m->move.x |= 0xffffff00;
			}
			if(decode_m->buf[0] & 0x20){
				decode_m->move.y |= 0xffffff00;
			}
			decode_m->move.y = - decode_m->move.y;
			return 1;

		case 20:
			if(data == 0xfa){
				decode_m->phase++;
			}
			break;
		case 21:
			if((data & 0xc8) == 0x08) {
				decode_m->buf[0] = data;
				decode_m->phase++;
			}
			break;
		case 22:
			decode_m->buf[1] = data;
			decode_m->phase++;
			break;
		case 23:
			decode_m->buf[2] = data;
			decode_m->btn = decode_m->buf[0];
			decode_m->move.x = decode_m->buf[1];
			decode_m->move.y = decode_m->buf[2];
			if(decode_m->buf[0] & 0x10){
				decode_m->move.x |= 0xffffff00;
			}
			if(decode_m->buf[0] & 0x20){
				decode_m->move.y |= 0xffffff00;
			}
			decode_m->move.y = - decode_m->move.y;
			decode_m->phase++;
			break;
		case 24:
			decode_m->phase -= 3;
			decode_m->buf[3] = data;
			decode_m->scroll = decode_m->buf[3] & 0x0f;
			if(decode_m->scroll & 0x08) {
				decode_m->scroll |= 0xfffffff0;
			}
			return 1;
	}
	return 0;
}

void Mouse_Send_Command(uint data)
{
	Keyboard_Controller_Wait_SendReady();
	IO_Out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	Keyboard_Controller_Wait_SendReady();
	IO_Out8(KEYB_DATA, data);
	return;
}

void Mouse_Make_MouseCursor(UI_MouseCursor *cursor, int rangex0, int rangey0, int rangex1, int rangey1, uint height)
{
	cursor->move_range0.x = rangex0;
	cursor->move_range0.y = rangey0;
	cursor->move_range1.x = rangex1;
	cursor->move_range1.y = rangey1;
	cursor->position.x = rangex0;
	cursor->position.y = rangey0;
	cursor->sheet = System_Sheet_Get(24, 24, 0, 0x000000FF);
	Mouse_Draw_MouseCursor(cursor, normal);
	Sheet_Show(cursor->sheet, cursor->position.x, cursor->position.y, height);
	return;
}

void Mouse_Draw_MouseCursor(UI_MouseCursor *cursor, mcursor_state state)
{
	uint x, y;
	uchar *pattern;

	pattern = &mcursor_pattern0[0][0];

	if(state == normal){
		cursor->state = normal;
	} else if(state == wait){
		pattern = 0;
		cursor->state = wait;
	}

	for (y = 0; y < 24; y++) {
		for (x = 0; x < 24; x++) {
			if (pattern[y * 24 + x] == '*') {
				Sheet_Draw_Point(cursor->sheet, 0x000000, x, y);
			}
			if (pattern[y * 24 + x] == 'O') {
				Sheet_Draw_Point(cursor->sheet, 0xFFFFFF, x, y);
			}
			if (pattern[y * 24 + x] == '.') {
				Sheet_Draw_Point(cursor->sheet, 0x0000FF, x, y);
			}
		}
	}

	return;
}

void Mouse_Move_Relative(UI_MouseCursor *cursor, int movex, int movey)
{
	cursor->position.x += movex;
	cursor->position.y += movey;

	if(cursor->position.x < cursor->move_range0.x){
		cursor->position.x = cursor->move_range0.x;
	} else if(cursor->position.x > cursor->move_range1.x){
		cursor->position.x = cursor->move_range1.x;
	}
	if(cursor->position.y < cursor->move_range0.y){
		cursor->position.y = cursor->move_range0.y;
	} else if(cursor->position.y > cursor->move_range1.y){
		cursor->position.y = cursor->move_range1.y;
	}

	Sheet_Slide(cursor->sheet, cursor->position.x, cursor->position.y);

	return;
}

void Mouse_Move_Absolute(UI_MouseCursor *cursor, int px, int py)
{
	cursor->position.x = px;
	cursor->position.y = py;

	if(cursor->position.x < cursor->move_range0.x){
		cursor->position.x = cursor->move_range0.x;
	} else if(cursor->position.x > cursor->move_range1.x){
		cursor->position.x = cursor->move_range1.x;
	}
	if(cursor->position.y < cursor->move_range0.y){
		cursor->position.y = cursor->move_range0.y;
	} else if(cursor->position.y > cursor->move_range1.y){
		cursor->position.y = cursor->move_range1.y;
	}

	Sheet_Slide(cursor->sheet, cursor->position.x, cursor->position.y);

	return;
}
