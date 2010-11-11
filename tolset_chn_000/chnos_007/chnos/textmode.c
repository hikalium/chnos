
#include "core.h"

void textmode_putfont(uchar c, col_text col, col_text backcol, int columns, int line, bool blink)
{
	uchar color;

	if(blink) (uint)col += 8;
	color = (uint)backcol + ((uint)col << 4);
	system.info.boot.vram[((line * system.info.boot.scrnx) + columns) * 2] = c;
	system.info.boot.vram[((line * system.info.boot.scrnx) + columns) * 2 + 1] = color;
	return;
}

void textmode_putstr(uchar *s, col_text col, col_text backcol, int columns, int line, bool blink)
{
	int mx = 0, my = 0;
	int i;

	for (i = 0; s[i] != 0x00; i++) {
		if(s[i] == '\n'){
			mx = 0;
			my++;
		} else if(s[i] == '\r'){

		} else{
			textmode_putfont(s[i], col, backcol, columns + mx, line + my, blink);
			mx++;
		}
	}
	return;
}

