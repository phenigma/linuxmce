#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#include "PlutoUtils/CommonIncludes.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "BD_CP_SendMeKeystrokes.h"


BD_CP_SendMeKeystrokes::BD_CP_SendMeKeystrokes(char YesNo) 
	
{
	m_YesNo=YesNo;
}

void BD_CP_SendMeKeystrokes::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_YesNo);
}

void BD_CP_SendMeKeystrokes::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_YesNo = Read_unsigned_char();
}

bool BD_CP_SendMeKeystrokes::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE

#ifndef SYMBIAN 
	g_pPlutoConfig->m_bSendKeyStrokes = m_YesNo;
#else
	LOG("#	Received 'SendKeyStrokes' command  #\n");
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bSendKeyStrokes = m_YesNo;
#endif

#endif
	return true;
}

