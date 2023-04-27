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
	mbr.partitionRecode[0].startingLba = config.offsetOfPartitionInByte / 512;
	mbr.partitionRecode[0].sizeInLba = 0; // ToDo
	mbr.signature = 0xAA55;
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
	bootSector.reservedSectorCount = (config.offsetOfFatTableInByte / SECTOR_SIZE); // ToDo
	bootSector.numOfFats = 0x2;
	bootSector.rootEntryCount = 0x0;
	bootSector.totalSectors16 = 0x0;
	bootSector.media = 0xF0;
	bootSector.fatSize16 = 0x0;
	bootSector.sectorsPerTrack = 0x3F;
	bootSector.numberOfHeads = 0xFF;
	if (!config.isMBR)
	{
		bootSector.hiddenSectors = 0x0;
	}
	else
	{
		bootSector.hiddenSectors = (config.offsetOfPartitionInByte / SECTOR_SIZE);
	}
	bootSector.totalSectors32 = bootSector.hiddenSectors - GetDiskSizeSectors(dev); // ToDo
	TRACE(_T("hiddenSectors: 0x%X\n"), bootSector.hiddenSectors);
	TRACE(_T("GetDiskSizeSectors: 0x%X\n"), GetDiskSizeSectors(dev));
	TRACE(_T("totalSectors32: 0x%X\n"), bootSector.totalSectors32);
	bootSector.fatSize32 = GetFatTableSizeSectors(bootSector.totalSectors32, bootSector.sectorsPerCluster); // ToDo
	TRACE(_T("fatSize32: 0x%X\n"), bootSector.fatSize32);

	bootSector.extFlags = 0x0;
	bootSector.fsVersion = 0x2;
	bootSector.firstRootCluster = 0x2;
	bootSector.fsInfo = 0x1;
	bootSector.backupBootSector = 0x6;
	// bootSector.reserved[12] ;
	bootSector.driveNumber = 0x80;
	bootSector.reserved1 = 0x0;
	bootSector.bootSignature = 0x29;
	// bootSector.volumeID[4]
	memcpy(bootSector.volumeLable, "NO NAME    ", sizeof(FAT32_BOOT_SECTOR::volumeLable));
	memcpy(bootSector.fileSystemType, "FAT32   ", sizeof(FAT32_BOOT_SECTOR::fileSystemType));
	// bootSector.zeros[420]
	bootSector.signatureWord = 0xAA55;

	return bootSector;
}

BOOL InitFat32BootSector(HANDLE dev, CFileSystemConfig config)
{
	FAT32_BOOT_SECTOR bootSector = PrepareFat32BootSector(dev, config);

	PrintBuffer((BYTE *)&bootSector, SECTOR_SIZE);
	return true;
}

DWORD GetDiskSizeSectors(HANDLE dev)
{
	DISK_GEOMETRY diskGeometry;
	BOOL status;
	DWORD returnStatus;
	status = DeviceIoControl(dev, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &diskGeometry, sizeof(diskGeometry), &returnStatus, NULL);
	TRACE(_T("Cylinders: %d\n", Cylinders));
	TRACE(_T("MediaType: %d\n", MediaType));
	TRACE(_T("TracksPerCylinder: %d\n", TracksPerCylinder));
	TRACE(_T("SectorsPerTrack: %d\n", SectorsPerTrack));
	TRACE(_T("BytesPerSector: %d\n", BytesPerSector));

	return 0;
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

CFileSystemConfig::CFileSystemConfig()
{
	this->isMBR = false;
	this->clusterSizeInByte = 8192;
	this->fileSystem = Fat32;
	this->offsetOfFatTableInByte = 2048;
	this->offsetOfPartitionInByte = 8192;
	this->diskPath = "";
}
