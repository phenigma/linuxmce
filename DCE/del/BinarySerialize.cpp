#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/ShapesColors.h"
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
