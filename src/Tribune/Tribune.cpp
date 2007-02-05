#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>

#include <stdio.h>

#include <string>
#include <vector>
#include <map>
#include <list>

#include "DCE/Logger.h"
#include "RA/RAServer.h"

using namespace std;
using namespace DCE;

namespace DCE
{
	class Logger *g_pPlutoLogger;
}


struct GlobalConfig_t{
	string m_sCommand;
	string m_sTribuneHost;
	int m_iTribunePort;
};

string GetCommand( )
{

	cout << "Use Tribune -? for command line options." << endl << endl
		<< "What would you like to do?" << endl
		<< "------Server-side functions------" << endl
		<< "1.	Start listening for incoming connections from clients ( listen )" << endl
		<< "------Client-side functions------" << endl
		<< "A.	" << endl
		<< endl
		<< "Q. Quit" << endl;

	string s;
	cin >> s;

	if( s=="1" )
		return "listen";
	else if( s=="q" || s=="Q" )
		exit( 0 );

	return "";
}


int main(int argc, char *argv[]){

	
	
	GlobalConfig_t g_GlobalConfig;
	g_GlobalConfig.m_sTribuneHost="localhost";
	g_GlobalConfig.m_iTribunePort=9003;

	cout << " Copyright (C) 2004 Pluto, Inc., a Florida Corporation " << endl
	    	<< " www.plutohome.com " << endl
	    	<< " Phone: +1 (877) 758-8648 " << endl
	    	<< " This program is free software; you can redistribute it " << endl
		<< " and/or modify it under the terms of the GNU General Public License. " << endl
		<< " This program is distributed in the hope that it will be useful, " << endl
		<< " but WITHOUT ANY WARRANTY; without even the implied warranty " <<endl
		<< " of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. " <<endl
		<< " See the GNU General Public License for more details. "<< endl << "-------" << endl << endl;

	g_pPlutoLogger = new DCE::FileLogger( stdout );
	if( g_pPlutoLogger == NULL )
	{
		cerr << "Problem creating logger. Check params." << endl;
		exit( 1 );
	}

	bool bError=false; /** An error parsing the command line */

	try
	{
		char c;
		for( int optnum=1;optnum<argc;++optnum )
		{
			if( argv[optnum][0]!='-' )
			{
				g_GlobalConfig.m_sCommand = argv[optnum];
				if( optnum<argc-1 )
				{
					cerr << "If " << argv[optnum] << " is the command it must be at the end of the line" << endl;
					bError=true;	/** The command must be the last thing on the line */
				}
				break;
			}

			c=argv[optnum][1];
			switch ( c )
			{
			case 'H':
				g_GlobalConfig.m_sTribuneHost = argv[++optnum];
				break;
			case 'P':
				g_GlobalConfig.m_iTribunePort = atoi( argv[++optnum] );
				break;
			case '?':
				bError=true;
				break;
			default:
				cerr << "Unknown option: " << argv[optnum] << endl;
				bError=true;
				break;
			};
		}
	}
	catch(...)
	{
		bError=true;
	}

	if ( bError )
	{
		cout << "Tribune"<<endl
			<< "Usage: Tribune [-H Tribune hostname] [-P Tribune port] " << endl
			<< "-P port    	   -- port for tribune connection, " << endl
			<< "			default is 99003" << endl
			<< "-H hostname    -- address or DNS of Tribune host," << endl
			<< "			default is `localhost`" << endl;
		exit( 1 );
	}

	while( true ) /** An endless loop processing commands */
	{
		while( g_GlobalConfig.m_sCommand.length( )==0 )
		{
			g_GlobalConfig.m_sCommand=GetCommand( );
		}
		if( g_GlobalConfig.m_sCommand=="listen" )
		{
			//cout << "ascult stapane" << endl;
			RAServer *pServer = new RAServer( );
			pServer->Initialize( );
			pServer->StartListening( g_GlobalConfig.m_iTribunePort );
			pServer->Run( );
			delete pServer;
		}
		else {
			cerr << "Unknown command: " << g_GlobalConfig.m_sCommand << endl;
		}
	}

}
