/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 See the GNU General Public License for more details.
*/

//<-dceag-incl-b->
#include "Disk_Drive.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCERouter.h"

// In source files stored in archives and packages, these 2 lines will have the release version (build)
// and the svn revision as a global variable that can be inspected within a core dump
#define  VERSION "<=version=>"
const char *g_szCompile_Date="<=compile_date=>";
/*SVN_REVISION*/

namespace DCE
{
	Logger *g_pPlutoLogger;
}
using namespace DCE;

// You can override this block if you don't want the app to reload in the event of a problem
extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
void DeadlockHandler(PlutoLock *pPlutoLock)
{
	// This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Deadlock problem.  Going to reload and quit");
		g_pCommand_Impl->OnReload();
	}
}
void SocketCrashHandler(Socket *pSocket)
{
	// This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Socket problem.  Going to reload and quit");
		g_pCommand_Impl->OnReload();
	}
}
void Plugin_DeadlockHandler(PlutoLock *pPlutoLock)
{
	// This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
	if( g_pCommand_Impl && g_pCommand_Impl->m_pRouter )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Plugin Deadlock problem.  Going to reload");
		g_pCommand_Impl->m_pRouter->CrashWithinPlugin(g_pCommand_Impl->m_dwPK_Device);
	}
}
void Plugin_SocketCrashHandler(Socket *pSocket)
{
	// This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
	if( g_pCommand_Impl && g_pCommand_Impl->m_pRouter )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Plugin Deadlock problem.  Going to reload");
		g_pCommand_Impl->m_pRouter->CrashWithinPlugin(g_pCommand_Impl->m_dwPK_Device);
	}
}
//<-dceag-incl-e->


//<-dceag-plug-b->
extern "C" {
	class Command_Impl *RegisterAsPlugIn(class Router *pRouter,int PK_Device,Logger *pPlutoLogger)
	{
		g_pPlutoLogger = pPlutoLogger;
		g_pPlutoLogger->Write(LV_STATUS, "Device: %d loaded as plug-in",PK_Device);

		Disk_Drive *pDisk_Drive = new Disk_Drive(PK_Device, "localhost",true,false,pRouter);
		if( pDisk_Drive->m_bQuit )
		{
			delete pDisk_Drive;
			return NULL;
		}
		else
		{
			g_pCommand_Impl=pDisk_Drive;
			g_pDeadlockHandler=Plugin_DeadlockHandler;
			g_pSocketCrashHandler=Plugin_SocketCrashHandler;
		}
		return pDisk_Drive;
	}
}
//<-dceag-plug-e->

//<-dceag-main-b->! Added loop call to watch the drive
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
		<< " See the GNU General Public License for more details. "<< endl << endl<< endl << endl << endl;

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

//                      123456789012345678901234567890123456789012345678901234567890
		cout << "Disk_Drive, v." << VERSION << endl
			<< "A Pluto DCE Device. See www.plutohome.com/dce for details." << endl
			<< "Usage: MythTV_Player [-r Router's IP] [-d My Device ID] " << endl
			<< " [-l dcerouter|stdout|null|filename]" << endl
			<< "-r router's IP	-- the IP address of the DCE Router " << endl
			<< "			 Defaults to 'dcerouter'." << endl
			<< "-d my device ID	-- This device's ID number. " << endl
			<< "			If not specified, it will be requested " << endl
			<< "			from the router based on our IP address." << endl
			<< "-l dcerouter|stdout|filename	-- Where to save the log files. " << endl
			<< "	Specify 'dcerouter' to have the messages logged to the DCE Router." << endl
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
        if( sLogger=="dcerouter" )
            g_pPlutoLogger = new ServerLogger(PK_Device, Disk_Drive::PK_DeviceTemplate_get_static(), sRouter_IP);
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
        Disk_Drive *pDisk_Drive = new Disk_Drive(PK_Device, sRouter_IP);
        if ( pDisk_Drive->Connect(pDisk_Drive->PK_DeviceTemplate_get()) )
        {
            g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
            pDisk_Drive->CreateChildren();
            pDisk_Drive->RunMonitorLoop();
            pthread_join(pDisk_Drive->m_RequestHandlerThread, NULL);
        }
        else
        {
            g_pPlutoLogger->Write(LV_CRITICAL, "Connect() Failed");
        }

        delete pDisk_Drive;
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
