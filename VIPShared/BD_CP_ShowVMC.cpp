#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "stdafx.h"
#include "VIPDesign/VIPDesignDoc.h"
#include "VIPShared/PlutoConfig.h"
#endif
#else
#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#endif
#endif

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "VIPShared/VIPMenu.h"
#include "BD_CP_ShowVMC.h"

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

	LOG("#	Received 'ShowVMC' command  #\n");
	VIPMenuCollection *pVMC = new VIPMenuCollection(VMCSize, m_pcCurrentPosition);
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->UpdateScreen(pVMC);
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->Show();
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
