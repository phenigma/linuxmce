/*
 Main
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */
 
/* 
 * $Id: Main.cpp,v 1.3 2004/10/16 12:36:39 aaron Exp $
 */
#include "PlutoUtils/CommonIncludes.h"	
#include <mysql.h>
#include <signal.h>
#include <iostream>
#include "DCERouter.h"
#include "DCE/DCEConfig.h"
#include "DeviceData_Router.h"
#include "../pluto_main/Table_DeviceData.h"
#include "../pluto_main/Table_DesignObj.h"

#define  VERSION "<=version=>"
const char *g_szCompile_Date="<=compile_date=>";
/*SVN_REVISION*/

extern pluto_pthread_mutex_t *m_mapLockMutex;
#ifdef LL_DEBUG_FILE
	extern pluto_pthread_mutex_t *m_LL_DEBUG_Mutex;
#endif

using namespace DCE;

namespace DCE 
{
	class Logger *g_pPlutoLogger;
	class Router *g_pRouter;
}

DCEConfig g_DCEConfig;

#define SIGINT_NOTIFY_MESSAGE	"Got SIGINT, exiting\n"
void sig_int(int sig)
{
	if(g_pPlutoLogger) {
		g_pPlutoLogger->Write(LV_STATUS, SIGINT_NOTIFY_MESSAGE);
	} else {
		cout << SIGINT_NOTIFY_MESSAGE;
	}
	
	if(g_pRouter) {
		g_pRouter->Quit();
		delete g_pRouter;
	}
}

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{
	int err;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
int main(int argc, char *argv[])
{
#endif
	g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
	g_sBinaryPath = FileUtils::BasePath(argv[0]);
   		
		cout<<"Copyright (C) 2004 Pluto, Inc., a Florida Corporation"<<endl
		    <<"www.plutohome.com"<<endl
		    <<"Phone: +1 (877) 758-8648"<<endl
		    <<"This program is distributed according to the terms of the Pluto Public License, available at: "<<endl
		    <<"http://plutohome.com/index.php?section=public_license "<<endl
		    <<"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "<<endl
		    <<"without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "<<endl
		    <<"See the Pluto Public License for more details."<<endl << "---------------" << endl << endl;
 
	srand((int) time(NULL));

	string DBHost="dcerouter",DBUser="root",DBPassword="",DBName="pluto_main",BasePath="";
	int DBPort=3306,ListenPort=3450,Installation=0,PK_Device=0;
	string sLogger="stdout";

	bool bError=false; // An error parsing the command line
    char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			exit(1);
		}

		c=argv[optnum][1];
        switch (c)
        {
            case 'i':
                Installation = atoi(argv[++optnum]);
                break;
            case 'h':
                DBHost = argv[++optnum];
                break;
            case 'u':
                DBUser = argv[++optnum];
                break;
            case 'p':
                DBPassword = argv[++optnum];
                break;
            case 'D':
                DBName = argv[++optnum];
                break;
            case 'P':
                DBPort = atoi(argv[++optnum]);
                break;
            case 't':
                ListenPort = atoi(argv[++optnum]);
                break;
            case 'd':
                PK_Device = atoi(argv[++optnum]);
                break;
	    case 'f':
		BasePath = argv[++optnum];
		break;
	    case 'l':
		sLogger = argv[++optnum];
		break;
            default:
		bError=true;
		break;
        };
	}

    if ( bError)
    {
//                123456789012345678901234567890123456789012345678901234567890
	cout << "Router, v." << VERSION << endl
                << "Usage: Router [-i installation] [-d device] [-h hostname]"<<endl
		<< "[-u username] [-p password] [-D database] [-P mysql port]"<<endl
		<< "[-t tcp/ip port to listen on] [-f Base Path][-l Logger]" << endl
                << "-h hostname  -- address or DNS of database host, default is `dcerouter`" << endl
                << "-u username  -- username for database connection" << endl
                << "-p password  -- password for database connection, default is `` (empty)" << endl
                << "-D database  -- database name" << endl
                << "-P port      -- port for database connection, default is 3306" << endl
                << "-f base path -- device requests for files will be relative to this path" << endl
		<< "-l logger	 -- where to put the log" << endl;

        exit(0);
    }

		
	
	try
	{
		if( sLogger=="null" )
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

#ifndef _WIN32
	/* install interrupt handler*/
	signal(SIGINT, sig_int);
#endif
	
	
	

	if( BasePath.length()>0 && BasePath[ BasePath.length()-1 ]!='/' )
		BasePath += "/";

	bool bStartRouter=true;
	while( bStartRouter )
	{
		g_pRouter = new Router(PK_Device,Installation,BasePath,DBHost,DBUser,DBPassword,DBName,DBPort,ListenPort);
		bStartRouter = g_pRouter->Run();
		g_pPlutoLogger->Write(LV_STATUS, "PlutoServer: Exited with %d",(int) bStartRouter);
		if( bStartRouter )
		{
			break; // This reloading thing isn't working.  We're just going to return an exit code of 2 and let the scripts restart us
			delete g_pRouter;
			g_pRouter = NULL;
			Sleep(2000); // Sleep a few just to be sure everything had a chance to disconnect
		}
	}

#ifdef _WIN32
	WSACleanup();
#endif
	if( m_mapLockMutex )
	{
		pthread_mutex_destroy(&m_mapLockMutex->mutex);
		delete m_mapLockMutex;
	}
#ifdef LL_DEBUG_FILE
	if( m_LL_DEBUG_Mutex )
	{
		pthread_mutex_destroy(&m_LL_DEBUG_Mutex->mutex);
		delete m_LL_DEBUG_Mutex;
	}
#endif
	g_pPlutoLogger->Write(LV_STATUS, "PlutoServer: terminating now with %d",(int) bStartRouter);
	delete g_pPlutoLogger;
	if( bStartRouter )
		return 2;
	else
		return 0;
}
