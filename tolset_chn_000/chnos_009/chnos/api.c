
#include "core.h"

uint *API_Execute(uint edi, uint esi, uint ebp, uint esp, uint ebx, uint edx, uint ecx, uint eax)
{
	UI_Task *nowtask;
	uchar s[128];
	uint i;

	nowtask = MultiTask_Get_NowTask();

	if(edx == 0x00000001){
		InputBox_Put_Character(nowtask->cons->input, eax & 0xff);
	} else if(edx == 0x00000002){
		InputBox_Put_String(nowtask->cons->input, (uchar *)(ebx + System_SegmentDescriptor_Get_Base(nowtask->cons->app_ds)));
	} else if(edx == 0x00000003){
		for(i = 0; i < ecx; i++){
			InputBox_Put_Character(nowtask->cons->input, ((uchar *)ebx + System_SegmentDescriptor_Get_Base(nowtask->cons->app_ds))[i]);
		}
	} else if(edx == 0x00000004){
		return &nowtask->tss.esp0;
	} else{
		sprintf(s, "API:0x%08X:Unknown API Function.\n", edx);
		InputBox_Put_String(nowtask->cons->input, s);
	}

	return 0;
}
