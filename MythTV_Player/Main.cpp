/*
	Main
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

//<-dceag-incl-b->
#include "MythTV_Player.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

// In source files stored in archives and packages, these 2 lines will have the release version (build)
// and the svn revision as a global variable that can be inspected within a core dump
#define  VERSION "<=version=>"
/*SVN_REVISION*/

namespace DCE
{
	Logger *g_pPlutoLogger;
}
using namespace DCE;
//<-dceag-incl-e->


//<-dceag-plug-b->
extern "C" {
	class Command_Impl *RegisterAsPlugIn(class Router *pRouter,int PK_Device,string sLogger)
	{
		if( sLogger=="dcerouter" )
		{
			g_pPlutoLogger = new ServerLogger(PK_Device, MythTV_Player::PK_DeviceTemplate_get_static(), "localhost");
			if( ! ((ServerLogger *) g_pPlutoLogger)->IsConnected() )
			{
				sLogger="stdout";
				cerr << "Failed to create server logger.  Reverting to stdout instead." << endl;
			}
		}
		
		if( sLogger=="null" )
			g_pPlutoLogger = new NullLogger();
		else if( sLogger=="stdout" )
			g_pPlutoLogger = new FileLogger(stdout);
		else if( sLogger!="dcerouter" )
			g_pPlutoLogger = new FileLogger(sLogger.c_str());

		g_pPlutoLogger->Write(LV_STATUS, "Device: %d loaded as plug-in",PK_Device);

		MythTV_Player *pMythTV_Player = new MythTV_Player(PK_Device, "localhost",true,false,pRouter);
		if( pMythTV_Player->m_bQuit )
		{
			delete pMythTV_Player;
			return NULL;
		}
		return pMythTV_Player;
	}
}
//<-dceag-plug-e->

//<-dceag-main-b->
int main(int argc, char* argv[]) 
{
	g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
	g_sBinaryPath = FileUtils::BasePath(argv[0]);

	cout << "MythTV_Player, v." << VERSION << endl
		<< "Visit www.plutohome.com for source code and license information" << endl << endl;

	string sRouter_IP="dcerouter";
	int PK_Device=0;
	string sLogger="stdout";

	bool bError=false; // An error parsing the command line
	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			bError=true;
		}

		c=argv[optnum][1];
		switch (c)
		{
		case 'r':
			sRouter_IP = argv[++optnum];
			break;
		case 'd':
			PK_Device = atoi(argv[++optnum]);
			break;
		case 'l':
			sLogger = argv[++optnum];
			break;
		default:
			bError=true;
			break;
		};
	}

	if (bError)
	{
		cout << "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
			<< "Usage: MythTV_Player [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]" << endl
			<< "-r -- the IP address of the DCE Router  Defaults to 'dcerouter'." << endl
			<< "-d -- This device's ID number.  If not specified, it will be requested from the router based on our IP address." << endl
			<< "-l -- Where to save the log files.  Specify 'dcerouter' to have the messages logged to the DCE Router.  Defaults to stdout." << endl;
		exit(0);
	}

#ifdef WIN32
	WORD    wVersion;
    WSADATA wsaData;
    wVersion = MAKEWORD( 1, 1 );
	if (WSAStartup(wVersion, &wsaData)!=0)
	{
		int ec = WSAGetLastError();
		char s[91];
		sprintf(s, "WSAStartup err %d", ec);
		cerr << s << endl;
		exit(1);
	}
#endif

	try
	{
		if( sLogger=="dcerouter" )
			g_pPlutoLogger = new ServerLogger(PK_Device, MythTV_Player::PK_DeviceTemplate_get_static(), sRouter_IP);
		else if( sLogger=="null" )
			g_pPlutoLogger = new NullLogger();
		else if( sLogger=="stdout" )
			g_pPlutoLogger = new FileLogger(stdout);
		else
			g_pPlutoLogger = new FileLogger(sLogger.c_str());
	}
	catch(...)
	{
		cerr << "Unable to create logger" << endl;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Device: %d starting.  Connecting to: %s",PK_Device,sRouter_IP.c_str());

	bool bReload=false;
	try
	{
		MythTV_Player *pMythTV_Player = new MythTV_Player(PK_Device, sRouter_IP);	
		if ( pMythTV_Player->Connect(pMythTV_Player->PK_DeviceTemplate_get()) ) 
		{
			g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
			pMythTV_Player->CreateChildren();
			pthread_join(pMythTV_Player->m_RequestHandlerThread, NULL);

		} 
		else 
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Connect() Failed");
		}

		if( pMythTV_Player->m_bReload )
			bReload=true;

		delete pMythTV_Player;
	}
	catch(string s)
	{
		cerr << "Exception: " << s << endl;
	}
	catch(const char *s)
	{
		cerr << "Exception: " << s << endl;
	}
	g_pPlutoLogger->Write(LV_STATUS, "Device: %d ending",PK_Device);
#ifdef WIN32
    WSACleanup();
#endif

	if( bReload )
		return 2;
	else
		return 0;
}
//<-dceag-main-e->
