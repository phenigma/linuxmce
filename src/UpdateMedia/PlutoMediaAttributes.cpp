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
void PlutoMediaAttribute::SetupSerialization(int iSC_Version)
{
	StartSerializeList() + m_nType + m_sName + m_nTrack + m_nSection;
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaAttributes::SerializeClassClassName() 
{ 
	return "PlutoMediaAttributes"; 
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
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes::~PlutoMediaAttributes()
{
	for(MapPlutoMediaAttributes::iterator it = m_mapAttributes.begin(), end = m_mapAttributes.end(); it != end; ++it)
		delete it->second;
}
//-----------------------------------------------------------------------------------------------------
bool PlutoMediaAttributes::OkayToDeserialize(int iSC_Version)
{
	return SERIALIZE_PLUTO_MEDIA_ATTRIBUTES_VERSION == iSC_Version;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaAttributes::SetupSerialization(int iSC_Version)
{
	StartSerializeList() + m_nInstallationID + m_nFileID + m_nPictureID + m_sPictureUrl;
	
	(*this) + m_mapAttributes;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaAttributes &PlutoMediaAttributes::operator+ (MapPlutoMediaAttributes &i) 
{ 
	m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MAP_PLUTOMEDIAATTRBUTE,(void *) &i)); 
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
		};
	}

	pDataBlock=m_pcDataBlock; iAllocatedSize=m_dwAllocatedSize; pCurrentPosition=m_pcCurrentPosition;
	return true;
}
//-----------------------------------------------------------------------------------------------------

