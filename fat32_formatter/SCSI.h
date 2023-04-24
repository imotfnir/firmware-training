#ifndef SCSI_H
#define SCSI_H

#define SECTOR_SIZE 0x200U

struct _stCDB
{
	BYTE bCDB[16];
};

extern DWORD ScsiCmdSend(HANDLE dev, _stCDB cdb, BYTE direction, BYTE cdbLen, void* data, DWORD dataXferLen, DWORD timeoutSecond = 60);

DWORD ScsiRead(HANDLE dev, BYTE* readBuffer, UINT offsetSector, UINT readSize);


#endif //SCSI_H