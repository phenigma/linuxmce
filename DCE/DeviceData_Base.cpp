#include "PlutoUtils/CommonIncludes.h"	
#include "DeviceData_Base.h"

using namespace DCE;

bool AllDevices::Serialize(bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition,void *pExtraSerializationData)
{
	bool bResult = SerializeClass::Serialize(bWriting,pDataBlock,iAllocatedSize,pCurrentPosition,pExtraSerializationData);

	if( !bWriting )
	{
		// Map them up to the parents
		Map_DeviceCategory::iterator itCat;
		for(itCat=m_mapDeviceCategory.begin();itCat!=m_mapDeviceCategory.end();++itCat)
		{
			DeviceCategory *pCat = (*itCat).second;
			if( pCat->m_iPK_DeviceCategory_Parent )
			{
				DeviceCategory *pParent = m_mapDeviceCategory_Find(pCat->m_iPK_DeviceCategory_Parent);
				pCat->m_pDeviceCategory_Parent = pParent;
			}
		}

		Map_DeviceData_Base::iterator itDevice;
		for(itDevice=m_mapDeviceData_Base.begin();itDevice!=m_mapDeviceData_Base.end();itDevice++)
		{
			DeviceData_Base *pDeviceData_Base = (*itDevice).second;
			pDeviceData_Base->m_pDeviceCategory = m_mapDeviceCategory_Find(pDeviceData_Base->m_iPK_DeviceCategory);
		}
	}
	return bResult;
}

bool AllDevices::UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition)
{
	m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
	if( bWriting )
	{
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE:
			{
				Map_DeviceData_Base *pMap = (Map_DeviceData_Base *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pMap->size());
				for(Map_DeviceData_Base::iterator it=pMap->begin();it!=pMap->end();++it)
				{
					DeviceData_Base *pDeviceData_Base = (*it).second;
					pDeviceData_Base->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				return true;  // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_MAP_DCECATEGORY:
			{
				Map_DeviceCategory *pMap = (Map_DeviceCategory *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pMap->size());
				for(Map_DeviceCategory::iterator it=pMap->begin();it!=pMap->end();++it)
				{
					DeviceCategory *pDeviceCategory = (*it).second;
					pDeviceCategory->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
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
		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE:
			{
				Map_DeviceData_Base *pMap = (Map_DeviceData_Base *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					DeviceData_Base *pDeviceData_Base = new DeviceData_Base();
					pDeviceData_Base->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					(*pMap)[pDeviceData_Base->m_iPK_Device]=pDeviceData_Base;
				}
				return true;  // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_MAP_DCECATEGORY:
			{
				Map_DeviceCategory *pMap = (Map_DeviceCategory *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					DeviceCategory *pDeviceCategory = new DeviceCategory();
					pDeviceCategory->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					(*pMap)[pDeviceCategory->m_iPK_DeviceCategory]=pDeviceCategory;
				}
				return true;  // We handled it
			}
			break;
		};
	}

	pDataBlock=m_pcDataBlock; iAllocatedSize=m_dwAllocatedSize; pCurrentPosition=m_pcCurrentPosition;
	return true;
}
