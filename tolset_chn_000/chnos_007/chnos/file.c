
#include "core.h"

void decode_fat(ushort *fat, bool backup)
{
	int i, j = 0;
	uchar *img;

	if(backup)	img = (uchar *)ADR_DISKIMG + 0x00001400;
	else		img = (uchar *)ADR_DISKIMG + 0x00000200;

	for(i = 0; i < 2880; i += 2){
		fat[i + 0] = (img[j + 0]	| img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4	| img[j + 2] << 4) & 0xfff;
		j += 3;
	}
	return;
}

void load_file(uint finfo_no, uchar *buf)
{
	int i;
	uchar *img;
	uint size;
	ushort clustno;

	if(finfo_no > 224) return;

	img = (uchar *)ADR_DISKIMG + 0x00003e00;
	size = system.file.list[finfo_no].size;
	clustno = system.file.list[finfo_no].clustno;


	for(;;){
		if(size <= 512){
			for(i = 0; i < size; i++){
				buf[i] = img[clustno * 512 + i];
			}
			break;
		}
		for(i = 0; i < 512; i++){
			buf[i] = img[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = system.file.fat[clustno];
		
	}
}

uint search_file(char *name)
{
	int i, j;
	uchar s[12];

	for(j = 0; j < 11; j++){
		s[j] = ' ';
	}
	j = 0;
	for(i = 0; j < 11 && name[i] != 0x00; i++){
		if(name[i] == '.' && j <= 8){
			j = 8;
		} else{
			s[j] = name[i];
			if('a' <= s[j] && s[j] <= 'z'){
				s[j] -= 0x20;
			}
			j++;
		}
	}
	for(i = 0; i < 224; ){
		if(system.file.list[i].name[0] == 0x00) break;
		if((system.file.list[i].type & 0x18) == 0){
			for(j = 0; j < 11; j++){
				if(system.file.list[i].name[j] != s[j]) goto next_file;
			}
			break; 
		}
next_file:
		i++;
	}
	if(i < 224 && system.file.list[i].name[0] != 0x00){
		return i;
	} else {
		return 0xFFFFFFFF;
	}
}
