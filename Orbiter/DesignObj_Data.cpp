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

DesignObj_Data::~DesignObj_Data()
{
	m_vectAltGraphicFilename.clear();
	m_mapObjParms.clear();

	DesignObjCommandList::iterator itCommands;

	for(itCommands=m_Action_LoadList.begin();itCommands!=m_Action_LoadList.end();++itCommands)
	{
		DesignObjCommand *pCommand = *itCommands;
		delete pCommand;
	}
	m_Action_LoadList.clear();

	for(itCommands=m_Action_UnloadList.begin();itCommands!=m_Action_UnloadList.end();++itCommands)
	{
		DesignObjCommand *pCommand = *itCommands;
		delete pCommand;
	}
	m_Action_UnloadList.clear();

	for(itCommands=m_Action_TimeoutList.begin();itCommands!=m_Action_TimeoutList.end();++itCommands)
	{
		DesignObjCommand *pCommand = *itCommands;
		delete pCommand;
	}
	m_Action_TimeoutList.clear();

	for(itCommands=m_Action_StartupList.begin();itCommands!=m_Action_StartupList.end();++itCommands)
	{
		DesignObjCommand *pCommand = *itCommands;
		delete pCommand;
	}
	m_Action_StartupList.clear();

	DesignObjZoneList::iterator itZones;
	for(itZones=m_ZoneList.begin();itZones!=m_ZoneList.end();++itZones)
	{
		DesignObjZone *pZone = *itZones;
		delete pZone;
	}
	m_ZoneList.clear();

	DesignObj_DataList::iterator itChild;
	for(itChild=m_ChildObjects.begin();itChild!=m_ChildObjects.end();++itChild)
	{
		DesignObj_Data *pObj = *itChild;
		delete pObj;
	}
	m_ChildObjects.clear();	

	for(size_t s=0;s<m_vectDesignObjText.size();++s)
		delete m_vectDesignObjText[s];
	m_vectDesignObjText.clear();
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
					Write_unsigned_long(pObj->m_ObjectType);
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
