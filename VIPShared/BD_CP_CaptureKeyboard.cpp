#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#include "PlutoUtils/CommonIncludes.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoBTAppUi.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "BD_CP_CaptureKeyboard.h"


BD_CP_CaptureKeyboard::BD_CP_CaptureKeyboard(
	bool bOnOff, bool bDataGrid, bool bReset, bool bTypePin, 
	int iVariable, string sText
) 
{
	m_bOnOff = bOnOff;
	m_bReset = bReset;
	m_bTypePin = bTypePin;
	m_bDataGrid = bDataGrid;
	m_iVariable = iVariable;
	m_sText = sText;
}

void BD_CP_CaptureKeyboard::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_bOnOff);
	Write_unsigned_char(m_bReset);
	Write_unsigned_char(m_bTypePin);
	Write_unsigned_char(m_bDataGrid);
	Write_long(m_iVariable);
	Write_string(m_sText);
}

void BD_CP_CaptureKeyboard::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);

	m_bOnOff = Read_unsigned_char()==1;
	m_bReset = Read_unsigned_char()==1;
	m_bTypePin = Read_unsigned_char()==1;
	m_bDataGrid = Read_unsigned_char()==1;
	
	m_iVariable = Read_long();
	Read_string(m_sText);
}


bool BD_CP_CaptureKeyboard::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE

#ifndef SYMBIAN 
	//
#else
	((CPlutoBTAppUi *)CCoeEnv::Static()->AppUi())->SetCaptureKeyboardCommand(
		m_bOnOff, m_bDataGrid, m_bReset, m_bTypePin, m_iVariable, m_sText
	);

#endif

#endif
	return true;
}

