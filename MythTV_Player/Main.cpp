//<-dceag-incl-b->
#include "MythTV_Player.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#define  MythTV_Player_VERSION "testing"

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
		cout << "MythTV_Player, v." << MythTV_Player_VERSION << endl
			<< "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
			<< "Usage: MythTV_Player [-r Router's IP] [-d My Device ID] [-l dce_router|stdout|null|filename]" << endl
			<< "-r -- the IP address of the DCE Router  Defaults to 'dce_router'." << endl
			<< "-d -- This device's ID number.  If not specified, it will be requested from the router based on our IP address." << endl
			<< "-l -- Where to save the log files.  Specify 'dce_router' to have the messages logged to the DCE Router.  Defaults to stdout." << endl;
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
		MythTV_Player *pMythTV_Player = new MythTV_Player(PK_Device, sRouter_IP);	
		if ( pMythTV_Player->Connect() ) 
		{
			g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
			pMythTV_Player->CreateChildren();
			pthread_join(pMythTV_Player->m_RequestHandlerThread, NULL);

		} 
		else 
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Connect() Failed");
		}

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
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}
//<-dceag-main-e->
