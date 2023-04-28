#include "CFileSystem.h"

#include "scsi.h"
#include "pch.h"

#include <cmath>

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
	mbr.partitionRecode[0].startingLba = config.offsetOfPartitionInSector;
	mbr.partitionRecode[0].sizeInLba = 0; // ToDo
	mbr.signature = 0xAA55;
	return mbr;
}

BOOL InitMbrStructure(HANDLE dev, CFileSystemConfig config)
{
	MBR_STRUCTURE mbr = PrepareMbrStructure(config);

	PrintBuffer((BYTE *)&mbr, sizeof(mbr));
	ScsiWrite(dev, (BYTE *)&mbr, 0, 1);

	return true;
}

FAT32_BOOT_SECTOR PrepareFat32BootSector(HANDLE dev, CFileSystemConfig config)
{
	FAT32_BOOT_SECTOR bootSector = {0};
	TRACE(_T("sizeof(FAT32_BOOT_SECTOR): 0x%X\n"), sizeof(bootSector));

	bootSector.jmpBoot[0] = 0xEB;
	bootSector.jmpBoot[1] = 0x58;
	bootSector.jmpBoot[2] = 0x90;
	memcpy(bootSector.oemName, "MSDOS5.0", sizeof(FAT32_BOOT_SECTOR::oemName));
	bootSector.bytesPerSector = SECTOR_SIZE;
	bootSector.sectorsPerCluster = config.clusterSizeInByte / SECTOR_SIZE;
	bootSector.reservedSectorCount = config.offsetOfFatTableInSector; // ToDo
	bootSector.numOfFats = 0x2;
	bootSector.rootEntryCount = 0x0;
	bootSector.totalSectors16 = 0x0;
	bootSector.media = 0xF0;
	bootSector.fatSize16 = 0x0;
	bootSector.sectorsPerTrack = 0x3F;
	bootSector.numberOfHeads = 0xFF;
	bootSector.hiddenSectors = config.offsetOfPartitionInSector;
	bootSector.totalSectors32 = GetDiskSizeSectors(dev) - config.offsetOfPartitionInSector;
	bootSector.fatSize32 = GetFatTableSizeSectors(GetDiskSizeSectors(dev) - config.offsetOfPartitionInSector, bootSector.sectorsPerCluster); // ToDo
	bootSector.extFlags = 0x0;
	bootSector.fsVersion = 0x2;
	bootSector.firstRootCluster = 0x2;
	bootSector.fsInfo = 0x1;
	bootSector.backupBootSector = 0x6;
	bootSector.driveNumber = 0x80;
	bootSector.reserved1 = 0x0;
	bootSector.bootSignature = 0x29;
	bootSector.volumeID = GetVolumeId();
	memcpy(bootSector.volumeLable, "NO NAME    ", sizeof(FAT32_BOOT_SECTOR::volumeLable));
	memcpy(bootSector.fileSystemType, "FAT32   ", sizeof(FAT32_BOOT_SECTOR::fileSystemType));
	bootSector.signatureWord = 0xAA55;

	return bootSector;
}

BOOL InitFat32BootSector(HANDLE dev, CFileSystemConfig config)
{
	FAT32_BOOT_SECTOR bootSector = PrepareFat32BootSector(dev, config);

	PrintBuffer((BYTE *)&bootSector, sizeof(bootSector));

	ScsiWrite(dev, (BYTE *)&bootSector, config.offsetOfPartitionInSector, 1);
	return true;
}

DWORD GetDiskSizeSectors(HANDLE dev)
{
	GET_LENGTH_INFORMATION lengthInfo;
	DWORD returnStatus;
	ULONGLONG diskSizeByte;
	DWORD diskSizeSector;

	if (!DeviceIoControl(dev, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &lengthInfo, sizeof(lengthInfo), &returnStatus, NULL))
	{
		TRACE(_T("Failed to get disk length!\n"));
		return 0;
	}

	diskSizeByte = lengthInfo.Length.QuadPart;
	diskSizeSector = (DWORD)(diskSizeByte / SECTOR_SIZE);
	TRACE(_T("Disk size: 0x%llX B\n"), diskSizeByte);
	TRACE(_T("Disk size: 0x%X Sector\n"), diskSizeSector);
	if (diskSizeByte >= 0x20000000000U)
	{
		return 0xFFFFFFFFU;
	}
	if (diskSizeSector < 0x10000U)
	{
		return 0x10000U;
	}
	return diskSizeSector;
}

