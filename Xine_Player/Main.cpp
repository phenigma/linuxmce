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

/**
 *
 * @file Main.cpp
 * @brief entry point for the Xine_Player 
 * @author
 *
 */


//<-dceag-incl-b->
#include "Xine_Player.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#define  VERSION "testing"

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
		if( sLogger=="dce_router" )
		{
			g_pPlutoLogger = new ServerLogger(PK_Device, "localhost");
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
		else if( sLogger!="dce_router" )
			g_pPlutoLogger = new FileLogger(sLogger.c_str());

		g_pPlutoLogger->Write(LV_STATUS, "Device: %d loaded as plug-in",PK_Device);

		Xine_Player *pXine_Player = new Xine_Player(PK_Device, "localhost",true,false,pRouter);
		if( pXine_Player->m_bQuit )
		{
			delete pXine_Player;
			return NULL;
		}
		return pXine_Player;
	}
}
//<-dceag-plug-e->

//<-dceag-main-b->
int main(int argc, char* argv[]) 
{
	
//                123456789012345678901234567890123456789012345678901234567890
	cout<< " Copyright (C) 2004 Pluto, Inc., a Florida Corporation " << endl
	    	<< " www.plutohome.com " << endl
	    	<< " Phone: +1 (877) 758-8648 " << endl
	    	<< " This program is free software; you can redistribute it " << endl
		<< " and/or modify it under the terms of the GNU General Public License. " << endl
		<< " This program is distributed in the hope that it will be useful, " << endl
		<< " but WITHOUT ANY WARRANTY; without even the implied warranty " <<endl
		<< " of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. " <<endl
		<< " See the GNU General Public License for more details. "<< endl;	
	
	string sRouter_IP="dce_router";
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
		
//                      123456789012345678901234567890123456789012345678901234567890			
		cout << "Xine_Player, v." << VERSION << endl
			<< "A Pluto DCE Device. See www.plutohome.com/dce for details." << endl
			<< "Usage: MythTV_Player [-r Router's IP] [-d My Device ID] " << endl
			<< " [-l dce_router|stdout|null|filename]" << endl
			<< "-r router's IP	-- the IP address of the DCE Router " << endl
			<< "			 Defaults to 'dce_router'." << endl
			<< "-d my device ID	-- This device's ID number. " << endl
			<< "			If not specified, it will be requested " << endl
			<< "			from the router based on our IP address." << endl
			<< "-l dce_router|stdout|filename	-- Where to save the log files. " << endl
			<< "	Specify 'dce_router' to have the messages logged to the DCE Router." << endl
			<< "	Defaults to stdout." << endl;
			
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
		if( sLogger=="dce_router" )
			g_pPlutoLogger = new ServerLogger(PK_Device, sRouter_IP);
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

	g_pPlutoLogger->Write(LV_STATUS, "Device: %d starting",PK_Device);

	try
	{
		Xine_Player *pXine_Player = new Xine_Player(PK_Device, sRouter_IP);	
		if ( pXine_Player->Connect() ) 
		{
			g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
			pXine_Player->CreateChildren();
			pthread_join(pXine_Player->m_RequestHandlerThread, NULL);

		} 
		else 
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Connect() Failed");
		}

		delete pXine_Player;
	}
	catch(string s)
	{
		cerr << "Exception: " << s << endl;
	}
	catch(const char *s)
	{
		cerr << "Exception: " << s << endl;
	}
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}
//<-dceag-main-e->
