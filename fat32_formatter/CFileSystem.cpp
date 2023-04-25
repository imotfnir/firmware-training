#include "CFileSystem.h"

#include "pch.h"
#include "scsi.h"

MBR_STRUCTURE PrepareMbrStructure(CFileSystemConfig config)
{
	MBR_STRUCTURE mbr = {0};

	mbr.partitionRecode[0].bootIndicator = 0x0;
	mbr.partitionRecode[0].startingChs[0] = 0x20;
	mbr.partitionRecode[0].startingChs[1] = 0x21;
	mbr.partitionRecode[0].startingChs[2] = 0x00;
	mbr.partitionRecode[0].osType = 0xC; // FAT32 osType
	mbr.partitionRecode[0].endingChs[0] = 0xFE;
	mbr.partitionRecode[0].endingChs[1] = 0xFF;
	mbr.partitionRecode[0].endingChs[2] = 0xFF;
	mbr.partitionRecode[0].startingLba[0] = config.offsetOfPartitionInByte / 512;
	mbr.partitionRecode[0].startingLba[1] = (config.offsetOfPartitionInByte / 512) << 8;
	mbr.partitionRecode[0].startingLba[2] = (config.offsetOfPartitionInByte / 512) << 16;
	mbr.partitionRecode[0].startingLba[3] = (config.offsetOfPartitionInByte / 512) << 24;
	mbr.partitionRecode[0].sizeInLba[0] = 0; // ToDo
	mbr.signature[0] = 0x55;
	mbr.signature[1] = 0xaa;
	return mbr;
}

BOOL InitMbrStructure(HANDLE dev, CFileSystemConfig config)
{
	MBR_STRUCTURE mbr = PrepareMbrStructure(config);
	BYTE *writeBuffer = (BYTE *)&mbr;

	PrintBuffer(writeBuffer, SECTOR_SIZE);
	ScsiWrite(dev, writeBuffer, 0, 1);

	TRACE(_T("%d"), sizeof(mbr));

	return true;
}

CFileSystemConfig::CFileSystemConfig()
{
	this->isMBR = false;
	this->clusterSizeInByte = 0;
	this->fileSystem = Fat32;
	this->offsetOfFatTableInByte = 0;
	this->offsetOfPartitionInByte = 0;
}
