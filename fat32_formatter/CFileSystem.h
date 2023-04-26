#ifndef C_FILE_SYSTEM_H
#define C_FILE_SYSTEM_H

#pragma once

#define SECTOR_SIZE 0x200U

typedef enum
{
	Fat32,
	ExFat
} FILE_SYSTEM_TYPE;

typedef struct
{
	BYTE bootIndicator;
	BYTE startingChs[3];
	BYTE osType;
	BYTE endingChs[3];
	BYTE startingLba[4];
	BYTE sizeInLba[4];
} MBR_PARTITION_RECORD;

typedef struct
{
	BYTE bootCode[446];
	MBR_PARTITION_RECORD partitionRecode[4];
	BYTE signature[2];
} MBR_STRUCTURE;

typedef struct
{
	BYTE jmpBoot[3];
	BYTE oemName[8];
	BYTE bytesPerSector[2];
	BYTE sectorsPerCluster;
	BYTE reservedSectorCount[2];
	BYTE numOfFats;
	BYTE rootEntryCount[2];
	BYTE totalSectors16[2];
	BYTE media;
	BYTE fatSize16[2];
	BYTE sectorsPerTrack[2];
	BYTE numberOfHeads[2];
	BYTE hiddenSectors[4];
	BYTE totalSectors32[4];
	BYTE fatSize32[4];
	BYTE extFlags[2];
	BYTE fsVersion[2];
	BYTE firstRootCluster[4];
	BYTE fsInfo[2];
	BYTE backupBootsector[2];
	BYTE reserved[12];
	BYTE driveNumber;
	BYTE reserved1;
	BYTE bootSignature;
	BYTE volumeID[4];
	BYTE volumeLable[11];
	BYTE fileSystemType[8];
	BYTE zeros[420];
	BYTE signatureWord[2];
} FAT32_BOOT_SECTOR;

typedef struct
{
	DWORD leadSignature;	  // 0x41615252
	BYTE reserved1[480];	  // zeros
	DWORD structureSignature; // 0x61417272
	DWORD freeCount;		  // 0xffffffff
	DWORD nextFree;			  // 0xffffffff
	BYTE reserved2[12];		  // zeros
	DWORD trailSignature;	  // 0xaa550000
} FAT32_FSINFO;

typedef struct
{

} FAT32_FAT_TABLE;

typedef struct
{

} FAT32_ROOT_DIRECTORY;

class CFileSystemConfig
{
	// Construction
public:
	CFileSystemConfig();

public:
	UINT isMBR;
	UINT clusterSizeInByte;
	FILE_SYSTEM_TYPE fileSystem;
	UINT offsetOfFatTableInByte;
	UINT offsetOfPartitionInByte;
};

MBR_STRUCTURE PrepareMbrStructure(CFileSystemConfig config);
BOOL InitMbrStructure(HANDLE dev, CFileSystemConfig config);
FAT32_BOOT_SECTOR PrepareFat32BootSector(CFileSystemConfig config);
BOOL InitFat32BootSector(HANDLE dev, CFileSystemConfig config);

#endif // C_FILE_SYSTEM_H