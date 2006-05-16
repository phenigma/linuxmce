#ifndef __PLUTO_MEDIA_ATTRIBUTES_H__
#define __PLUTO_MEDIA_ATTRIBUTES_H__
//-----------------------------------------------------------------------------------------------------
#include "../SerializeClass/SerializeClass.h"
#define SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE	2100
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
};
//-----------------------------------------------------------------------------------------------------
typedef multimap<long, PlutoMediaAttribute *> MapPlutoMediaAttributes;
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
	MapPlutoMediaAttributes m_mapAttributes;

	PlutoMediaAttributes();
	~PlutoMediaAttributes();

	bool OkayToDeserialize(int iSC_Version);
	string SerializeClassClassName();
	void SetupSerialization(int iSC_Version);
	PlutoMediaAttributes &operator+ (MapPlutoMediaAttributes &i);
	bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,
		unsigned long &iAllocatedSize,char *&pCurrentPosition);
};
//-----------------------------------------------------------------------------------------------------
#endif
