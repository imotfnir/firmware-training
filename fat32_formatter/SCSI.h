#ifndef SCSI_H
#define SCSI_H

#define SECTOR_SIZE 0x200

struct _stCDB
{
	BYTE bCDB[16];
};

extern DWORD ScsiCmdSend(HANDLE hDev, _stCDB stCDB, BYTE bDirection, BYTE bCdbLen, void *pData, DWORD dwDataXferLen, DWORD dwTimeout = 60);

BOOL scsi_read(HANDLE dev, BYTE* readBuffer, UINT offsetSector, UINT readSize);
BOOL scsi_write(HANDLE dev, BYTE* readBuffer, UINT offsetSector, UINT writeSize);

#endif //SCSI_H