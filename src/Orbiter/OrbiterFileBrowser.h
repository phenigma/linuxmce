#ifndef OrbiterFileBrowser_h
#define OrbiterFileBrowser_h

// Here is a collection of OrbiterFileBrowser_Entry objects which the orbiter
// uses in response to the 'Show File List' command so it knows what screen
// to present the user to pick a file based on a given media type

#include "SerializeClass/SerializeClass.h"
#define SERIALIZE_DATA_TYPE_MAP_OBJFILEBROWSER		1000


class OrbiterFileBrowser_Entry : public SerializeClass
{
public:
	int m_DesignObj,m_DesignObj_Popup,m_MediaType;
	string m_sFilename;

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_DesignObj + m_DesignObj_Popup + m_MediaType + m_sFilename;
	}
};

// int=MediaType
typedef map<int,OrbiterFileBrowser_Entry *> MapOrbiterFileBrowser;

class OrbiterFileBrowser_Collection : public SerializeClass
{
public:
	MapOrbiterFileBrowser m_mapOrbiterFileBrowser;

	~OrbiterFileBrowser_Collection()
	{
		for(MapOrbiterFileBrowser::iterator it=m_mapOrbiterFileBrowser.begin();it!=m_mapOrbiterFileBrowser.end();++it)
			delete it->second;
	}

	OrbiterFileBrowser_Collection &operator+ (MapOrbiterFileBrowser &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MAP_OBJFILEBROWSER,(void *) &i)); return (*this); }
	virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition);

	void SetupSerialization(int iSC_Version)
	{
		(*this) + m_mapOrbiterFileBrowser; // this is serialized custom
	}
};


#endif