#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "stdafx.h"
#include "VIPDesign/VIPDesignDoc.h"
#endif
#else
#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#endif
#endif

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoBTAppUi.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "VIPShared/VIPMenu.h"
#include "BD_CP_ShowVMC.h"
#include "BD/BDCommandProcessor.h"

BD_CP_ShowVMC::BD_CP_ShowVMC(unsigned char Store,unsigned long VMCSize,const char *pVMC) 
	
{
	m_iVMCSize=VMCSize;
	m_pVMC=(char *)malloc(m_iVMCSize);
	memcpy(m_pVMC,pVMC,m_iVMCSize);
}

void BD_CP_ShowVMC::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_iStore);
	Write_long(m_iVMCSize);
	Write_block(m_pVMC,m_iVMCSize);
}

void BD_CP_ShowVMC::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size, data);
	m_iStore = Read_unsigned_char();
#ifdef VIPPHONE

#ifdef SYMBIAN
	long VMCSize = Read_long();
	VIPMenuCollection *pVMC = new VIPMenuCollection(VMCSize, m_pcCurrentPosition);
	((CPlutoBTAppUi *)CCoeEnv::Static()->AppUi())->UpdateScreen(pVMC);
	((CPlutoBTAppUi *)CCoeEnv::Static()->AppUi())->Show();
#endif

#ifdef VIPDESIGN
	long VMCSize = Read_long();
	VIPMenuCollection *pVMC = new VIPMenuCollection(VMCSize, m_pcCurrentPosition);
	g_pPlutoConfig->m_pDoc->m_pMenuCollection=pVMC;
	g_pPlutoConfig->m_pDoc->m_pMenu=pVMC->m_pMenu_Starting;
	g_pPlutoConfig->m_pDoc->InvalidateAllViews();
#endif

#endif
}

bool BD_CP_ShowVMC::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
