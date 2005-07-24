/*
 DesignObj_Data
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */
 
#include "PlutoUtils/CommonIncludes.h"	
#include "DesignObj_Orbiter.h"
#include "pluto_main/Define_DesignObjType.h"
#include "DCE/Logger.h"

DesignObj_Data::~DesignObj_Data()
{
	m_vectAltGraphicFilename.clear();
	m_mapObjParms.clear();
	m_Action_LoadList.clear();
	m_Action_UnloadList.clear();
	m_Action_TimeoutList.clear();
	m_Action_StartupList.clear();
	m_ZoneList.clear();
	m_ChildObjects.clear();	
	m_vectDesignObjText.clear();
}

bool DesignObjCommand::Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData)
{
	if( bWriting )
	{
		m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;
		Write_block( (char *)&Data,sizeof(Data));
		pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	}
	else
	{
g_pPlutoLogger->Write(LV_STATUS,"start to deserialize command");
		memcpy((void *)&Data,pcCurrentPosition,sizeof(Data));
		pcCurrentPosition += sizeof(Data);
g_pPlutoLogger->Write(LV_STATUS,"done command %d",Data.m_PK_Command);
	}
	return SerializeClass::Serialize(bWriting,pcDataBlock,dwAllocatedSize,pcCurrentPosition,pExtraSerializationData);
}

bool DesignObjZone::UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition) 
{
	m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
	if( bWriting )
	{
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST:
			{
				DesignObjCommandList *pList = (DesignObjCommandList *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pList->size());
				for(DesignObjCommandList::iterator it=pList->begin();it!=pList->end();++it)
				{
					(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
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
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST:
			{
				DesignObjCommandList *pList = (DesignObjCommandList *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					DesignObjCommand *pAction = new DesignObjCommand();
					pAction->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					pList->push_back(pAction);
				}
				return true;  // We handled it
			}
			break;
		};
	}

	pDataBlock=m_pcDataBlock; iAllocatedSize=m_dwAllocatedSize; pCurrentPosition=m_pcCurrentPosition;
	return true;
} 

bool DesignObjZone::Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData)
{
	if( bWriting )
	{
		m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;
		Write_block( (char *)&m_Rect,sizeof(PlutoRectangle));
		pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	}
	else
	{
g_pPlutoLogger->Write(LV_STATUS,"start to deserialize zone");
		memcpy((void *)&m_Rect,pcCurrentPosition,sizeof(PlutoRectangle));
		pcCurrentPosition += sizeof(PlutoRectangle);
g_pPlutoLogger->Write(LV_STATUS,"done zone %d",m_Rect.X);
	}
	return SerializeClass::Serialize(bWriting,pcDataBlock,dwAllocatedSize,pcCurrentPosition,pExtraSerializationData);
}

bool DesignObjText::UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition)
{
	m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
	bool bHandledIt=false;  // We must return false if we don't handle this
	if( bWriting )
	{
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_INT_STYLEMAP:
			{
				MapTextStyle *pMapTextStyle = (MapTextStyle *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pMapTextStyle->size());
				for(MapTextStyle::iterator it=pMapTextStyle->begin();it!=pMapTextStyle->end();++it)
				{
					TextStyle *pTextStyle = (*it).second;
					pTextStyle->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				bHandledIt=true;  // We handled it
			}
			break;
		};
	}
	else
	{
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_INT_STYLEMAP:
			{
				MapTextStyle *pMapTextStyle = (MapTextStyle *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					TextStyle *pTextStyle = new TextStyle();
					pTextStyle->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					(*pMapTextStyle)[pTextStyle->m_iVersion]=pTextStyle;
				}
				bHandledIt=true;  // We handled it
			}
			break;
		};
	}

	pDataBlock=m_pcDataBlock; iAllocatedSize=m_dwAllocatedSize; pCurrentPosition=m_pcCurrentPosition;
	return bHandledIt;
}

bool DesignObjText::Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData)
{
	if( bWriting )
	{
		m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;
		Write_block( (char *)&Data,sizeof(Data));
		pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	}
	else
	{
g_pPlutoLogger->Write(LV_STATUS,"start ext");
		memcpy((void *)&Data,pcCurrentPosition,sizeof(Data));
		pcCurrentPosition += sizeof(Data);
g_pPlutoLogger->Write(LV_STATUS,"done text %d",Data.m_PK_Text);
	}
	return SerializeClass::Serialize(bWriting,pcDataBlock,dwAllocatedSize,pcCurrentPosition,pExtraSerializationData);
}

