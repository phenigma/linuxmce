/*
	Mozilla_Plugin
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/


//<-dceag-d-b->
#include "Mozilla_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DataGrid.h"
#include "DCERouter.h"
#include "DCE/DataGrid.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Mozilla_Plugin::Mozilla_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Mozilla_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Mozilla_Plugin::~Mozilla_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Mozilla_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin = NULL;
	ListCommand_Impl * pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Datagrid_Plugin_CONST);

	if (!pListCommand_Impl || pListCommand_Impl->size() != 1)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "File grids cannot find datagrid handler %s", (pListCommand_Impl ? "There were more than 1" : ""));
		return false;
	}

	m_pDatagrid_Plugin=(Datagrid_Plugin *) pListCommand_Impl->front();

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&Mozilla_Plugin::BookmarkList)), DATAGRID_Mozilla_Bookmarks_CONST);

	return Connect(PK_DeviceTemplate_get());
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Mozilla_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Mozilla_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

class DataGridTable * Mozilla_Plugin::BookmarkList(string GridID, string Parms, void * ExtraData,int *iPK_Variable,string *sValue_To_Assign, class Message *pMessage)
{
	DataGridTable * pDataGrid = new DataGridTable;
	DataGridCell * pCell;

	string sPK_User = Parms;

	list<pair<string, string> > Bookmarks = GetUserBookmarks(sPK_User);
	list<pair<string, string> >::iterator i;
	int row = 0;
	for (i = Bookmarks.begin(); i != Bookmarks.end(); i++, row++)
	{
		pCell = new DataGridCell((*i).second, (*i).first);
		pDataGrid->SetData(0, row, pCell);
	}

	return pDataGrid;
}

pair<string, string> strpair(string x, string y)
{
	return pair<string, string>(x, y);
}

list<pair<string, string> > Mozilla_Plugin::GetUserBookmarks(string sPK_User)
{
	list<pair<string, string> > Bookmarks;

	Bookmarks.push_back(strpair("http://www.audi.de/", "Audi Deutschland"));
	Bookmarks.push_back(strpair("http://digitallogic.ch/english/index.asp", "DIGITAL-LOGIC AG (English)"));
	Bookmarks.push_back(strpair("http://digitallogic.ch/deutsch/index.asp", "DIGITAL-LOGIC AG (Deutsch)"));
	Bookmarks.push_back(strpair("http://www.plutohome.com/", "Plutohome.com"));
	return Bookmarks;
	
	// the following code reads the Mozilla bookmarks
	
	size_t Size;
//	char * Buffer = FileUtils::ReadFileIntoBuffer("/home/user_" + sPK_User + "/.mozilla/firefox/profiles.ini", Size);
	char * Buffer = FileUtils::ReadFileIntoBuffer("/root/.mozilla/firefox/profiles.ini", Size);
	if( !Buffer )
		return Bookmarks;

	const char *Path = strstr(Buffer,"Path=");
	if( !Path )
	{
		delete [] Buffer;
		return Bookmarks;
	}

	Path+=5;

	char *EOL = strchr(Path,'\n');
	if( !EOL )
	{
		delete[] Buffer;
		return Bookmarks;
	}

	*EOL=0;
	string Filename = Path;
	delete[] Buffer;

//	Buffer = FileUtils::ReadFileIntoBuffer("/home/user_" + sPK_User + "/.mozilla/firefox/" + Filename + "/bookmarks.html", Size);
	Buffer = FileUtils::ReadFileIntoBuffer("/root/.mozilla/firefox/" + Filename + "/bookmarks.html", Size);
	if( !Buffer )
		return Bookmarks;

	char * BufferTop = Buffer;
	
	char *BraceA;
	char *PosInBuffer=Buffer;
	while( (BraceA=strstr(Buffer,"<A")) )
	{
		char *HRef = strstr(BraceA,"HREF");
		if( !HRef )
		{
			Buffer++;  // Skip past this and continue
			continue;
		}
		char *FirstQuote = strchr(HRef,'"');
		if( !FirstQuote )
		{
			Buffer++;  // Skip past this and continue
			continue;
		}
		char *SecondQuote = strchr(FirstQuote+1,'"');
		if( !SecondQuote )
		{
			Buffer++;  // Skip past this and continue
			continue;
		}
		*SecondQuote=0;
		string Link(FirstQuote+1);

		char *LastBrace = strchr(SecondQuote+1,'>');
		if( !LastBrace )
		{
			Buffer++;  // Skip past this and continue
			continue;
		}

		char * EndBraceA = strstr(LastBrace+1, "</A>");
		*EndBraceA = 0;
		string LinkText(LastBrace+1);

		Buffer = LastBrace+1;

		Bookmarks.push_back(pair<string, string>(Link, LinkText));
	}

	delete[] BufferTop;
	return Bookmarks;
}

//<-dceag-sample-b->!
//<-dceag-createinst-b->!
