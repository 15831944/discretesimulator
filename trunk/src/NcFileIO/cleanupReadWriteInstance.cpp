#include "NcFileIO\cleanupReadWriteInstance.h"
#include "NcFileIO\NcFileReadWriteManager.h"
#include "NcFileIO\NcFanuc0TFileReader.h"
#include "NcFileIO\NcFanuc10T11TFileReader.h"
#include "NcFileIO\NcSinumerikFileReader.h"
using namespace DiscreteSimulator;

cleanupReadWriteInstance::~cleanupReadWriteInstance()
{
	//delete NcFileReadWriteManager::mNcFileReadWriteInstance;
	//NcFileReadWriteManager::mNcFileReadWriteInstance = 0;

	//delete NcFanuc0TFileReader::mFanuc0TFileReaderInstance;
	//NcFanuc0TFileReader::mFanuc0TFileReaderInstance = 0;

	//delete NcFanuc10T11TFileReader::mNcFileReaderInstance;
	//NcFanuc10T11TFileReader::mNcFileReaderInstance = 0;

	//delete  NcSinumerikFileReader::mReaderInstance;
	// NcSinumerikFileReader::mReaderInstance = 0;

}