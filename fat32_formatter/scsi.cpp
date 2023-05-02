//  need to include WinDDK in project
#pragma once

#include "scsi.h"

#include "pch.h"

#include "devioctl.h"

//  bDirection have 3 type
//  SCSI_IOCTL_DATA_OUT: for write command
//  SCSI_IOCTL_DATA_IN: for read command
//  SCSI_IOCTL_DATA_UNSPECIFIED: for no data command
//  if no data command, set pData to NULL and dwDataXferLen to 0

//  dwTimeout: unit is second

//  return      0: no error
//              other: windows errorcode

DWORD ScsiCmdSend(HANDLE dev, _stCDB cdb, BYTE direction, BYTE cdbLen, void *data, DWORD dataXferLen, DWORD timeoutSecond);

DWORD ScsiCmdSend(HANDLE dev, _stCDB cdb, BYTE direction, BYTE cdbLen, void *data, DWORD dataXferLen, DWORD timeoutSecond)
{
	BOOL apiStatus = FALSE;
	BYTE abRequestSense[32] = {0};
	DWORD byteReturn;

	SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE sptd = {0};
	sptd.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	sptd.sptd.PathId = 0;
	sptd.sptd.TargetId = 1;
	sptd.sptd.Lun = 0;
	sptd.sptd.CdbLength = cdbLen;
	sptd.sptd.DataIn = (BYTE)direction;
	sptd.sptd.SenseInfoLength = sizeof(sptd.abRequestSense);
	sptd.sptd.DataTransferLength = dataXferLen;
	sptd.sptd.TimeOutValue = timeoutSecond;
	sptd.sptd.DataBuffer = (data == NULL) ? abRequestSense : data;
	sptd.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE, abRequestSense);

	memcpy(sptd.sptd.Cdb, &cdb, sizeof(sptd.sptd.Cdb));

	apiStatus = DeviceIoControl(dev,
								IOCTL_SCSI_PASS_THROUGH_DIRECT,
								&sptd,
								sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE),
								&sptd,
								sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE),
								&byteReturn,
								FALSE);

	if ((sptd.sptd.ScsiStatus == 0) && (apiStatus != 0)){
		return 0;
	}
	TRACE(_T("ScsiCmdSend Error, status: %d, scsi_status: %s\n"), apiStatus, sptd.sptd.ScsiStatus);
	return GetLastError();
}

DWORD ScsiRead(HANDLE dev, BYTE *readBuffer, UINT offsetSector, UINT readSizeSector)
{
	_stCDB cdb = {0};
	cdb.bCDB[0] = 0x28;
	cdb.bCDB[1] = 0x00;
	cdb.bCDB[2] = (offsetSector >> 24) & 0xff;
	cdb.bCDB[3] = (offsetSector >> 16) & 0xff;
	cdb.bCDB[4] = (offsetSector >> 8) & 0xff;
	cdb.bCDB[5] = offsetSector & 0xff;
	cdb.bCDB[6] = 0x00;
	cdb.bCDB[7] = (readSizeSector >> 8) & 0xff;
	cdb.bCDB[8] = readSizeSector & 0xff;
	cdb.bCDB[9] = 0x00;

	return ScsiCmdSend(dev, cdb, SCSI_IOCTL_DATA_IN, 10, (void *)readBuffer, SECTOR_SIZE, 2);
}

DWORD ScsiWrite(HANDLE dev, BYTE *writeBuffer, UINT offsetSector, UINT writeSizeSector)
{
	TRACE(_T("Write setor: 0x%X, Len: 0x%X\n"), offsetSector, writeSizeSector);
	_stCDB cdb = {0};
	cdb.bCDB[0] = 0x2A;
	cdb.bCDB[1] = 0x00;
	cdb.bCDB[2] = (offsetSector >> 24) & 0xff;
	cdb.bCDB[3] = (offsetSector >> 16) & 0xff;
	cdb.bCDB[4] = (offsetSector >> 8) & 0xff;
	cdb.bCDB[5] = offsetSector & 0xff;
	cdb.bCDB[6] = 0x00;
	cdb.bCDB[7] = (writeSizeSector >> 8) & 0xff;
	cdb.bCDB[8] = writeSizeSector & 0xff;
	cdb.bCDB[9] = 0x00;

	DWORD status = ScsiCmdSend(dev, cdb, SCSI_IOCTL_DATA_OUT, 10, (void *)writeBuffer, SECTOR_SIZE * writeSizeSector, 2);
	TRACE(_T("status: 0x%X\n"), status);
	return status;
}

BOOL PrintBuffer(BYTE *buffer, UINT bufferLen)
{
	CHAR printBuffer[10] = {0};
	for (size_t i = 0; i < bufferLen; i++)
	{
		if (i % 0x10 == 0)
		{
			sprintf_s(printBuffer, "0x%04X: ", (UINT)i);
			OutputDebugStringA(printBuffer);
		}

		sprintf_s(printBuffer, " %02X", buffer[i]);
		OutputDebugStringA(printBuffer);

		if (i % 0x10 == 0xF)
		{
			OutputDebugStringA("\n");
		}
	}
	return true;
}