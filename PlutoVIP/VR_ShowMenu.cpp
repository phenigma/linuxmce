#ifdef VIPPHONE

#ifndef SYMBIAN

#include "stdafx.h"

#include "VIPIncludes.h"

#include "VIPShared/PlutoConfig.h"

#include "VIPDesign/VIPDesignDoc.h"

#endif

#endif



#include "VIPIncludes.h"

#include "VIPShared/PlutoConfig.h"

#include "VR_ShowMenu.h"

#include "VIPMenu.h"



VR_ShowMenu::VR_ShowMenu(const char *VMCFile)

	: RA_Request()

{

	m_iMenuTermination = DONT_CLOSE;

	m_pMenuCollection=NULL;

	FILE *file = fopen(VMCFile,"rb");

	if( !file )

	{

#ifndef SYMBIAN
		ErrorLog << "Cannot open " << VMCFile; }

#endif
		return;

	}



	m_pMenuCollection = new VIPMenuCollection(0);  // ID will be in the file

	fseek(file, 0L, SEEK_END);

	m_pMenuCollection->m_iBinarySize=ftell(file);

	if( m_pMenuCollection->m_iBinarySize==0 )

	{

#ifndef SYMBIAN
		ErrorLog << "Cannot open " << VMCFile; }

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



VR_ShowMenu::VR_ShowMenu(unsigned long size,const char *data) 

	: RA_Request(size,data) 

{

	m_iMenuTermination = DONT_CLOSE;



	unsigned long CollectionSize = Read_unsigned_long();

	if( CollectionSize==0 )

		m_pMenuCollection = new VIPMenuCollection(0);

	else

		m_pMenuCollection = new VIPMenuCollection(CollectionSize,m_pcCurrentPosition);

	m_pcCurrentPosition+=CollectionSize;

	unsigned short NumberOfVariables = Read_unsigned_short();

	for(unsigned short iV=0;iV<NumberOfVariables;++iV)

	{

		long VariableID = Read_long();

		string sInitialValue;

		Read_string(sInitialValue);



		VIPVariable *pVariable = new VIPVariable(VariableID,0,sInitialValue,0,0);

		MYSTL_ADDTO_LIST(m_listInputVariables,pVariable);

	}



	unsigned long NumberOfBasketItems = Read_unsigned_long();

	for(size_t iB=0;iB<NumberOfBasketItems;++iB)

	{

		BasketItem *bi = new BasketItem();

		Read_string(bi->m_sID);

		Read_string(bi->m_sDescription);

		bi->m_iQty = Read_long();

		bi->m_iCost = Read_long();

		unsigned long NumberOfBasketOptions = Read_unsigned_long();

		for(size_t i2=0;i2<NumberOfBasketOptions;++i2)

		{

			BasketOption *bo = new BasketOption();

			Read_string(bo->m_sID);

			Read_string(bo->m_sDescription);

			bo->m_iQty = Read_long();

			bo->m_iCost = Read_long();

			MYSTL_ADDTO_LIST(bi->m_listBasketOptions,bo);

		}

		MYSTL_ADDTO_LIST(m_listBasket,bi);

	}



	m_dwRequestSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);

}



bool VR_ShowMenu::ProcessRequest()

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



void VR_ShowMenu::ConvertRequestToBinary()

{

	RA_Request::ConvertRequestToBinary();



	if( !m_pMenuCollection )

	{

		Write_unsigned_long(0);  // no menu or variables

		Write_unsigned_short(0);

	}

	else

	{

		if( !m_pMenuCollection->m_iBinarySize )

			m_pMenuCollection->ConvertToBinary();



		Write_unsigned_long(m_pMenuCollection->m_iBinarySize);

		Write_block(m_pMenuCollection->m_pBinary,m_pMenuCollection->m_iBinarySize);

		
		Write_unsigned_short((unsigned short) MYSTL_SIZEOF_LIST(m_listInputVariables));
		//Write_unsigned_short((unsigned short) m_listInputVariables.size());



		MYSTL_ITERATE_LIST(m_listInputVariables,VIPVariable,pVariable,it2)

		{

			Write_long(pVariable->m_iVariableID);

			Write_string(pVariable->m_sInitialValue);

		}

	}

	

	;
	Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LIST(m_listBasket));
