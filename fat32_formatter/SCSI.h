#ifndef SCSI_H
#define SCSI_H

#include "CFileSystem.h"

#include <ntddscsi.h>
#include <WinIoCtl.h>
#include <Windows.h>

struct _stCDB
{
	BYTE bCDB[16];
};

typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE
{
	SCSI_PASS_THROUGH_DIRECT sptd;
	DWORD filler; // align abRequestSense to DWORD boundary
	BYTE abRequestSense[32];
} SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE, *PSCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE;

extern DWORD ScsiCmdSend(HANDLE dev, _stCDB cdb, BYTE direction, BYTE cdbLen, void *data, DWORD dataXferLen, DWORD timeoutSecond = 60);

DWORD ScsiRead(HANDLE dev, BYTE *readBuffer, UINT offsetSector, UINT readSizeSector);
DWORD ScsiWrite(HANDLE dev, BYTE *writeBuffer, UINT offsetSector, UINT writeSizeSector);
BOOL PrintBuffer(BYTE *buffer, UINT bufferLen);
BOOL Format(HANDLE dev, CFileSystemConfig config);
BOOL InitPartitionTable(HANDLE dev);

#endif // SCSI_H