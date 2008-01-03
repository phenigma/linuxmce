#ifndef __GENERIC_FILE_HANDLER_H__
#define __GENERIC_FILE_HANDLER_H__
//-----------------------------------------------------------------------------------------------------
#include <string>
#include <list>
using namespace std;
//-----------------------------------------------------------------------------------------------------
class PlutoMediaAttributes;
//-----------------------------------------------------------------------------------------------------
class GenericFileHandler
{
protected:

	string m_sDirectory;
	string m_sFile;
	string m_sFullFilename;
	int m_nMediaType;

public:

	GenericFileHandler(string sDirectory, string sFile);
	virtual ~GenericFileHandler(void);

	virtual bool LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, list<pair<char *, size_t> >& listPicturesForTags);
	virtual bool SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes);
	virtual bool RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes);
	virtual bool FileAttributeExists();
	virtual string GetFileAttribute();
	virtual string GetFileSourceForDB();

	void MediaType(int nMediaType) { m_nMediaType = nMediaType; }
};
//-----------------------------------------------------------------------------------------------------
#endif //__GENERIC_FILE_HANDLER_H__
