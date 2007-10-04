#ifndef __VDR_INFO_FILE_HANDLER_H__
#define __VDR_INFO_FILE_HANDLER_H__
//-----------------------------------------------------------------------------------------------------
#include "GenericFileHandler.h"
//-----------------------------------------------------------------------------------------------------
class VDRInfoFileHandler : public GenericFileHandler
{
public:
	VDRInfoFileHandler(string sDirectory, string sFile);
	~VDRInfoFileHandler(void);

	bool LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, 
		list<pair<char *, size_t> >& listPicturesForTags);
	bool SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes);
	bool RemoveAttribute(int nTagType, string sValue);
	bool FileAttributeExists();
	string GetFileAttribute();
};
//-----------------------------------------------------------------------------------------------------
#endif //__VDR_INFO_FILE_HANDLER_H__
