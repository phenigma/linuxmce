#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "stdafx.h"
#include "VIPDesign/VIPDesignDoc.h"
#endif
#else
//#ifndef SYMBIAN
//#include <iostream>
//#include <fstream>
//#endif
#endif

#ifndef SYMBIAN
//#include "PlutoUtils/CommonIncludes.h"
//#include "VIPShared/PlutoConfig.h"
#else
//#include "VIPShared/VIPIncludes.h"
#include "PlutoBTAppUi.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_ShowList.h"
#include "BD/BDCommandProcessor.h"

#ifndef SYMBIAN
BD_CP_ShowList::BD_CP_ShowList(
		unsigned long x, 
		unsigned long y, 
		unsigned long Width, 
		unsigned long Height,
		list<string>  &DataGridList
) 
{
	m_x		 = x;
	m_y		 = y;
	m_Width  = Width;
	m_Height = Height;

	for(list<string>::iterator it = DataGridList.begin(); it != DataGridList.end(); ++it)
		m_DataGridList.push_back(*it);
}
#endif //SYMBIAN

void BD_CP_ShowList::ConvertCommandToBinary()
{
#ifndef SYMBIAN
	
	BDCommand::ConvertCommandToBinary();

	Write_long(m_x);
	Write_long(m_y);
	Write_long(m_Width);
	Write_long(m_Height);

	Write_long(long(m_DataGridList.size()));
	for(list<string>::iterator it = m_DataGridList.begin(); it != m_DataGridList.end(); ++it)
        Write_string(*it);

	m_DataGridList.clear();

#endif //SYMBIAN
}

void BD_CP_ShowList::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size, data);
#ifdef VIPPHONE

#ifdef SYMBIAN

	unsigned long x = Read_long();
	unsigned long y = Read_long();
	unsigned long Width = Read_long();
	unsigned long Height = Read_long();

	unsigned long ListSize = Read_long();

	RPointerArray<string> DatagridStringList;
	for(int i = 0; i < ListSize; i++)
	{
		string *s = new string;
		Read_string(*s);

		DatagridStringList.Append(s);
	}

	((CPlutoBTAppUi *)CCoeEnv::Static()->AppUi())->ShowList(x, y, Width, Height, DatagridStringList);
	//((CPlutoBTAppUi *)CCoeEnv::Static()->AppUi())->Show();

#endif //SYMBIAN

#ifdef VIPDESIGN
	//TODO: implement this in VIPDESIGN too.
#endif //VIPDESIGN

#endif //VIPPHONE
}

bool BD_CP_ShowList::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
