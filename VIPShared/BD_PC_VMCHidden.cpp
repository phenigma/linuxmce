#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "BD_PC_VMCHidden.h"

BD_PC_VMCHidden::BD_PC_VMCHidden(unsigned long MenuCollectionID) 
	
{
	m_iMenuCollectionID=MenuCollectionID;
}

void BD_PC_VMCHidden::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_long(m_iMenuCollectionID);
}

void BD_PC_VMCHidden::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_iMenuCollectionID = Read_unsigned_long();
}

bool BD_PC_VMCHidden::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}

