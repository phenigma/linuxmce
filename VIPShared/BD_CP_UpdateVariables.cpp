#ifndef SYMBIAN
#include "BD/BDCommandProcessor.h"
#endif

#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "VIPShared/VIPMenu.h"
#include "BD_CP_UpdateVariables.h"

BD_CP_UpdateVariables::BD_CP_UpdateVariables(unsigned long MenuCollectionID) 
	
{
	m_iMenuCollectionID=MenuCollectionID;
}

void BD_CP_UpdateVariables::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_long(m_iMenuCollectionID);
	Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LONG_MAP(m_mapVariables));
	MYSTL_ITERATE_LONG_MAP(m_mapVariables,VIPVariable,pVariable,it)
	{
		Write_unsigned_long(pVariable->m_iVariableID);
		Write_string(pVariable->m_sCurrentValue);
	}
}

void BD_CP_UpdateVariables::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_iMenuCollectionID = Read_unsigned_long();
	unsigned long NumVariables = Read_unsigned_long();
	for(unsigned long i=0;i<NumVariables;++i)
	{
		int VariableID = Read_unsigned_long();
		string Value;
		Read_string(Value);
		VIPVariable *pVariable = new VIPVariable(VariableID,1,"",0,0);
		pVariable->m_sCurrentValue = Value;
		MYSTL_ADD_TO_LONG_MAP(m_mapVariables,pVariable,VariableID);
	}
}

bool BD_CP_UpdateVariables::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}