//	Write_unsigned_long((unsigned long) m_listBasket.size());



	MYSTL_ITERATE_LIST(m_listBasket,BasketItem,bi,it3)

	{

		Write_string(bi->m_sID);

		Write_string(bi->m_sDescription);

		Write_long(bi->m_iQty);

		Write_long(bi->m_iCost);



		Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LIST(bi->m_listBasketOptions));
//		Write_unsigned_long((unsigned long) bi->m_listBasketOptions.size());



		MYSTL_ITERATE_LIST(bi->m_listBasketOptions,BasketOption,bo,it4)

		{

			Write_string(bo->m_sID);

			Write_string(bo->m_sDescription);

			Write_long(bo->m_iQty);

			Write_long(bo->m_iCost);

		}

	}



	m_dwRequestSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);

	m_pcRequest = m_pcDataBlock;

}



void VR_ShowMenu::ConvertResponseToBinary()

{



	RA_Request::ConvertResponseToBinary();



	Write_unsigned_char(m_iMenuTermination);

	Write_unsigned_long(m_iMenuCollectionID);

	Write_unsigned_int64(m_iMacAddress);



	Write_unsigned_short((unsigned short) MYSTL_SIZEOF_LIST(m_listReturnedVariables));
	//Write_unsigned_short((unsigned short)m_listReturnedVariables.size());



	MYSTL_ITERATE_LIST(m_listReturnedVariables,VIPVariable,pVariable,it5)

	{

		Write_long(pVariable->m_iVariableID);

		Write_string(pVariable->m_sCurrentValue);

	}



	Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LIST(m_listBasket));
//	Write_unsigned_long((unsigned long) m_listBasket.size());



	MYSTL_ITERATE_LIST(m_listBasket,BasketItem,bi,it6)

	{

		Write_string(bi->m_sID);

		Write_string(bi->m_sDescription);

		Write_long(bi->m_iQty);

		Write_long(bi->m_iCost);



		Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LIST(bi->m_listBasketOptions));
		//Write_unsigned_long((unsigned long) bi->m_listBasketOptions.size());

		MYSTL_ITERATE_LIST(bi->m_listBasketOptions,BasketOption,bo,it7)

		{

			Write_string(bo->m_sID);

			Write_string(bo->m_sDescription);

			Write_long(bo->m_iQty);

			Write_long(bo->m_iCost);

		}

	}

	m_dwResponseSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);

	m_pcResponse = m_pcDataBlock;

}



bool VR_ShowMenu::ParseResponse(unsigned long size,const char *data)

{

	RA_Request::ParseResponse(size,data);



	m_iMenuTermination = Read_unsigned_char();

	m_iMenuCollectionID = Read_unsigned_long();

	m_iMacAddress = Read_unsigned_int64();

	unsigned short NumVariables = Read_unsigned_short();



	for(unsigned short iV=0;iV<NumVariables;++iV)

	{

		long VariableID = Read_long();

		string sValue;

		Read_string(sValue);



		VIPVariable *pVariable = new VIPVariable(VariableID,1,sValue,0,0);

		MYSTL_ADDTO_LIST(m_listReturnedVariables,pVariable);

	}



	// Remove any basket items that may already be in there from the input side

	
	Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LIST(m_listBasket));
//	Write_unsigned_long((unsigned long) m_listBasket.size());



	MYSTL_ITERATE_LIST(m_listBasket,BasketItem,pBI,it8)

	{

		delete pBI;

	}

	MYSTL_CLEAR_LIST(m_listBasket);



	unsigned long NumberOfBasketItems = Read_unsigned_long();

	for(size_t iB=0;iB<NumberOfBasketItems;++iB)

	{

		BasketItem *bi = new BasketItem();

		Read_string(bi->m_sID);

		Read_string(bi->m_sDescription);

		bi->m_iQty = Read_long();

		bi->m_iCost = Read_long();

		unsigned long NumberOfBasketOptions = Read_unsigned_long();

		for(size_t i2=0;i2<NumberOfBasketOptions;++i2)

		{

			BasketOption *bo = new BasketOption();

			Read_string(bo->m_sID);

			Read_string(bo->m_sDescription);

			bo->m_iQty = Read_long();

			bo->m_iCost = Read_long();

			MYSTL_ADDTO_LIST(bi->m_listBasketOptions,bo);

		}

		MYSTL_ADDTO_LIST(m_listBasket,bi);

	}

	m_dwResponseSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);



	return true;

}

