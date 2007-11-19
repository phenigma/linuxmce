#ifndef __FILE_HANDLER_FACTORY_H__
#define __FILE_HANDLER_FACTORY_H__
//-----------------------------------------------------------------------------------------------------
#include <string>
using namespace std;
//-----------------------------------------------------------------------------------------------------
class GenericFileHandler;
//-----------------------------------------------------------------------------------------------------
enum FileHandlerType
{
	fhtGeneric,
	fhtId3,
	fhtVdr
};
//-----------------------------------------------------------------------------------------------------
class FileHandlerFactory
{
public:
	FileHandlerFactory(void);
	~FileHandlerFactory(void);

	static GenericFileHandler *CreateFileHandler(string sDirectory, string sFile);
	static FileHandlerType GetFileHandlerType(string sDirectory, string sFile);

private:

	static bool IsValidVDRFile(string sDirectory, string sFile);
	static bool IsValidId3File(string sDirectory, string sFile);
};
//-----------------------------------------------------------------------------------------------------
#endif //__FILE_HANDLER_FACTORY_H__
