#ifndef SYMBIAN
#include "BD/BDCommandProcessor.h"
#endif

#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "VIPShared/VIPMenu.h"
#include "BD_PC_VMCTerminated.h"

BD_PC_VMCTerminated::BD_PC_VMCTerminated(char TerminatingResolution,unsigned long MenuCollectionID) 
	
{
	m_iTerminatingResolution=TerminatingResolution;
	m_iMenuCollectionID=MenuCollectionID;
}

void BD_PC_VMCTerminated::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_char(m_iTerminatingResolution);
	Write_unsigned_long(m_iMenuCollectionID);
	Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LONG_MAP(m_mapVariables));
	MYSTL_ITERATE_LONG_MAP(m_mapVariables,VIPVariable,pVariable,it)
	{
		Write_unsigned_long(pVariable->m_iVariableID);
		Write_string(pVariable->m_sCurrentValue);
		Write_unsigned_char((unsigned char) pVariable->m_bEncryptBeforeSending);
		Write_unsigned_short(pVariable->m_iFormat);
	}
}

void BD_PC_VMCTerminated::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_iTerminatingResolution = Read_char();
	m_iMenuCollectionID = Read_unsigned_long();
	unsigned long NumVariables = Read_unsigned_long();
	for(unsigned long i=0;i<NumVariables;++i)
	{
		int VariableID = Read_unsigned_long();
		string Value;
		Read_string(Value);
		bool iEncrypt = Read_unsigned_char()==1;
		unsigned short iFormat = Read_unsigned_short();
		VIPVariable *pVariable = new VIPVariable(VariableID,1,"",iEncrypt,iFormat);
		pVariable->m_sCurrentValue = Value;
		MYSTL_ADD_TO_LONG_MAP(m_mapVariables,pVariable,VariableID);
	}
}

bool BD_PC_VMCTerminated::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}

