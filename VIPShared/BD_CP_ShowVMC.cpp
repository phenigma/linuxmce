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
#include "PlutoUtils/PlutoDefs.h"
#include "VIPShared/VIPMenu.h"
#include "BD_CP_ShowVMC.h"

BD_CP_ShowVMC::BD_CP_ShowVMC(unsigned char Store,unsigned long VMCSize,const char *pVMC,
							 unsigned long iVMCFileNameSize, char *pVMCFileName) 
	
{
	m_iStore = Store;
	m_iVMCSize=VMCSize;
	m_pVMC=(char *)malloc(m_iVMCSize);
	memcpy(m_pVMC,pVMC,m_iVMCSize);

	m_iVMCFileNameSize = iVMCFileNameSize;
	m_pVMCFileName=(char *)malloc(m_iVMCFileNameSize);	
	memcpy(m_pVMCFileName, pVMCFileName, m_iVMCFileNameSize);
}	

BD_CP_ShowVMC::~BD_CP_ShowVMC()
{
	PLUTO_SAFE_DELETE(m_pVMC);
	PLUTO_SAFE_DELETE(m_pVMCFileName);
}

void BD_CP_ShowVMC::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_iStore);
	Write_long(m_iVMCSize);
	Write_block(m_pVMC,m_iVMCSize);
	Write_long(m_iVMCFileNameSize);
	Write_block(m_pVMCFileName, m_iVMCFileNameSize);
}

void BD_CP_ShowVMC::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size, data);

#ifdef VIPPHONE

	m_iStore = Read_unsigned_char();
	long m_iVMCSize = Read_long();
	const char *m_pVMC = Read_block(m_iVMCSize);
	long m_iVMCFileNameSize = Read_long();
	const char *m_pVMCFileName = Read_block(m_iVMCFileNameSize);

#ifdef SYMBIAN

	LOG("#	Received 'ShowVMC' command  #\n");
	
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->UpdateScreen(m_iStore == '1', m_iVMCSize, m_pVMC, 
		m_iVMCFileNameSize, m_pVMCFileName);
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->Show();
#endif

#ifdef VIPDESIGN

	//@todo
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
