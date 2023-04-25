#ifndef SCSI_H
#define SCSI_H

#include "CFileSystem.h"

struct _stCDB
{
	BYTE bCDB[16];
};

extern DWORD ScsiCmdSend(HANDLE dev, _stCDB cdb, BYTE direction, BYTE cdbLen, void* data, DWORD dataXferLen, DWORD timeoutSecond = 60);

DWORD ScsiRead(HANDLE dev, BYTE* readBuffer, UINT offsetSector, UINT readSize);
DWORD ScsiWrite(HANDLE dev, BYTE* writeBuffer, UINT offsetSector, UINT writeSize);
BOOL PrintBuffer(BYTE* buffer, UINT bufferLen);
BOOL Format(HANDLE dev, CFileSystemConfig config);
BOOL InitPartitionTable(HANDLE dev);
BOOL InitFatFileSystem(HANDLE dev);



#endif //SCSI_H