#ifndef __PLUTO_MEDIA_ATTRIBUTES_H__
#define __PLUTO_MEDIA_ATTRIBUTES_H__
//-----------------------------------------------------------------------------------------------------
#include "../SerializeClass/SerializeClass.h"
#define SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE	2100
#define SERIALIZE_DATA_TYPE_MULTIMAP_PICTURES		2101
#define SERIALIZE_PLUTO_MEDIA_ATTRIBUTES_VERSION	1
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaAttribute class
//
//----------------------------------------------------------------------------------------------------
class PlutoMediaAttribute : public SerializeClass
{
public:

	int m_nType;
	string m_sName;
	int m_nTrack;
	int m_nSection;

	PlutoMediaAttribute();
	PlutoMediaAttribute(int nType, string sName, int nTrack = 0, int nSection = 0);
	~PlutoMediaAttribute();

	string SerializeClassClassName();
	void SetupSerialization(int iSC_Version);

	bool operator ==(PlutoMediaAttribute& attribute);
	PlutoMediaAttribute& operator =(PlutoMediaAttribute& attribute);
};
//-----------------------------------------------------------------------------------------------------
typedef multimap<long, PlutoMediaAttribute *> MapPlutoMediaAttributes;
typedef multimap<unsigned long, char *> MapPictures;
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaAttributes class
//
//-----------------------------------------------------------------------------------------------------
class PlutoMediaAttributes : public SerializeClass
{
public:

	int m_nInstallationID;
	int m_nFileID;
	int m_nPictureID;
	string m_sPictureUrl;
	string m_sStartPosition;
	MapPlutoMediaAttributes m_mapAttributes;
	MapPictures m_mmapBookmarks;
	MapPictures m_mmapCoverArts;

	PlutoMediaAttributes();
	~PlutoMediaAttributes();

	bool OkayToDeserialize(int iSC_Version);
	string SerializeClassClassName();
	void SetupSerialization(int iSC_Version);

	PlutoMediaAttributes &operator+ (MapPlutoMediaAttributes &i);
	PlutoMediaAttributes &operator+ (MapPictures &i);

	bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,
		unsigned long &iAllocatedSize,char *&pCurrentPosition);
};
//-----------------------------------------------------------------------------------------------------
#endif
