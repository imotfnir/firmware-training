//  need to include WinDDK in project
#pragma once

#include "scsi.h"

#include "pch.h"
#include <WinIoCtl.h>
#include "devioctl.h"
#include <ntddscsi.h>
#include <Windows.h>

typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE
{
	SCSI_PASS_THROUGH_DIRECT sptd;
	DWORD filler;	// align abRequestSense to DWORD boundary
	BYTE abRequestSense[32];
} SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE, *PSCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE;

//struct _stCDB
//{
//	BYTE bCDB[16];
//};

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

	if ((sptd.sptd.ScsiStatus == 0) && (apiStatus != 0))
		return 0;

	return GetLastError();
}

DWORD ScsiRead(HANDLE dev, BYTE* readBuffer, UINT offsetSector, UINT readSize) {
	_stCDB cdb = {0};
	cdb.bCDB[0] = 0x28;
	cdb.bCDB[1] = 0x00;
	cdb.bCDB[2] = (offsetSector >> 24) & 0xff;
	cdb.bCDB[3] = (offsetSector >> 16) & 0xff;
	cdb.bCDB[4] = (offsetSector >> 8) & 0xff;
	cdb.bCDB[5] = offsetSector & 0xff;
	cdb.bCDB[6] = 0x00;
	cdb.bCDB[7] = (readSize >> 8) & 0xff;
	cdb.bCDB[8] = readSize & 0xff;
	cdb.bCDB[9] = 0x00;

	return ScsiCmdSend(dev, cdb, SCSI_IOCTL_DATA_IN, 10, (void *)readBuffer, SECTOR_SIZE, 2);
}