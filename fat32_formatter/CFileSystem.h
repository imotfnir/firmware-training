#ifndef C_FILE_SYSTEM_H
#define C_FILE_SYSTEM_H

#pragma once

#define SECTOR_SIZE 0x200U

typedef enum
{
	Fat32,
	ExFat
} FILE_SYSTEM_TYPE;

#pragma pack(push, 1)
typedef struct
{
	BYTE bootIndicator;
	BYTE startingChs[3];
	BYTE osType;
	BYTE endingChs[3];
	DWORD startingLba;
	DWORD sizeInLba;
} MBR_PARTITION_RECORD;

typedef struct
{
	BYTE bootCode[446];
	MBR_PARTITION_RECORD partitionRecode[4];
	WORD signature;
} MBR_STRUCTURE;

typedef struct
{
	BYTE jmpBoot[3];
	CHAR oemName[8];
	WORD bytesPerSector;
	BYTE sectorsPerCluster;
	WORD reservedSectorCount;
	BYTE numOfFats;
	WORD rootEntryCount;
	WORD totalSectors16;
	BYTE media;
	WORD fatSize16;
	WORD sectorsPerTrack;
	WORD numberOfHeads;
	DWORD hiddenSectors;
	DWORD totalSectors32;
	DWORD fatSize32;
	WORD extFlags;
	WORD fsVersion;
	DWORD firstRootCluster;
	WORD fsInfo;
	WORD backupBootSector;
	BYTE reserved[12];
	BYTE driveNumber;
	BYTE reserved1;
	BYTE bootSignature;
	DWORD volumeID[4];
	CHAR volumeLable[11];
	CHAR fileSystemType[8];
	BYTE zeros[420];
	WORD signatureWord;
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

#pragma pack(pop)

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
FAT32_BOOT_SECTOR PrepareFat32BootSector(HANDLE dev, CFileSystemConfig config);
BOOL InitFat32BootSector(HANDLE dev, CFileSystemConfig config);
DWORD GetDiskSizeSectors(HANDLE dev);
DWORD GetFatTableSizeSectors(DWORD dataSizeSector, BYTE sectorPerCluster);

#endif // C_FILE_SYSTEM_H