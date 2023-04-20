#include "pch.h"
#include "CFileSystemConfig.h"


CFileSystemConfig::CFileSystemConfig(){
	this->isMBR = false;
	this->clusterSizeInByte = 0;
	this->fileSystem = Fat32;
	this->offsetOfFatTableInByte = 0;
	this->offsetOfPartitionInByte = 0;
}
