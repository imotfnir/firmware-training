#pragma once
typedef enum {
	Fat32,
	ExFat
}FILE_SYSTEM_TYPE;

class CFileSystemConfig
{
// Construction
public:
	CFileSystemConfig();

public:
	BOOL isMBR;
	UINT clusterSizeInByte;
	FILE_SYSTEM_TYPE fileSystem;
	UINT offsetOfFatTableInByte;
	UINT offsetOfPartitionInByte;

};

