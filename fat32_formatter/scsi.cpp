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

DWORD ScsiCmdSend(HANDLE hDev, _stCDB stCDB, BYTE bDirection, BYTE bCdbLen, void *pData, DWORD dwDataXferLen, DWORD dwTimeout);
BOOL scsi_read(HANDLE dev, BYTE* readBuffer, UINT offsetSector, UINT readSize);

DWORD ScsiCmdSend(HANDLE hDev, _stCDB stCDB, BYTE bDirection, BYTE bCdbLen, void *pData, DWORD dwDataXferLen, DWORD dwTimeout)
{
	BOOL xAPIStatus = FALSE;
	BYTE abRequestSense[32] = {0};
	DWORD dwByteReturn;

	SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE sptd = {0};
	sptd.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	sptd.sptd.PathId = 0;
	sptd.sptd.TargetId = 1;
	sptd.sptd.Lun = 0;
	sptd.sptd.CdbLength = bCdbLen;
	sptd.sptd.DataIn = (BYTE)bDirection;
	sptd.sptd.SenseInfoLength = sizeof(sptd.abRequestSense);
	sptd.sptd.DataTransferLength = dwDataXferLen;
	sptd.sptd.TimeOutValue = dwTimeout;
	sptd.sptd.DataBuffer = (pData == NULL) ? abRequestSense : pData;
	sptd.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE, abRequestSense);

	memcpy(sptd.sptd.Cdb, &stCDB, sizeof(sptd.sptd.Cdb));

	xAPIStatus = DeviceIoControl(hDev,
								IOCTL_SCSI_PASS_THROUGH_DIRECT,
								&sptd,
								sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE),
								&sptd,
								sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE),
								&dwByteReturn,
								FALSE);

	if ((sptd.sptd.ScsiStatus == 0) && (xAPIStatus != 0))
		return 0;

	return GetLastError();
}

BOOL scsi_read(HANDLE dev, BYTE* readBuffer, UINT offsetSector, UINT readSize) {
	BOOL apiStatus = FALSE;
	BYTE abRequestSense[32] = { 0 };
	DWORD byteReturn;
	SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE sptd = { 0 };

	sptd.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	sptd.sptd.PathId = 0;
	sptd.sptd.TargetId = 1;
	sptd.sptd.Lun = 0;
	sptd.sptd.CdbLength = 10;
	sptd.sptd.DataIn = SCSI_IOCTL_DATA_IN;
	sptd.sptd.SenseInfoLength = 24;
	sptd.sptd.DataTransferLength = 0x200;
	sptd.sptd.TimeOutValue = 2;
	sptd.sptd.DataBuffer = readBuffer;
	sptd.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE, abRequestSense);

	sptd.sptd.Cdb[0] = 0x28;        //opcode: Host read data from storage device
	sptd.sptd.Cdb[1] = 0x00;
	sptd.sptd.Cdb[2] = (offsetSector >> 24) & 0xff;
	sptd.sptd.Cdb[3] = (offsetSector >> 16) & 0xff;
	sptd.sptd.Cdb[4] = (offsetSector >> 8) & 0xff;
	sptd.sptd.Cdb[5] = offsetSector & 0xff;
	sptd.sptd.Cdb[6] = 0x00;
	sptd.sptd.Cdb[7] = (readSize >> 8) & 0xff;
	sptd.sptd.Cdb[8] = readSize & 0xff;
	sptd.sptd.Cdb[9] = 0x00;

	apiStatus = DeviceIoControl(dev,
		IOCTL_SCSI_PASS_THROUGH_DIRECT,
		&sptd,
		sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE),
		&sptd,
		sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_REQSENSE),
		&byteReturn,
		FALSE);

	return apiStatus;
}