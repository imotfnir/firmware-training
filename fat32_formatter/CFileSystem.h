#ifndef C_FILE_SYSTEM_H
#define C_FILE_SYSTEM_H

#pragma once

#define SECTOR_SIZE 0x200U

typedef enum {
	Fat32,
	ExFat
}FILE_SYSTEM_TYPE;

typedef struct {
	BYTE bootIndicator;
	BYTE startingChs[3];
	BYTE osType;
	BYTE endingChs[3];
	BYTE startingLba[4];
	BYTE sizeInLba[4];
}MBR_PARTITION_RECORD;

typedef struct {
	BYTE bootCode[446];
	MBR_PARTITION_RECORD partitionRecode[4];
	BYTE signature[2];
}MBR_STRUCTURE;

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

#endif //C_FILE_SYSTEM_H