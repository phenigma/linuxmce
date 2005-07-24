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
	m_Action_LoadList.clear();
	m_Action_UnloadList.clear();
	m_Action_TimeoutList.clear();
	m_Action_StartupList.clear();
	m_ZoneList.clear();
	m_ChildObjects.clear();	
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

bool DesignObj_Data::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )	
{
	// Save the starting points
	m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;

	if( bWriting )
	{
		Write_char(m_bChild);
		Write_char(m_bDontResetState);
		Write_char(m_bCantGoBack);
		Write_char(m_bCanGoBackToSameScreen);
		Write_char(m_bChildrenBeforeText);
		Write_char(m_bProcessActionsAtServer);
		Write_char(m_bAnimate);
		Write_char(m_bHideByDefault);

		Write_char(m_bTabStop);
		Write_char(m_bRepeatParm);

		Write_long(m_iPK_Button);
		Write_long(m_dwTimeoutSeconds);
		Write_long(m_Priority);
		Write_long(m_iPK_Criteria);

		Write_long(m_iBaseObjectID);
		Write_long(m_iVersion);
		Write_long(m_iPage);

		Write_long(m_ObjectType);

		Write_long(m_rPosition.X);
		Write_long(m_rPosition.Y);
		Write_long(m_rPosition.Width);
		Write_long(m_rPosition.Height);

		Write_long(m_rBackgroundPosition.X);
		Write_long(m_rBackgroundPosition.Y);
		Write_long(m_rBackgroundPosition.Width);
		Write_long(m_rBackgroundPosition.Height);

		Write_long(m_sOriginalSize.Width);
		Write_long(m_sOriginalSize.Height);

		Write_long(m_PK_DesignObj_Up);
		Write_long(m_PK_DesignObj_Down);
		Write_long(m_PK_DesignObj_Left);
		Write_long(m_PK_DesignObj_Right);

		Write_long(m_iRepeatIntervalInMS);
		Write_long(m_dwIDim);
		Write_long(m_iRegenInterval);

		Write_string(m_sBackgroundFile);
		Write_string(m_sSelectedFile);
		Write_string(m_sHighlightGraphicFilename);

		Write_string(m_ObjectID);
		Write_string(m_sPK_DesignObj_TiedTo);
		Write_string(m_sVisibleState);
		Write_block(m_dbHitTest.m_pBlock,m_dbHitTest.m_dwSize);

		
		{
			Write_long(m_vectAltGraphicFilename.size());
			for(size_t s=0;s<m_vectAltGraphicFilename.size();++s)
				Write_string(m_vectAltGraphicFilename[s]);
		}

		{
			Write_long(m_mapObjParms.size());
			for(map<int,string>::iterator it=m_mapObjParms.begin();it!=m_mapObjParms.end();++it)
			{
				Write_long(it->first);
				Write_string(it->second);
			}
		}
		{
			Write_long(m_Action_LoadList.size());
			for(DesignObjCommandList::iterator it=m_Action_LoadList.begin();it!=m_Action_LoadList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}

		{
			Write_long(m_Action_UnloadList.size());
			for(DesignObjCommandList::iterator it=m_Action_UnloadList.begin();it!=m_Action_UnloadList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			Write_long(m_Action_TimeoutList.size());
			for(DesignObjCommandList::iterator it=m_Action_TimeoutList.begin();it!=m_Action_TimeoutList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			Write_long(m_Action_StartupList.size());
			for(DesignObjCommandList::iterator it=m_Action_StartupList.begin();it!=m_Action_StartupList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			Write_long(m_ZoneList.size());
			for(DesignObjZoneList::iterator it=m_ZoneList.begin();it!=m_ZoneList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			Write_long(m_ChildObjects.size());
			for(DesignObj_DataList::iterator it=m_ChildObjects.begin();it!=m_ChildObjects.end();++it)
			{
				Write_unsigned_long( (*it)->m_ObjectType );
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
			}
		}

		{
			Write_long(m_vectDesignObjText.size());
			for(VectorDesignObjText::iterator it=m_vectDesignObjText.begin();it!=m_vectDesignObjText.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
	}
	else
	{
		m_bChild=Read_char();
		m_bDontResetState=Read_char();
		m_bCantGoBack=Read_char();
		m_bCanGoBackToSameScreen=Read_char();
		m_bChildrenBeforeText=Read_char();
		m_bProcessActionsAtServer=Read_char();
		m_bAnimate=Read_char();
		m_bHideByDefault=Read_char();

		m_bTabStop=Read_char();
		m_bRepeatParm=Read_char();

		m_iPK_Button=Read_long();
		m_dwTimeoutSeconds=Read_long();
		m_Priority=Read_long();
		m_iPK_Criteria=Read_long();

		m_iBaseObjectID=Read_long();
		m_iVersion=Read_long();
		m_iPage=Read_long();

		m_ObjectType=Read_long();

		m_rPosition.X=Read_long();
		m_rPosition.Y=Read_long();
		m_rPosition.Width=Read_long();
		m_rPosition.Height=Read_long();

		m_rBackgroundPosition.X=Read_long();
		m_rBackgroundPosition.Y=Read_long();
		m_rBackgroundPosition.Width=Read_long();
		m_rBackgroundPosition.Height=Read_long();

		m_sOriginalSize.Width=Read_long();
		m_sOriginalSize.Height=Read_long();

		m_PK_DesignObj_Up=Read_long();
		m_PK_DesignObj_Down=Read_long();
		m_PK_DesignObj_Left=Read_long();
		m_PK_DesignObj_Right=Read_long();

		m_iRepeatIntervalInMS=Read_long();
		m_dwIDim=Read_long();
		m_iRegenInterval=Read_long();

		Read_string(m_sBackgroundFile);
		Read_string(m_sSelectedFile);
		Read_string(m_sHighlightGraphicFilename);

		Read_string(m_ObjectID);
		Read_string(m_sPK_DesignObj_TiedTo);
		Read_string(m_sVisibleState);
		m_dbHitTest.m_pBlock=Read_block(m_dbHitTest.m_dwSize);

		{
			int count=Read_long();
			for(size_t s=0;s<count;++s)
			{
				string str;
				Read_string(str);
				m_vectAltGraphicFilename.push_back(str);
			}
		}

		{
			int count=Read_long();
			for(size_t s=0;s<count;++s)
			{
				long l = Read_long();
				string str;
				Read_string(str);
				m_mapObjParms[l]=str;
			}
		}

		{
			int count=Read_long();
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_LoadList.push_back(pDesignObjCommand);
			}
		}

		{
			int count=Read_long();
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_UnloadList.push_back(pDesignObjCommand);
			}
		}

		{
			int count=Read_long();
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_TimeoutList.push_back(pDesignObjCommand);
			}
		}

		{
			int count=Read_long();
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_StartupList.push_back(pDesignObjCommand);
			}
		}

		{
			int count=Read_long();
			for(size_t s=0;s<count;++s)
			{
				DesignObjZone *pDesignObjZone = new DesignObjZone();
				pDesignObjZone->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_ZoneList.push_back(pDesignObjZone);
			}
		}

		{
			int count=Read_long();
			for(size_t s=0;s<count;++s)
			{
				unsigned long Type = Read_unsigned_long();
	#ifdef ORBITER
				DesignObj_Orbiter *pDesignObj_Data=NULL;
				if( Type==DESIGNOBJTYPE_Datagrid_CONST )
					pDesignObj_Data = new DesignObj_DataGrid((class Orbiter *) m_pExtraSerializationData);
				else
					pDesignObj_Data = new DesignObj_Orbiter((class Orbiter *) m_pExtraSerializationData);
	#else
				DesignObj_Data *pDesignObj_Data = new DesignObj_Data();
	#endif

				pDesignObj_Data->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_ChildObjects.push_back(pDesignObj_Data);
			}
		}

		{
			int count=Read_long();
			for(size_t s=0;s<count;++s)
			{
				DesignObjText *pDesignObjText = new DesignObjText();
				pDesignObjText->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_vectDesignObjText.push_back(pDesignObjText);
			}
		}
	}
	// We may have re-allocated the memory block and size, and the position will have changed
	pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	return true;
}