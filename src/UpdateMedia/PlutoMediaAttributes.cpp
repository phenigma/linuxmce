#include "PlutoMediaAttributes.h"

#include "Logger.h"
using namespace DCE;

//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaAttribute class
//
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttribute::PlutoMediaAttribute() : 
	SerializeClass(), 
	m_nType(0), m_nTrack(0), m_nSection(0)
{
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttribute::PlutoMediaAttribute(int nType, string sName, int nTrack/* = 0*/, int nSection/* = 0*/) :
	SerializeClass(), 
	m_nType(nType), m_sName(sName), m_nTrack(nTrack), m_nSection(nSection)
{

}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttribute::~PlutoMediaAttribute()
{
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaAttribute::SerializeClassClassName() 
{ 
	return "PlutoMediaAttribute"; 
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaAttribute::SetupSerialization(int /*iSC_Version*/)
{
	StartSerializeList() + m_nType + m_sName + m_nTrack + m_nSection;
}
//-----------------------------------------------------------------------------------------------------
bool PlutoMediaAttribute::operator ==(PlutoMediaAttribute& attribute)
{
	return 
		attribute.m_nSection	== m_nSection	&&
		attribute.m_nTrack		== m_nTrack		&&
		attribute.m_nType		== m_nType		&&
		attribute.m_sName		== m_sName;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttribute& PlutoMediaAttribute::operator =(PlutoMediaAttribute& attribute)
{
	m_nSection = attribute.m_nSection;
	m_nTrack = attribute.m_nTrack;
	m_nType = attribute.m_nType;
	m_sName = attribute.m_sName;

	return *this;
}
//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaAttributes class
//
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes::PlutoMediaAttributes() : 
	SerializeClass(), 
	m_nInstallationID(0), m_nFileID(0), m_nPictureID(0)
{
	m_iSC_Version = SERIALIZE_PLUTO_MEDIA_ATTRIBUTES_VERSION;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes::~PlutoMediaAttributes()
{
	for(MapPlutoMediaAttributes::iterator it = m_mapAttributes.begin(), end = m_mapAttributes.end(); it != end; ++it)
		delete it->second;
	for(MapPlutoMediaAttributes::iterator it = m_mapLongAttributes.begin(), end = m_mapLongAttributes.end(); it != end; ++it)
		delete it->second;
	for(MapPictures::iterator itp = m_mapBookmarks.begin(), endp = m_mapBookmarks.end(); itp != endp; ++itp)
		delete [] itp->second;
	for(MapPictures::iterator itp = m_mapBookmarksThumbs.begin(), endp = m_mapBookmarksThumbs.end(); itp != endp; ++itp)
		delete [] itp->second;
	for(MapPictures::iterator itp = m_mapCoverArts.begin(), endp = m_mapCoverArts.end(); itp != endp; ++itp)
		delete [] itp->second;
	for(MapPictures::iterator itp = m_mapCoverArtsThumbs.begin(), endp = m_mapCoverArtsThumbs.end(); itp != endp; ++itp)
		delete [] itp->second;
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaAttributes::SerializeClassClassName() 
{ 
	return "PlutoMediaAttributes"; 
}
//-----------------------------------------------------------------------------------------------------
bool PlutoMediaAttributes::OkayToDeserialize(int iSC_Version)
{
	return SERIALIZE_PLUTO_MEDIA_ATTRIBUTES_VERSION == iSC_Version;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaAttributes::SetupSerialization(int /*iSC_Version*/)
{
	StartSerializeList() + m_nInstallationID + m_nFileID + m_nPictureID + m_sPictureUrl;
	
	(*this) + m_mapAttributes;
	(*(static_cast<SerializeClass *>(this))) + m_sStartPosition;
	(*this) + m_mapLongAttributes;
	(*this) + m_mapBookmarks;
	(*this) + m_mapBookmarksThumbs;
	(*this) + m_mapCoverArts;
	(*this) + m_mapCoverArtsThumbs;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes &PlutoMediaAttributes::operator+ (MapPlutoMediaAttributes &i) 
{ 
	m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE,(void *) &i)); 
	return (*this); 
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes &PlutoMediaAttributes::operator+ (MapPictures &i) 
{ 
	m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MULTIMAP_PICTURES,(void *) &i)); 
	return (*this); 
}
//-----------------------------------------------------------------------------------------------------
bool PlutoMediaAttributes::UnknownSerialize(ItemToSerialize *pItem,bool bWriting,
	char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition)
{
	m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
	if( bWriting )
	{
		switch(pItem->m_iSerializeDataType)
		{
			case SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE:
				{
					MapPlutoMediaAttributes *pMap = (MapPlutoMediaAttributes *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
					for(MapPlutoMediaAttributes::iterator it = pMap->begin(); it!=pMap->end(); ++it)
					{
						Write_long((unsigned long) it->first);
						it->second->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					}
					return true;  // We handled it
				}
				break;

			case SERIALIZE_DATA_TYPE_MULTIMAP_PICTURES:
				{
					MapPictures *pMap = (MapPictures *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
					for(MapPictures::iterator it = pMap->begin(); it!=pMap->end(); ++it)
					{
						Write_unsigned_long(it->first);
						Write_block(it->second, it->first);
					}
					return true;  // We handled it
				}
				break;
		};
	}
	else
	{
		switch(pItem->m_iSerializeDataType)
		{
			case SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE:
				{
					MapPlutoMediaAttributes *pMap = (MapPlutoMediaAttributes *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
					for(unsigned long i = 0; i<count; ++i)
					{
						long ID = Read_long();
						PlutoMediaAttribute *pPlutoMediaAttribute = new PlutoMediaAttribute();
						pPlutoMediaAttribute->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
						pMap->insert(std::make_pair(ID, pPlutoMediaAttribute));
					}
					return true;  // We handled it
				}
				break;

			case SERIALIZE_DATA_TYPE_MULTIMAP_PICTURES:
				{
					MapPictures *pMap = (MapPictures *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
					for(unsigned long i = 0; i<count; ++i)
					{
                        unsigned long size = Read_unsigned_long();
						char *pData = Read_block(size);
						if(NULL != pData)
							pMap->insert(std::make_pair(size, pData));
					}
					return true;  // We handled it
				}
				break;
		};
	}

	pDataBlock=m_pcDataBlock; iAllocatedSize=m_dwAllocatedSize; pCurrentPosition=m_pcCurrentPosition;
	return true;
}
//-----------------------------------------------------------------------------------------------------