DWORD GetFatTableSizeSectors(DWORD dataSizeSector, BYTE sectorPerCluster)
{
	UINT numberOfCluster = dataSizeSector / sectorPerCluster;
	DWORD numberOfSector;

	numberOfSector = (numberOfCluster * 4) / SECTOR_SIZE; // 1 FAT entry is 32bits aka 4bytes
	if ((numberOfCluster * 4) % SECTOR_SIZE != 0)
	{
		numberOfSector++;
	}

	return numberOfSector;
}

BOOL DeviceLock(HANDLE dev)
{
	DWORD returnStatus;
	if (0 == DeviceIoControl(dev, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &returnStatus, NULL))
		return false;
	return true;
}
BOOL DeviceUnLock(HANDLE dev)
{
	DWORD returnStatus;
	if (0 == DeviceIoControl(dev, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &returnStatus, NULL))
		return false;
	return true;
}

DWORD GetVolumeId()
{
	SYSTEMTIME s;
	DWORD d;
	WORD lo, hi, tmp;
	GetLocalTime(&s);
	lo = s.wDay + (s.wMonth << 8);
	tmp = (s.wMilliseconds / 10) + (s.wSecond << 8);
	lo += tmp;
	hi = s.wMinute + (s.wHour << 8);
	hi += s.wYear;
	d = lo + (hi << 16);
	return d;
}

FAT32_FSINFO PrepareFat32FsInfo(HANDLE dev, CFileSystemConfig config)
{
	FAT32_FSINFO fsInfo = {0};
	TRACE(_T("sizeof(FAT32_FSINFO): 0x%X\n"), sizeof(fsInfo));

	fsInfo.leadSignature = 0x41615252;
	fsInfo.structureSignature = 0x61417272;
	fsInfo.freeCount = 0xffffffff;
	fsInfo.nextFree = 0xffffffff;
	fsInfo.trailSignature = 0xaa550000;

	return fsInfo;
}

BOOL InitFat32FsInfo(HANDLE dev, CFileSystemConfig config)
{
	FAT32_FSINFO fsInfo = PrepareFat32FsInfo(dev, config);

	PrintBuffer((BYTE *)&fsInfo, sizeof(fsInfo));

	ScsiWrite(dev, (BYTE *)&fsInfo, config.offsetOfPartitionInSector + 1, 1);
	return true;
}

BOOL InitFat32FatStructure(HANDLE dev, CFileSystemConfig config)
{
	FAT32_FAT_TABLE fatStructure = {0};
	UINT fatStructureStartingSector;
	UINT fatStructureSize;
	BYTE zeros[SECTOR_SIZE] = {0};

	fatStructureStartingSector = config.offsetOfPartitionInSector + config.offsetOfFatTableInSector;
	fatStructureSize = (UINT)GetFatTableSizeSectors(GetDiskSizeSectors(dev) - config.offsetOfPartitionInSector, config.clusterSizeInByte / SECTOR_SIZE);

	fatStructure.entry[0] = 0x0FFFFFF8;
	fatStructure.entry[1] = 0xFFFFFFFF;
	fatStructure.entry[2] = 0x0FFFFFFF;

	// Clear fat structure
	for (UINT i = 0; i < 2 * fatStructureSize; i++) // 2 FAT Table
	{
		ScsiWrite(dev, zeros, fatStructureStartingSector + i, 1);
	}

	// Fill fat1, fat2
	ScsiWrite(dev, (BYTE *)&fatStructure, fatStructureStartingSector, 1);
	ScsiWrite(dev, (BYTE *)&fatStructure, fatStructureStartingSector + fatStructureSize, 1);
	PrintBuffer((BYTE *)&fatStructure, sizeof(fatStructure));

	return true;
}

BOOL ClearRootDirectory(HANDLE dev, CFileSystemConfig config)
{
	return true;
}

CFileSystemConfig::CFileSystemConfig()
{
	this->isMBR = false;
	this->clusterSizeInByte = 8192;
	this->offsetOfFatTableInSector = 4;
	this->offsetOfPartitionInSector = 4;
	this->fatStructureSizeInSector = 0;
	this->offsetOfDataRegionInSector = 0;
	this->offsetOfFatStructureInSector = 0;
	this->diskPath = "";
}

BOOL CFileSystemConfig::InitConfig()
{
	if (this->diskPath == "")
	{
		AfxMessageBox(_T("Please select disk path"), MB_ICONWARNING | MB_OK);
		return false;
	}
	return true;
}
