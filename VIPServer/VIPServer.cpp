#include "PlutoUtils/CommonIncludes.h"	

#ifndef WIN32
#include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>

#include <stdio.h>
#ifdef WIN32
#include <conio.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <list>

#include <mysql.h>

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "RA/RAServer.h"
#include "DCE/DCEConfig.h"

#define  VERSION "<=version=>"

using namespace std;
namespace DCE
{
	class Logger *g_pPlutoLogger;
}
using namespace DCE;

//PlutoConfig g_dceConfig;
//class PlutoConfig *g_pPlutoConfig;  // Temporary.  Required at some point, but we won't use it.

int main(int argc, char *argv[])
{
	bool bError=false; // An error parsing the command line
	int iPort=3460;

	char c;
	/*
	for(int optnum=1;optnum<argc;++optnum)
	{
		c=argv[optnum][1];
		switch (c)
		{
		case 'h':
			g_dceConfig.m_sDBHost = argv[++optnum];
			break;
		case 'u':
			g_dceConfig.m_sDBUser = argv[++optnum];
			break;
		case 'p':
			g_dceConfig.m_sDBPassword = argv[++optnum];
			break;
		case 'D':
			g_dceConfig.m_sDBName = argv[++optnum];
			break;
		case 'P':
			g_dceConfig.m_iDBPort = atoi(argv[++optnum]);
			break;
		case 'l':
			iPort = atoi(argv[++optnum]);
		default:
			bError=true;
			break;
		};
	}
	*/

	if ( bError)
	{
		cout << "VIPServer, v." << VERSION << endl
			<< "Usage: sqlCVS [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-r Repository(-ies)] [-t Table(s)] [-U Users(s)]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "port        -- port for database connection, default is 3306" << endl;

		exit(0);
	}

	try
	{
		g_pPlutoLogger = new DCE::FileLogger(stdout);
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

		RAServer *pServer = new RAServer();
		pServer->Initialize();
		pServer->StartListening(iPort);
		pServer->Run();
		delete pServer;

		return 0;
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
