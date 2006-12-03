#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "Logger.h"
#include "UpdateEntArea.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif

#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceGroup.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Users.h"

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;
using namespace DefaultScenarios;

UpdateEntArea::UpdateEntArea()
{
}

UpdateEntArea::~UpdateEntArea()
{
	if(NULL != m_pDatabase_pluto_main)
	{
		delete m_pDatabase_pluto_main;
		m_pDatabase_pluto_main = NULL;
	}
}

void UpdateEntArea::AddDefaultScenarios()
{
	AddDefaultMediaScenarios();
	AddDefaultLightingScenarios();
	AddDefaultClimateScenarios();
	AddDefaultTelecomScenarios();
	AddDefaultSecurityScenarios();
	AddDefaultEventHandlers();
}

void UpdateEntArea::AddShortcut(int PK_Room, char cCharacter, string sMessage)
{
	MapShortcuts *pMapShortcuts=NULL;
	map<int, MapShortcuts *>::iterator it=m_mapOrbiter_Shortcuts.find(PK_Room);
	if( it==m_mapOrbiter_Shortcuts.end() )
	{
		pMapShortcuts = new MapShortcuts;
		m_mapOrbiter_Shortcuts[PK_Room]=pMapShortcuts;
	}
	else
		pMapShortcuts = it->second;

	(*pMapShortcuts)[cCharacter] = sMessage;
}

void UpdateEntArea::UpdateOrbiterShortcuts()
{
	// Override the DEVICEDATA_Shortcut_CONST to have the shortcuts in our m_mapOrbiter_Shortcuts, unless
	// there's already a shortcut for that orbiter where Automatic=false, meaning the user manually updated it on his own
	vector<Row_Device *> vectRow_Orbiters;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" TOSTRING(DEVICECATEGORY_Standard_Orbiter_CONST),
		&vectRow_Orbiters);
	for(vector<Row_Device *>::iterator it=vectRow_Orbiters.begin();it!=vectRow_Orbiters.end();++it)
	{
		Row_Device *pRow_Device = *it;
		// Get the shortcuts we have for this room
		map<int, MapShortcuts *>::iterator itMS=m_mapOrbiter_Shortcuts.find(pRow_Device->FK_Room_get());
		MapShortcuts *pMapShortcuts = itMS!=m_mapOrbiter_Shortcuts.end() ? itMS->second : NULL;

		// Get the shortcuts the orbiter already has and put them in here which weren't automatically done
		map< pair<bool,char>, string> mapOrbiterShortcuts; // bool=automatic, char=character
		Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Shortcut_CONST);
		if( pRow_Device_DeviceData )
		{
			vector<string> vectExistingShortcuts;
			string s = pRow_Device_DeviceData->IK_DeviceData_get();
			StringUtils::Tokenize(s,"\r\n",vectExistingShortcuts);
			for(vector<string>::iterator it=vectExistingShortcuts.begin();it!=vectExistingShortcuts.end();++it)
			{
				string::size_type pos=0;
				bool bAutomatic = StringUtils::Tokenize( *it, "\t", pos )=="1";
				string sCharacter = StringUtils::Tokenize( *it, "\t", pos );
				string sMessage = StringUtils::Tokenize( *it, "\t", pos );

				if( sCharacter.empty() )
					continue; // Shouldn't happen

				if( bAutomatic==false )
					mapOrbiterShortcuts[ make_pair<bool,char> (false,sCharacter[0]) ] = sMessage;
			}
		}

		// Now we've added back in all the shortcuts the user did manually.  Add in our own
		if( pMapShortcuts )
		{
			for(map<char,string>::iterator it=pMapShortcuts->begin();it!=pMapShortcuts->end();++it)
			{
				if( mapOrbiterShortcuts.find( make_pair<bool,char> (false,it->first) )==mapOrbiterShortcuts.end() )
				{
					// The user doesn't already have his own manual shortcut.  Use this automatic one
					mapOrbiterShortcuts[ make_pair<bool,char> (true,it->first) ] = it->second;
				}
			}
		}

		// Now we've got all the shortcuts.  Make them a string again and update the database
		string sOutput,sChar=" ";
		for( map< pair<bool,char>, string>::iterator itMS=mapOrbiterShortcuts.begin();itMS!=mapOrbiterShortcuts.end();++itMS )
		{
			sChar[0] = itMS->first.second;  // Doesn't seem to like have a + and a char
			sOutput += (itMS->first.first ? "1\t" : "0\t") + sChar + "\t" + itMS->second + "\n";
		}

		DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Shortcut_CONST,sOutput);
	}
}

