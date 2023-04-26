#include "CFileSystem.h"

#include "pch.h"
#include "scsi.h"

MBR_STRUCTURE PrepareMbrStructure(CFileSystemConfig config)
{
	MBR_STRUCTURE mbr = {0};
	TRACE(_T("sizeof(MBR_STRUCTURE): 0x%X\n"), sizeof(mbr));

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
	// ScsiWrite(dev, writeBuffer, 0, 1);

	return true;
}

FAT32_BOOT_SECTOR PrepareFat32BootSector(CFileSystemConfig config)
{
	FAT32_BOOT_SECTOR bootSector = {0};
	TRACE(_T("sizeof(FAT32_BOOT_SECTOR): 0x%X\n"), sizeof(bootSector));

	bootSector.jmpBoot[0] = 0xEB;
	bootSector.jmpBoot[1] = 0x58;
	bootSector.jmpBoot[2] = 0x90;
	memcpy(bootSector.oemName, "MSDOS5.0", sizeof(FAT32_BOOT_SECTOR::oemName));
	bootSector.bytesPerSector[0] = SECTOR_SIZE & 0xFF;
	bootSector.bytesPerSector[1] = SECTOR_SIZE >> 8;
	bootSector.sectorsPerCluster = config.clusterSizeInByte / SECTOR_SIZE;
	bootSector.reservedSectorCount[0] = (config.offsetOfFatTableInByte / SECTOR_SIZE) & 0Xff; //ToDo
	bootSector.reservedSectorCount[1] = (config.offsetOfFatTableInByte / SECTOR_SIZE) >> 8; //ToDo
	bootSector.numOfFats = 0x2;
	bootSector.rootEntryCount[0] = 0x2;
	bootSector.rootEntryCount[1] = 0x0;
	bootSector.totalSectors16[0] = 0x0;
	bootSector.totalSectors16[1] = 0x0;
	bootSector.media = 0xF0;
	bootSector.fatSize16[0] = 0x0;
	bootSector.fatSize16[1] = 0x0;
	bootSector.sectorsPerTrack[0] = 0x3F;
	bootSector.sectorsPerTrack[1] = 0x0;
	bootSector.numberOfHeads[0] = 0xFF;
	bootSector.numberOfHeads[1] = 0x0;
	if (!config.isMBR)
	{
		bootSector.hiddenSectors[0] = 0x0;
		bootSector.hiddenSectors[1] = 0x0;
		bootSector.hiddenSectors[2] = 0x0;
		bootSector.hiddenSectors[3] = 0x0;
	}
	else
	{
		// ToDo
		bootSector.hiddenSectors[0] = (config.offsetOfPartitionInByte / SECTOR_SIZE) & 0XFF;
		bootSector.hiddenSectors[1] = (config.offsetOfPartitionInByte / SECTOR_SIZE) >> 8 & 0xFF;
		bootSector.hiddenSectors[2] = (config.offsetOfPartitionInByte / SECTOR_SIZE) >> 16 & 0xFF;
		bootSector.hiddenSectors[3] = (config.offsetOfPartitionInByte / SECTOR_SIZE) >> 24 & 0xFF;
	}
	bootSector.totalSectors32[2] = 0x1; //ToDo
	bootSector.fatSize32[1] = 0x1; //ToDo
	bootSector.extFlags[0] = 0x0;
	bootSector.extFlags[1] = 0x0;
	bootSector.extFlags[2] = 0x0;
	bootSector.extFlags[3] = 0x0;
	bootSector.fsVersion[0] = 0x2;
	bootSector.fsVersion[1] = 0x0;
	bootSector.firstRootCluster[0] = 0x2;
	bootSector.firstRootCluster[1] = 0x0;
	bootSector.firstRootCluster[2] = 0x0;
	bootSector.firstRootCluster[3] = 0x0;
	bootSector.fsInfo[0] = 0x1;
	bootSector.fsInfo[1] = 0x0;
	bootSector.backupBootsector[0] = 0x6;
	bootSector.backupBootsector[1] = 0x0;
	// bootSector.reserved[12] ;
	bootSector.driveNumber = 0x80;
	bootSector.reserved1 = 0x0;
	bootSector.bootSignature = 0x29;
	// bootSector.volumeID[4]
	memcpy(bootSector.volumeLable, "NO NAME    ", 11);
	memcpy(bootSector.fileSystemType, "FAT32   ", 8);
	// bootSector.zeros[420]
	bootSector.signatureWord[0] = 0x55;
	bootSector.signatureWord[1] = 0xAA;

	return bootSector;
}

BOOL InitFat32BootSector(HANDLE dev, CFileSystemConfig config)
{
	FAT32_BOOT_SECTOR bootSector = PrepareFat32BootSector(config);

	PrintBuffer((BYTE *)&bootSector, SECTOR_SIZE);
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
