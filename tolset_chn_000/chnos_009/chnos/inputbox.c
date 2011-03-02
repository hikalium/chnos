
#include "core.h"

void InputBox_Initialise(UI_InputBox *box, void *vram, uint x, uint y, uint xsize, uint ysize, uint txtbufsize, uint forecol, uint backcol)
{
	box->vram = vram;
	box->position.x = x;
	box->position.y = y;
	box->size.x = xsize;
	box->size.y = ysize;
	box->input_buf_size = txtbufsize;
	box->input_buf = (uint *)System_MemoryControl_Allocate(box->input_buf_size);
	box->cursor.x = 0;
	box->cursor.y = 0;
	box->prompt.x = 0;
	box->prompt.y = 0;
	FIFO32_Initialise(&box->input, box->input_buf_size, box->input_buf);
	return;
}
