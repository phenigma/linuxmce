/**
 *
 * @file DeviceData_Impl.cpp
 * @brief source file for the DeviceData_Impl class
 * @author
 * @todo notcommented
 *
 */

 /**
  *
  * Copyright Notice goes here
  *
  */


#include "DeviceData_Impl.h"

using namespace DCE;

bool DeviceData_Impl::UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition )
{
	m_pcDataBlock = pcDataBlock; m_dwAllocatedSize = dwAllocatedSize; m_pcCurrentPosition = pcCurrentPosition;
	if( bWriting ) // writing
	{
		switch( pItem->m_iSerializeDataType )
		{
		case SERIALIZE_DATA_TYPE_VECTOR_DEVICEDATA_IMPL:
			{
				VectDeviceData_Impl *pVect = (VectDeviceData_Impl *) pItem->m_pItem;
				Write_unsigned_long( (unsigned long) pVect->size() );
				for( size_t s=0; s < pVect->size(); ++s )
				{
					DeviceData_Impl *pDeviceData_Impl = (*pVect)[s];
					pDeviceData_Impl->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
				}
				return true;  // We handled it
			}
			break;
		};
	}
	else // reading
	{
		switch( pItem->m_iSerializeDataType )
		{
		case SERIALIZE_DATA_TYPE_VECTOR_DEVICEDATA_IMPL:
			{
				VectDeviceData_Impl *pVect = (VectDeviceData_Impl *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for( unsigned long i=0; i < count; ++i )
				{
					DeviceData_Impl *pDeviceData_Impl = new DeviceData_Impl();
					pDeviceData_Impl->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
					pVect->push_back( pDeviceData_Impl );
				}
				return true;  // We handled it
			}
			break;
		};
	}

	pcDataBlock = m_pcDataBlock; dwAllocatedSize = m_dwAllocatedSize; pcCurrentPosition = m_pcCurrentPosition;
	return true;
}
