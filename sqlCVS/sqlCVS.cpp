//#define C99_FORMAT_SPECIFIERS
#include "PlutoUtils/CommonIncludes.h"	

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32
#include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>

#include <stdio.h>
#include <conio.h>

#include <string>
#include <vector>
#include <map>
#include <list>

#include <mysql.h>

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "Database.h"
#include "CommonFunctions.h"
#include "DCE/Logger.h"
#include "RA/RAServer.h"

#define sqlCVS_VERSION "0.1"

using namespace std;
using namespace sqlCVS;

vector<string> tables_list;

namespace DCE
{
	class Logger *g_pPlutoLogger;
}

namespace sqlCVS
{
	GlobalConfig g_GlobalConfig;
}

using namespace DCE;
using namespace sqlCVS;

string GetCommand()
{
	cout << "What would you like to do?" << endl
		<< "In the future you can also make your selections on the command line using the command shown in parenthesis." << endl
		<< "------Server-side functions------" << endl
		<< "1.	Create a new sqlCVS repository from an existing database (create)" << endl
		<< "2.	Edit an existing repository (edit)" << endl
		<< "3.	Start listening for incoming connections from clients (listen)" << endl
		<< "4.	Permanently roll-back checkins (rollback)" << endl
		<< "------Client-side functions------" << endl
		<< "A.	Create a working, local copy of the data, either current or a past snapshot (copy)" << endl
		<< "B.	Check-in changes you've made locally (checkin)" << endl
		<< "C.	View my local changes (diff)" << endl;

	char c=(char) getch();

	switch(c)
	{
	case '1':
		return "create";
	case '2':
		return "edit";
	case '3':
		return "listen";
	case '4':
		return "rollback";

	case 'A':
	case 'a':
		return "copy";
	case 'B':
	case 'b':
		return "checkin";
	case 'C':
	case 'c':
		return "diff";
	}
	return "";
}

int main(int argc, char *argv[])
{
	bool bError=false; // An error parsing the command line

	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			g_GlobalConfig.m_sCommand = argv[++optnum];
			if( optnum<argc )
				bError=true;	// The command must be the last thing on the line
			break;
		}

		c=argv[optnum][1];
		switch (c)
		{
		case 'h':
			g_GlobalConfig.m_sDBHost = argv[++optnum];
			break;
		case 'u':
			g_GlobalConfig.m_sDBUser = argv[++optnum];
			break;
		case 'p':
			g_GlobalConfig.m_sDBPassword = argv[++optnum];
			break;
		case 'D':
			g_GlobalConfig.m_sDBName = argv[++optnum];
			break;
		case 'P':
			g_GlobalConfig.m_iDBPort = atoi(argv[++optnum]);
			break;
		case 'r':
			g_GlobalConfig.m_sRepository = argv[++optnum];
			break;
		case 't':
			g_GlobalConfig.m_sTable = argv[++optnum];
			break;
		default:
			bError=true;
			break;
		};
	}

	if ( bError)
	{
		cout << "sqlCVS, v." << sqlCVS_VERSION << endl
			<< "Usage: sqlCVS [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-r Repository(-ies)] [-i Table(s)]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "port        -- port for database connection, default is 3306" << endl
			<< "output path -- Where to put the output files.  Default is ../[database name]" << endl
			<< "input path  -- Where to find the template files.  Default is . then ../sqlCVS" << endl;

		exit(0);
	}

	try
	{
		g_pPlutoLogger = new DCE::FileLogger("sqlCVS.log");
		if(g_pPlutoLogger == NULL)
		{
			cerr << "Problem creating logger.  Check params." << endl;
			exit(1);
		}

#ifdef WIN32
		long err;
		WSADATA wsaData;
		err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#endif
		Database database(g_GlobalConfig.m_sDBHost, g_GlobalConfig.m_sDBUser, g_GlobalConfig.m_sDBPassword, g_GlobalConfig.m_sDBName, g_GlobalConfig.m_iDBPort);

		// Fill the lists with any repositories or tables that were passed in on the command line
		database.GetRepositoriesTables();

		while( true ) // An endless loop processing commands
		{
			if( g_GlobalConfig.m_sCommand.length()==0 )
			{
				g_GlobalConfig.m_sCommand=GetCommand();
				database.m_bInteractiveMode=true;
			}
			if( g_GlobalConfig.m_sCommand=="create" )
				database.CreateRepository();
			else if( g_GlobalConfig.m_sCommand=="checkin" )
			{
				if( g_GlobalConfig.m_sRepository.length() )
				{   
					// If it was a valid repository, GetRepositoriesTables would have replaced it with a pointer to the repository in mapRepository
					cerr << "Repository: " << g_GlobalConfig.m_sRepository << "is invalid";
					throw "Bad Arguments";
				}
				database.CheckIn();
			}
			else if( g_GlobalConfig.m_sCommand=="listen" )
			{
				RAServer *pServer = new RAServer();
				pServer->Initialize();
				pServer->StartListening(g_GlobalConfig.m_iSqlCVSPort);
				pServer->Run();
				delete pServer;
			}
/*
			else if( Command=="add-tables" )
				database.AddTablesToRepository( sRepository, &listCommandParms );
			else if( Command=="remove-tables" )
				database.RemoveTablesFromRepository( sRepository, &listCommandParms );
			else if( Command=="listen" )
				rp.Listen(iListenPort, &database, sRepository);
			else if( Command=="dump" )
				database.DumpTables( sRepository, &listCommandParms );

*/
return 0;
		};
	}
	catch(char *pException)
	{
		cerr << "Caught exception: " << pException;
	}

	delete g_pPlutoLogger;
#ifdef _WIN32
		WSACleanup();
#endif
	return 0;
}
