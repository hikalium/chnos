
#include "core.h"

IO_FloppyDisk *FloppyDisk_Initialise(void *img)
{
	IO_FloppyDisk *fd;

	fd = MemoryBlock_Allocate_System(sizeof(IO_FloppyDisk));
	MemoryBlock_Write_Description(fd, "IO_FloppyDisk");

	fd->img = img;
	fd->files = (IO_FloppyDisk_RootDirectoryEntry *)(fd->img + 0x002600);
	fd->userdataarea = (sector *)(fd->img + 0x002600 + 32 * 224);

	fd->fat = MemoryBlock_Allocate_System(2 * 2880);
	MemoryBlock_Write_Description(fd->fat, "IO_FD_FAT");
	FloppyDisk_Decode_FAT16(fd->img, fd->fat, false);

	return fd;
}

int FloppyDisk_Search_File(IO_FloppyDisk *fd, const uchar *name)
{
	uint i, j;
	uchar filename[11];

	for(i = 0; i < 11; i++){
		filename[i] = ' ';
	}

	j = 0;
	for(i = 0; i < 11; ){
		if(name[j] == 0x00){
			i++;
		} else{
			if(name[j] == '.'){
				i = 8;
			} else{
				filename[i] = name[j];
				if('a' <= filename[i] && filename[i] <= 'z'){
					filename[i] -= 0x20;
				}
				i++;
			}
			j++;
		}
	}

	for(j = 0; j < 224; j++){
		if(fd->files[j].name[0] == 0x00){
			break;
		}
		if(fd->files[j].name[0] != 0xe5){
			for(i = 0; i < 11; i++){
				if(filename[i] != fd->files[j].name[i]){
					break;
				}
			}
			if(i == 11){
				return j;
			}
		}
	}

	return -1;
}

int FloppyDisk_Load_File(IO_FloppyDisk *fd, IO_File *fileinfo, int fileno)
{
	uint i, j, now_cluster, now_size;

	fileinfo->name[12] = 0x00;

	if(fileno < 0 || 224 <= fileno){
		return -1;
	}

	fileinfo->data = MemoryBlock_Allocate_System(fd->files[fileno].size);
	fileinfo->size = fd->files[fileno].size;

	sprintf(fileinfo->name, "FILENAME.EXT");
	for(i = 0; i < 8; i++){
		fileinfo->name[i] = fd->files[fileno].name[i];
	}
	for(i = 9; i < 12; i++){
		fileinfo->name[i] = fd->files[fileno].name[i - 1];
	}
	MemoryBlock_Write_Description(fileinfo->data, fileinfo->name);

	now_cluster = fd->files[fileno].cluster;
	now_size = fileinfo->size;
	for(j = 0; ; j++){
		if(now_size <= 512){
			for(i = 0; i < now_size; i++){
				fileinfo->data[(j << 9) + i] = fd->userdataarea[now_cluster - 2][i];
			}
			break;
		}
		for(i = 0; i < 512; i++){
			fileinfo->data[(j << 9) + i] = fd->userdataarea[now_cluster - 2][i];
		}
		now_size -= 512;
		now_cluster = ((ushort *)fd->fat)[now_cluster];
	}

	return i;
}

int File_Free(IO_File *fileinfo)
{
	uint i;

	for(i = 0; i < 13; i++){
		fileinfo->name[i] = 0x00;
	}
	fileinfo->size = 0;
	return MemoryBlock_Free(fileinfo->data);
}

void FloppyDisk_Decode_FAT16(uchar *img, ushort *fat, bool backup)
{
	int i, j = 0;

	if(backup){
		img += 0x00001400;
	} else{
		img += 0x00000200;
	}

	for(i = 0; i < 2880; i += 2){
		fat[i + 0] = (img[j + 0]	| img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4	| img[j + 2] << 4) & 0xfff;
		j += 3;
	}
	return;
}
