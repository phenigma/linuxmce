#include "FileHandlerFactory.h"

#include "VDRInfoFileHandler.h"
#include "ID3FileHandler.h"
#include "PlutoUtils/FileUtils.h"
#include "FileUtils/file_utils.h"
//-----------------------------------------------------------------------------------------------------
FileHandlerFactory::FileHandlerFactory(void)
{
}
//-----------------------------------------------------------------------------------------------------
FileHandlerFactory::~FileHandlerFactory(void)
{
}
//-----------------------------------------------------------------------------------------------------
/*static*/ GenericFileHandler *FileHandlerFactory::CreateFileHandler(string sDirectory, string sFile)
{
	GenericFileHandler *pFileHandler = NULL;

	switch(GetFileHandlerType(sDirectory, sFile))
	{
		case fhtVdr:
			pFileHandler = new VDRInfoFileHandler(sDirectory, sFile);
			break;

		case fhtId3:
			pFileHandler = new ID3FileHandler(sDirectory, sFile);
			break;

		default:
			pFileHandler = new GenericFileHandler(sDirectory, sFile);
			break;
	}

	return pFileHandler;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ FileHandlerType FileHandlerFactory::GetFileHandlerType(string sDirectory, string sFile)
{
	FileHandlerType type = fhtGeneric;

	if(IsValidVDRFile(sDirectory, sFile))
		type = fhtVdr;
	else if(IsValidId3File(sDirectory, sFile))
		type = fhtId3;

	return type;
}	
//-----------------------------------------------------------------------------------------------------
/*static*/ bool FileHandlerFactory::IsValidVDRFile(string sDirectory, string sFile)
{
	return sFile == "001.vdr" && FileUtils::FileExists(sDirectory + "/info.vdr");
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool FileHandlerFactory::IsValidId3File(string sDirectory, string sFile)
{
	string sFullFilename = sDirectory + "/" + sFile;
	return !UpdateMediaFileUtils::IsDirectory(sFullFilename.c_str());
}
//-----------------------------------------------------------------------------------------------------
