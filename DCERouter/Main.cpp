/* 
 * $Id: Main.cpp,v 1.3 2004/10/16 12:36:39 aaron Exp $
 */
#include "PlutoUtils/CommonIncludes.h"	
#include <mysql.h>
#include <signal.h>
#include <iostream>
#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "../pluto_main/Table_DeviceData.h"
#include "../pluto_main/Table_DesignObj.h"

#define ROUTER_VERSION		"2.0 devel"

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

void sig_int(int sig)
{
	g_pRouter->Quit();
	printf("Got SIGINT, exiting");
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
   	srand((int) time(NULL));

	string DBHost="dce_router",DBUser="root",DBPassword="",DBName="pluto_main",BasePath="";
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
        cout << "Router, v." << ROUTER_VERSION << endl
                << "Usage: Router [-i installation] [-d device] [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-t tcp/ip port to listen on] [-f Base Path]" << endl
                << "hostname  -- address or DNS of database host, default is `dce_router`" << endl
                << "username  -- username for database connection" << endl
                << "password  -- password for database connection, default is `` (empty)" << endl
                << "database  -- database name" << endl
                << "port      -- port for database connection, default is 3306" << endl
                << "base path -- device requests for files will be relative to this path" << endl;

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


	if( BasePath.length()>0 && BasePath[ BasePath.length()-1 ]!='/' )
		BasePath += "/";

	bool bStartRouter=true;
	while( bStartRouter )
	{
		g_pRouter = new Router(PK_Device,Installation,BasePath,DBHost,DBUser,DBPassword,DBName,DBPort,ListenPort);
		bStartRouter = g_pRouter->Run();
		if( bStartRouter )
		{
			delete g_pRouter;
			Sleep(2000); // Sleep a few just to be sure everything had a chance to disconnect
		}
	}

#ifdef _WIN32
	WSACleanup();
#endif
	delete g_pPlutoLogger;
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
	return 0;
}
