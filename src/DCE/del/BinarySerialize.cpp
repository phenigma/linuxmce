/* 
	BinarySerialize
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"	
#include "SerializeClass/ShapesColors.h"
#endif

#include "SerializeClass/SerializeClass.h"


bool SerializeClass::SelfSerializing(bool bWriting,ItemToSerialize *pItem)
{
#ifndef SYMBIAN
	switch(pItem->m_iSerializeDataType)
	{
	case SERIALIZE_DATA_TYPE_COLOR:
		{
			PlutoColor *pColor = (PlutoColor *) pItem->m_pItem;
			pColor->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		}
		return true;
	case SERIALIZE_DATA_TYPE_POINT:
		{
			PlutoPoint *pPoint = (PlutoPoint *) pItem->m_pItem;
			pPoint->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		}
		return true;
	case SERIALIZE_DATA_TYPE_SIZE:
		{
			PlutoSize *pSize = (PlutoSize *) pItem->m_pItem;
			pSize->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		}
		return true;
	case SERIALIZE_DATA_TYPE_RECTANGLE:
		{
			PlutoRectangle *pRectangle = (PlutoRectangle *) pItem->m_pItem;
			pRectangle->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		}
		return true;
	};
#endif

	return false;
}
