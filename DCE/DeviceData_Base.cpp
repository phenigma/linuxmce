/* 
	DeviceData_Base
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

/**
 *
 * @file DeviceData_Base.cpp
 * @brief source file for the AllDevices class
 * @author
 * @todo notcommented
 *
 */


#include "PlutoUtils/CommonIncludes.h"	
#include "DeviceData_Base.h"

using namespace DCE;

bool AllDevices::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )
{
	bool bResult = SerializeClass::Serialize( bWriting, pcDataBlock, dwAllocatedSize, pcCurrentPosition, pExtraSerializationData );

	if( !bWriting )
	{
		// Map them up to the parents
		Map_DeviceCategory::iterator itCat;
		for( itCat = m_mapDeviceCategory.begin(); itCat != m_mapDeviceCategory.end(); ++itCat )
		{
			DeviceCategory *pCat = (*itCat).second;
			if( pCat->m_dwPK_DeviceCategory_Parent ) // must make this ceck, not all cateories have a parent
			{
				DeviceCategory *pParent = m_mapDeviceCategory_Find( pCat->m_dwPK_DeviceCategory_Parent );
				pCat->m_pDeviceCategory_Parent = pParent;
			}
		}

		// Map them up to the category
		Map_DeviceData_Base::iterator itDevice;
		for( itDevice = m_mapDeviceData_Base.begin(); itDevice != m_mapDeviceData_Base.end(); itDevice++ )
		{
			DeviceData_Base *pDeviceData_Base = (*itDevice).second;
			pDeviceData_Base->m_pDeviceCategory = m_mapDeviceCategory_Find( pDeviceData_Base->m_dwPK_DeviceCategory );
			if( pDeviceData_Base->m_dwPK_Device_ControlledVia )
				pDeviceData_Base->m_pDevice_ControlledVia = m_mapDeviceData_Base_Find(pDeviceData_Base->m_dwPK_Device_ControlledVia);
		}

		Map_DeviceGroup::iterator itGroup;
		for( itGroup = m_mapDeviceGroup.begin(); itGroup != m_mapDeviceGroup.end(); ++itGroup )
		{
			DeviceGroup *pDeviceGroup = (*itGroup).second;
			for(size_t s=0;s<pDeviceGroup->m_vectPK_Device.size();++s)
			{
				DeviceData_Base *pDeviceData_Base = m_mapDeviceData_Base_Find(pDeviceGroup->m_vectPK_Device[s]);
				pDeviceGroup->m_vectDeviceData_Base.push_back(pDeviceData_Base);
				pDeviceData_Base->m_vectDeviceGroup.push_back(pDeviceGroup);
			}
		}
	}
	
	return bResult;
}

bool AllDevices::UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition )
{
	m_pcDataBlock = pcDataBlock; m_dwAllocatedSize = dwAllocatedSize; m_pcCurrentPosition = pcCurrentPosition;
	
	if( bWriting ) // writing
	{
		switch( pItem->m_iSerializeDataType )
		{
		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICE_GROUP:
			{
				Map_DeviceGroup *pMap = (Map_DeviceGroup *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pMap->size());
				for( Map_DeviceGroup::iterator it = pMap->begin(); it != pMap->end(); ++it )
				{
					DeviceGroup *pDeviceGroup = (*it).second; // we know how to serialize this
					pDeviceGroup->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
				}
				return true; // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE:
			{
				Map_DeviceData_Base *pMap = (Map_DeviceData_Base *) pItem->m_pItem;
				Write_unsigned_long( (unsigned long) pMap->size() );
				for( Map_DeviceData_Base::iterator it = pMap->begin(); it != pMap->end(); ++it )
				{
					DeviceData_Base *pDeviceData_Base = (*it).second; // we know how to serialize this
					pDeviceData_Base->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
				}
				return true; // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_MAP_DCECATEGORY:
			{
				Map_DeviceCategory *pMap = (Map_DeviceCategory *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pMap->size());
				for( Map_DeviceCategory::iterator it = pMap->begin(); it != pMap->end(); ++it )
				{
					DeviceCategory *pDeviceCategory = (*it).second; // we know how to serialize this
					pDeviceCategory->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
				}
				return true; // We handled it
			}
			break;
			
		};
	}
	else // reading
	{
		switch( pItem->m_iSerializeDataType )
		{
		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICE_GROUP:
			{
				Map_DeviceGroup *pMap = (Map_DeviceGroup *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for( unsigned long i = 0; i < count; ++i )
				{
					DeviceGroup *pDeviceGroup = new DeviceGroup();
					pDeviceGroup->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
					(*pMap)[pDeviceGroup->m_dwPK_DeviceGroup] = pDeviceGroup;
				}
				return true;  // We handled it
			}
			break;
		
		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE:
			{
				Map_DeviceData_Base *pMap = (Map_DeviceData_Base *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for( unsigned long i = 0; i < count; ++i )
				{
					DeviceData_Base *pDeviceData_Base = new DeviceData_Base();
					pDeviceData_Base->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
					(*pMap)[pDeviceData_Base->m_dwPK_Device] = pDeviceData_Base;
				}
				return true;  // We handled it
			}
			break;
			
		case SERIALIZE_DATA_TYPE_MAP_DCECATEGORY:
			{
				Map_DeviceCategory *pMap = (Map_DeviceCategory *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for( unsigned long i = 0; i < count; ++i )
				{
					DeviceCategory *pDeviceCategory = new DeviceCategory();
					pDeviceCategory->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
					(*pMap)[pDeviceCategory->m_dwPK_DeviceCategory] = pDeviceCategory;
				}
				return true;  // We handled it
			}
			break;
			
		};
	}

	pcDataBlock = m_pcDataBlock; dwAllocatedSize = m_dwAllocatedSize; pcCurrentPosition = m_pcCurrentPosition;
	return true;
}

DeviceData_Base *AllDevices::m_mapDeviceData_Base_FindFirstOfCategory( unsigned long dwPK_DeviceCategory ) 
{
	for(map<int,class DeviceData_Base *>::iterator it = m_mapDeviceData_Base.begin();it != m_mapDeviceData_Base.end();++it)
		if( (*it).second->m_dwPK_DeviceCategory==dwPK_DeviceCategory )
			return (*it).second;
	return NULL;
}