bool DesignObj_Data::UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition) 
{ 
	m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
	bool bHandledIt=false;  // We must return false if we don't handle this

	if( bWriting )
	{
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST:
			{
				DesignObjCommandList *pList = (DesignObjCommandList *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pList->size());
				for(DesignObjCommandList::iterator it=pList->begin();it!=pList->end();++it)
				{
					(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				bHandledIt=true;  // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJZONELIST:
			{
				DesignObjZoneList *pList = (DesignObjZoneList *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pList->size());
				for(DesignObjZoneList::iterator it=pList->begin();it!=pList->end();++it)
				{
					(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				bHandledIt=true;  // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJCHILDLIST:
			{
				DesignObj_DataList *pList = (DesignObj_DataList *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pList->size());
				for(DesignObj_DataList::iterator it=pList->begin();it!=pList->end();++it)
				{
					DesignObj_Data *pObj = *it;
					// Different types of objects require different derived classes, so save this information first
					Write_unsigned_long(pObj->Data.m_ObjectType);
					pObj->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				bHandledIt=true;  // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_VECT_DESIGNOBJTEXT:
			{
				VectorDesignObjText *pVect = (VectorDesignObjText *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pVect->size());
				for(size_t s=0;s<pVect->size();++s)
				{
					DesignObjText *pDesignObjText = (*pVect)[s];
					// Different types of objects require different derived classes, so save this information first
					pDesignObjText->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				bHandledIt=true;  // We handled it
			}
			break;

		};
	}
	else
	{
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST:
			{
				DesignObjCommandList *pList = (DesignObjCommandList *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					DesignObjCommand *pAction = new DesignObjCommand();
					pAction->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					pList->push_back(pAction);
				}
				bHandledIt=true;  // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJZONELIST:
			{
				DesignObjZoneList *pList = (DesignObjZoneList *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					DesignObjZone *pZone = new DesignObjZone();
					pZone->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					pList->push_back(pZone);
				}
				bHandledIt=true;  // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_INT_DESIGNOBJCHILDLIST:
			{
				DesignObj_DataList *pList = (DesignObj_DataList *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					unsigned long Type = Read_unsigned_long();
#ifdef ORBITER
					DesignObj_Data *pChild=NULL;
					if( Type==DESIGNOBJTYPE_Datagrid_CONST )
						pChild = new DesignObj_DataGrid((class Orbiter *) m_pExtraSerializationData);
					else
						pChild = new DesignObj_Orbiter((class Orbiter *) m_pExtraSerializationData);

#else
					DesignObj_Data *pChild = new DesignObj_Data();
#endif
					pChild->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,m_pExtraSerializationData);
					pChild->m_pParentObject=this;
					pList->push_back(pChild);
				}
				bHandledIt=true;  // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_VECT_DESIGNOBJTEXT:
			{
				VectorDesignObjText *pVect = (VectorDesignObjText *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					DesignObjText *pDesignObjText = new DesignObjText();
					// Different types of objects require different derived classes, so save this information first
					pDesignObjText->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					pVect->push_back(pDesignObjText);
				}
				bHandledIt=true;  // We handled it
			}
			break;

		};
	}

	pDataBlock=m_pcDataBlock; iAllocatedSize=m_dwAllocatedSize; pCurrentPosition=m_pcCurrentPosition;
	return bHandledIt;
} 

bool DesignObj_Data::Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData)
{
	if( bWriting )
	{
		m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;
		Write_block( (char *)&Data,sizeof(Data));
		pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	}
	else
	{
g_pPlutoLogger->Write(LV_STATUS,"start object");
		memcpy((void *)&Data,pcCurrentPosition,sizeof(Data));
		pcCurrentPosition += sizeof(Data);
g_pPlutoLogger->Write(LV_STATUS,"done object %d",Data.m_iBaseObjectID);
	}
	return SerializeClass::Serialize(bWriting,pcDataBlock,dwAllocatedSize,pcCurrentPosition,pExtraSerializationData);
}
