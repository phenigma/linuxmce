/*
 BD_CP_ShowList
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

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
#include "BD/BDCommandProcessor.h"
#else
#include "PlutoMOAppUi.h"
#include "Logger.h" 
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_ShowList.h"


#ifndef SYMBIAN
BD_CP_ShowList::BD_CP_ShowList(
		unsigned long x, 
		unsigned long y, 
		unsigned long Width, 
		unsigned long Height,
		list<string>  &DataGridList,
		bool		  bSendSelectedOnMove,
		bool		  bTurnOn
) 
{
	m_x		 = x;
	m_y		 = y;
	m_Width  = Width;
	m_Height = Height;
	m_bSendSelectedOnMove = bSendSelectedOnMove;
	m_bTurnOn = bTurnOn;

	for(list<string>::iterator it = DataGridList.begin(); it != DataGridList.end(); ++it)
		m_DataGridList.push_back(*it);
}
#endif //SYMBIAN

BD_CP_ShowList::~BD_CP_ShowList()
{
#ifndef SYMBIAN
	m_DataGridList.clear();
#else
	m_DataGridList.Close();
#endif //SYMBIAN
}

void BD_CP_ShowList::ConvertCommandToBinary()
{
#ifndef SYMBIAN
	
	BDCommand::ConvertCommandToBinary();

	Write_long(m_x);
	Write_long(m_y);
	Write_long(m_Width);
	Write_long(m_Height);
	Write_long(m_bSendSelectedOnMove);
	Write_long(m_bTurnOn);

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

	m_x = Read_long();
	m_y = Read_long();
	m_Width = Read_long();
	m_Height = Read_long();
	m_bSendSelectedOnMove = Read_long();
	m_bTurnOn = Read_long();

	unsigned long ListSize = Read_long();

	for(int i = 0; i < ListSize; i++)
	{
		string *s = new string;
		Read_string(*s);

		m_DataGridList.Append(s);
	}

	LOG("#	Received 'ShowList' command  #\n"); 
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->ShowList(m_x, m_y, m_Width, m_Height, m_DataGridList,
		m_bSendSelectedOnMove, m_bTurnOn);

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
