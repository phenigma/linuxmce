#ifdef VIPPHONE

#ifndef SYMBIAN

#include "stdafx.h"

#include "VIPShared/VIPIncludes.h"

#include "VIPShared/PlutoConfig.h"

#include "VIPDesign/VIPDesignDoc.h"

#endif

#endif



#include "VIPShared/VIPIncludes.h"

//#include "VIPShared/PlutoConfig.h"

#include "VR_ShowMenu.h"

#include "VIPShared/VIPMenu.h"



VR_ShowMenu::VR_ShowMenu(const char *VMCFile)

	: RA_Request()

{

	m_iMenuTermination = DONT_CLOSE;

	m_pMenuCollection=NULL;

	FILE *file = fopen(VMCFile,"rb");

	if( !file )

	{

#ifndef SYMBIAN
		cerr << "Cannot open " << VMCFile;

#endif
		return;

	}



	m_pMenuCollection = new VIPMenuCollection(0);  // ID will be in the file

	fseek(file, 0L, SEEK_END);

	m_pMenuCollection->m_iBinarySize=ftell(file);

	if( m_pMenuCollection->m_iBinarySize==0 )

	{

#ifndef SYMBIAN
		cerr << "Cannot open " << VMCFile;

#endif
		fclose(file);

		return;

	}

	fseek(file, 0L, SEEK_SET);

	m_pMenuCollection->m_pBinary=(char *)malloc(m_pMenuCollection->m_iBinarySize+1);

	size_t bytesread=fread(m_pMenuCollection->m_pBinary,1,m_pMenuCollection->m_iBinarySize,file);

	fclose(file);

}



VR_ShowMenu::VR_ShowMenu(class VIPMenuCollection *pMenuCollection)

	: RA_Request()

{

	// Request is just a binary representation of the collection

	m_pMenuCollection=pMenuCollection;

	m_iMenuTermination = DONT_CLOSE;

}





bool VR_ShowMenu::ProcessRequest(class RA_Processor *pRA_Processor)
{

#ifdef VIPPHONE

	if( !m_pMenuCollection || !m_pMenuCollection->m_pMenu_Starting )

	{

		return false;

	}

	class VIPMenuCollection *pMenuCollection_Old = g_pPlutoConfig->m_pDoc->m_pMenuCollection;

	class VIPMenu *pMenu_Old = g_pPlutoConfig->m_pDoc->m_pMenu;

	class VIPMenuInput *pInput_Old = g_pPlutoConfig->m_pDoc->m_pMenuInput;



	MYSTL_ITERATE_LONG_MAP(m_pMenuCollection->m_mapVariables,VIPVariable,pVariable,itVariables)

	{

		pVariable->m_sCurrentValue = pVariable->m_sInitialValue;

	}





	MYSTL_ITERATE_LIST(m_listInputVariables,VIPVariable,pInputVariable,it1)

	{

		if( pInputVariable )

		{

			VIPVariable *pVariable = m_pMenuCollection->m_mapVariables_Find(pInputVariable->m_iVariableID);

			// If the variable isn't in the menu collection, go ahead and add it

			if( !pVariable )

			{

				pInputVariable->m_iPhoneSetsValue = 1;

				m_pMenuCollection->AddVariable(pInputVariable);

			}

			else

				pVariable->m_sCurrentValue = pInputVariable->m_sCurrentValue;

		}

	}



	g_pPlutoConfig->m_bWaitingForTermination=true;

	g_pPlutoConfig->m_pDoc->m_pMenuCollection = m_pMenuCollection;

	g_pPlutoConfig->m_pDoc->SwitchToMenu(m_pMenuCollection->m_pMenu_Starting,false);

	SendMessage(g_pPlutoConfig->m_hWnd_Emulator,WM_SHOWWINDOW,0,0);

	SendMessage(g_pPlutoConfig->m_hWnd_Emulator,WM_SHOWWINDOW,1,0);

	SendMessage(g_pPlutoConfig->m_hWnd_Emulator,WM_SETFOCUS,1,0);



	while( g_pPlutoConfig->m_bWaitingForTermination )

		Sleep(500);



	// Restore the original values

	g_pPlutoConfig->m_pDoc->m_pMenuCollection = pMenuCollection_Old;

	g_pPlutoConfig->m_pDoc->m_pMenu = pMenu_Old;

	g_pPlutoConfig->m_pDoc->m_pMenuInput = pInput_Old;



	m_iMenuCollectionID = m_pMenuCollection->m_iMenuCollectionID;

	m_iMenuTermination = g_pPlutoConfig->m_iResolutionValue;



	MYSTL_ITERATE_LONG_MAP(m_pMenuCollection->m_mapVariables,VIPVariable,pVariable2,itVariables2)

	{

		if( pVariable2->m_iPhoneSetsValue )

			MYSTL_ADDTO_LIST(m_listReturnedVariables,pVariable2);

	}



#endif

	return true;

}


bool VR_ShowMenu::UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition )
{
	m_pcDataBlock = pcDataBlock; m_dwAllocatedSize = dwAllocatedSize; m_pcCurrentPosition = pcCurrentPosition;
	
	if( bWriting ) // writing
	{
		switch( pItem->m_iSerializeDataType )
		{
		case SERIALIZE_DATA_TYPE_LIST_VAR:
			{
				list<VIPVariable *> *p_listVIPVariable = (list<VIPVariable *> *) pItem->m_pItem;
				Write_unsigned_long( (unsigned long) p_listVIPVariable->size() );
				for( list<VIPVariable *>::iterator it = p_listVIPVariable->begin(); it != p_listVIPVariable->end(); ++it )
				{
					VIPVariable *pVIPVariable = *it;
					pVIPVariable->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				return true; // We handled it
			}
			break;
			
		case SERIALIZE_DATA_TYPE_LIST_BASKET:
			{
				list<BasketItem *> *p_listBasketItem = (list<BasketItem *> *) pItem->m_pItem;
				Write_unsigned_long( (unsigned long) p_listBasketItem->size() );
				for( list<BasketItem *>::iterator it = p_listBasketItem->begin(); it != p_listBasketItem->end(); ++it )
				{
					BasketItem *pBasketItem = *it;
					pBasketItem->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
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
		
		case SERIALIZE_DATA_TYPE_LIST_VAR:
			{
				list<VIPVariable *> *p_listVIPVariable = (list<VIPVariable *> *) pItem->m_pItem;
				long Count=Read_unsigned_long();
				for(long c=0;c<Count;++c)
				{
					VIPVariable *pVIPVariable = new VIPVariable();
					pVIPVariable->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					p_listVIPVariable->push_back(pVIPVariable);
				}
				return true; // We handled it
			}
			break;
			
		case SERIALIZE_DATA_TYPE_LIST_BASKET:
			{
				list<BasketItem *> *p_listBasketItem = (list<BasketItem *> *) pItem->m_pItem;
				long Count=Read_unsigned_long();
				for(long c=0;c<Count;++c)
				{
					BasketItem *pBasketItem = new BasketItem();
					pBasketItem->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					p_listBasketItem->push_back(pBasketItem);
				}
				return true; // We handled it
			}
			break;
			
		};
	}

	pcDataBlock = m_pcDataBlock; dwAllocatedSize = m_dwAllocatedSize; pcCurrentPosition = m_pcCurrentPosition;
	return true;
}
