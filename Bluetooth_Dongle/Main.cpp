/**
 *
 * @file Main.cpp
 * @brief entry point for the BluetoothDongle 
 * @author
 * @todo notcommented
 *
 */

 /**
 *
 * Copyright Notice goes here
 *
 */

//<-dceag-incl-b->
#include "Bluetooth_Dongle.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#define Bluetooth_Dongle_VERSION "testing"
/** @testing */

namespace DCE
{
	Logger *g_pPlutoLogger;
}
using namespace DCE;
//<-dceag-incl-e->

//<-dceag-plug-b->
extern "C" 
{
	class Command_Impl *RegisterAsPlugIn( class Router *pRouter, int dwPK_Device, string sLogger )
	{
		if( sLogger=="dce_router" )
		{
			g_pPlutoLogger = new ServerLogger( dwPK_Device, "localhost" );
			if( ! ( ( ServerLogger * ) g_pPlutoLogger )->IsConnected() )
			{
				sLogger="stdout";
				cerr << "Failed to create server logger. Reverting to stdout instead." << endl;
			}
		}
		
		if( sLogger=="null" )
			g_pPlutoLogger = new NullLogger();
		else if( sLogger=="stdout" )
			g_pPlutoLogger = new FileLogger( stdout );
		else if( sLogger!="dce_router" )
			g_pPlutoLogger = new FileLogger( sLogger.c_str() );

		g_pPlutoLogger->Write( LV_STATUS, "Device: %d loaded as plug-in", dwPK_Device );

		Bluetooth_Dongle *pBluetooth_Dongle = new Bluetooth_Dongle( dwPK_Device, "localhost", true, false, pRouter );
		if( pBluetooth_Dongle->m_bQuit )
		{
			delete pBluetooth_Dongle;
			return NULL;
		}
		return pBluetooth_Dongle;
	}
}
//<-dceag-plug-e->

//<-dceag-main-b->
int main( int argc, char* argv[] ) 
{
	string sRouter_IP="dce_router";
	int dwPK_Device=0;
	string sLogger="stdout";

	bool bError=false; // An error parsing the command line
	char c;
	for( int iOptnum=1; iOptnum < argc; ++iOptnum ) // checking the command line parameters
	{
		if( argv[iOptnum][0] != '-' )
		{
			cerr << "Unknown option " << argv[iOptnum] << endl;
			bError=true;
		}

		c=argv[iOptnum][1];
		switch ( c )
		{
		case 'r':
			sRouter_IP = argv[++iOptnum];
			break;
		case 'd':
			dwPK_Device = atoi( argv[++iOptnum] );
			break;
		case 'l':
			sLogger = argv[++iOptnum];
			break;
		default:
			bError = true;
			break;
		};
	}

	if ( bError ) // an error parsng the command line parameters, displaying the help
	{
		cout << "Bluetooth_Dongle, v." << Bluetooth_Dongle_VERSION << endl
			<< "A Pluto DCE Device. See www.plutohome.com/dce for details." << endl
			<< "Usage: Bluetooth_Dongle [-r Router's IP] [-d My Device ID] [-l dce_router|stdout|null|filename]" << endl
			<< "-r -- the IP address of the DCE Router Defaults to 'dce_router'." << endl
			<< "-d -- This device's ID number. If not specified, it will be requested from the router based on our IP address." << endl
			<< "-l -- Where to save the log files. Specify 'dce_router' to have the messages logged to the DCE Router. Defaults to stdout." << endl;
		exit( 0 );
	}

#ifdef WIN32
	WORD wVersion;
	WSADATA wsaData;
	wVersion = MAKEWORD( 1, 1 );
	if ( WSAStartup( wVersion, &wsaData ) != 0 )
	{
		int ec = WSAGetLastError();
		char s[91];
		sprintf( s, "WSAStartup err %d", ec );
		cerr << s << endl;
		exit( 1 );
	}
#endif

	try
	{
		if( sLogger == "dce_router" )
			g_pPlutoLogger = new ServerLogger( dwPK_Device, sRouter_IP );
		else if( sLogger == "null" )
			g_pPlutoLogger = new NullLogger();
		else if( sLogger == "stdout" )
			g_pPlutoLogger = new FileLogger( stdout );
		else
			g_pPlutoLogger = new FileLogger( sLogger.c_str() );
	}
	catch( ... )
	{
		cerr << "Unable to create logger" << endl;
	}

	g_pPlutoLogger->Write( LV_STATUS, "Device: %d starting", dwPK_Device );

	try
	{
		Bluetooth_Dongle *pBluetooth_Dongle = new Bluetooth_Dongle( dwPK_Device, sRouter_IP );	
		if ( pBluetooth_Dongle->Connect() ) 
		{
			g_pPlutoLogger->Write( LV_STATUS, "Connect OK" );
			pBluetooth_Dongle->CreateChildren();
			pthread_join( pBluetooth_Dongle->m_RequestHandlerThread, NULL ); // running the separate thread

		} 
		else 
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Connect() Failed" );
		}

		delete pBluetooth_Dongle;
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
 WSACleanup();
#endif
 return 0;
}
//<-dceag-main-e->
