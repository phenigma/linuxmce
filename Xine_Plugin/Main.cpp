/**
 *
 * @file Main.cpp
 * @brief entry point for the Xine_Plugin
 * @author
 *
 */

 /**
 *
 * Copyright Notice goes here
 *
 */

//<-dceag-incl-b->
#include "Xine_Plugin.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#define Xine_Plugin_VERSION "testing"

namespace DCE
{
	Logger *g_pPlutoLogger;
}
using namespace DCE;
//<-dceag-incl-e->


//<-dceag-plug-b->
extern "C" {
	class Command_Impl *RegisterAsPlugIn( class Router *pRouter, int PK_Device, string sLogger )
	{
		if( sLogger=="dce_router" )
		{
			g_pPlutoLogger = new ServerLogger( PK_Device, "localhost" );
			if( ! ( ( ServerLogger * ) g_pPlutoLogger )->IsConnected( ) )
			{
				sLogger="stdout";
				cerr << "Failed to create server logger. Reverting to stdout instead." << endl;
			}
		}
		
		if( sLogger=="null" )
			g_pPlutoLogger = new NullLogger( );
		else if( sLogger=="stdout" )
			g_pPlutoLogger = new FileLogger( stdout );
		else if( sLogger!="dce_router" )
			g_pPlutoLogger = new FileLogger( sLogger.c_str( ) );

		g_pPlutoLogger->Write( LV_STATUS, "Device: %d loaded as plug-in", PK_Device );

		Xine_Plugin *pXine_Plugin = new Xine_Plugin( PK_Device, "localhost", true, false, pRouter );
		if( pXine_Plugin->m_bQuit )
		{
			delete pXine_Plugin;
			return NULL;
		}
		return pXine_Plugin;
	}
}
//<-dceag-plug-e->

//<-dceag-main-b->
int main( int argc, char* argv[] ) 
{
	string sRouter_IP="dce_router";
	int PK_Device=0;
	string sLogger="stdout";

	bool bError=false; // An error parsing the command line
	char c;
	for( int optnum=1;optnum<argc;++optnum )
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			bError=true;
		}

		c=argv[optnum][1];
		switch ( c )
		{
		case 'r':
			sRouter_IP = argv[++optnum];
			break;
		case 'd':
			PK_Device = atoi( argv[++optnum] );
			break;
		case 'l':
			sLogger = argv[++optnum];
			break;
		default:
			bError=true;
			break;
		};
	}

	if ( bError )
	{
		cout << "Xine_Plugin, v." << Xine_Plugin_VERSION << endl
			<< "A Pluto DCE Device. See www.plutohome.com/dce for details." << endl
			<< "Usage: Xine_Plugin [-r Router's IP] [-d My Device ID] [-l dce_router|stdout|null|filename]" << endl
			<< "-r -- the IP address of the DCE Router Defaults to 'dce_router'." << endl
			<< "-d -- This device's ID number. If not specified, it will be requested from the router based on our IP address." << endl
			<< "-l -- Where to save the log files. Specify 'dce_router' to have the messages logged to the DCE Router. Defaults to stdout." << endl;
		exit( 0 );
	}

#ifdef WIN32
	WORD  wVersion;
  WSADATA wsaData;
  wVersion = MAKEWORD( 1, 1 );
	if ( WSAStartup( wVersion, &wsaData )!=0 )
	{
		int ec = WSAGetLastError( );
		char s[91];
		sprintf( s, "WSAStartup err %d", ec );
		cerr << s << endl;
		exit( 1 );
	}
#endif

	try
	{
		if( sLogger=="dce_router" )
			g_pPlutoLogger = new ServerLogger( PK_Device, sRouter_IP );
		else if( sLogger=="null" )
			g_pPlutoLogger = new NullLogger( );
		else if( sLogger=="stdout" )
			g_pPlutoLogger = new FileLogger( stdout );
		else
			g_pPlutoLogger = new FileLogger( sLogger.c_str( ) );
	}
	catch( ... )
	{
		cerr << "Unable to create logger" << endl;
	}

	g_pPlutoLogger->Write( LV_STATUS, "Device: %d starting", PK_Device );

	try
	{
		Xine_Plugin *pXine_Plugin = new Xine_Plugin( PK_Device, sRouter_IP );	
		if ( pXine_Plugin->Connect( ) ) 
		{
			g_pPlutoLogger->Write( LV_STATUS, "Connect OK" );
			pXine_Plugin->CreateChildren( );
			pthread_join( pXine_Plugin->m_RequestHandlerThread, NULL );

		} 
		else 
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Connect( ) Failed" );
		}

		delete pXine_Plugin;
	}
	catch( string s )
	{
		cerr << "Exception: " << s << endl;
	}
	catch( const char *s )
	{
		cerr << "Exception: " << s << endl;
	}
#ifdef WIN32
  WSACleanup( );
#endif
  return 0;
}
//<-dceag-main-e->
