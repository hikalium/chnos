
#include "core.h"

IO_FloppyDisk *FloppyDisk_Initialise(void *img)
{
	IO_FloppyDisk *fd;

	fd = MemoryBlock_Allocate_System(sizeof(IO_FloppyDisk));
	MemoryBlock_Write_Description(fd, "IO_FloppyDisk");

	fd->img = img;
	fd->files = (IO_FloppyDisk_RootDirectoryEntry *)(fd->img + 0x002600);
	fd->userdataarea = (sector *)(fd->img + 0x002600 + 32 * 224);

	return fd;
}
