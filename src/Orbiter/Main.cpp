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
 
//<-dceag-incl-b->!
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

// In source files stored in archives and packages, these 2 lines will have the release version (build)
// and the svn revision as a global variable that can be inspected within a core dump
#define  VERSION "<=version=>"
/*SVN_REVISION*/

namespace DCE
{
	Logger *g_pPlutoLogger;
}
using namespace DCE;
//<-dceag-incl-e->

// For whatever reason, if I put: #include "OrbiterImageSDL.h" in this file,
// then Microsoft won't compile, reporting an unresolved symbol _main.  It is something
// to do with SDL.  Moving those references to another cpp file solves the problems.  See StartOrbiterSDL.cpp
bool StartOrbiter(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
				  int Width,int Height, bool bFullScreen);



//<-dceag-plug-b->! never a plug-in

//<-dceag-main-b->!  **DON'T AUTOMATICALLY OVERWRITE THIS SECTIONS, IT'S CUSTOM
int main(int argc, char* argv[])
{
	g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
	g_sBinaryPath = FileUtils::BasePath(argv[0]);

	cout<<"Copyright (C) 2004 Pluto, Inc., a Florida Corporation"<<endl
		<<"www.plutohome.com"<<endl
		<<"Phone: +1 (877) 758-8648"<<endl
		<<"This program is distributed according to the terms of the Pluto Public License, available at: "<<endl
		<<"http://plutohome.com/index.php?section=public_license "<<endl
		<<"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "<<endl
		<<"without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "<<endl
		<<"See the Pluto Public License for more details."<<endl << "-----" << endl << endl;    
    
    string sRouter_IP="dcerouter";
    int PK_Device=0;
    string sLogger="stdout";
    int Width=800,Height=600;
    bool bLocalMode=false; // If true, it will not connect to PlutoServer but will look for it's files in the local directory
	bool bFullScreen=false;
    string sLocalDirectory="";
    string sNestedDisplay = "";

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
        case 'D':
            sLocalDirectory = argv[++optnum];
            break;
        case 'L':
            bLocalMode = true;
            break;
        case 'W':
            Width = atoi(argv[++optnum]);
            break;
        case 'H':
            Height = atoi(argv[++optnum]);
            break;
		case 'F':
			bFullScreen = true;
			break;
        default:
            bError=true;
            break;
        };
    }

    if (bError)
    {
        
//                123456789012345678901234567890123456789012345678901234567890	
	cout << "Orbiter, v." << VERSION << endl
            << "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
            << "Usage: Orbiter [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]" << endl 
	    << "[-D Directory] [-L] [-W Width] [-H Height]" << endl
            << "-r router's IP		-- the IP address of the DCE Router  Defaults to 'dcerouter'." << endl
            << "-d my device ID		-- This device's ID number.  If not specified, it will be requested" << endl
	    << " 	from the router based on our IP address." << endl
            << "-l dcerouter		-- Where to save the log files.  Specify 'dce_router' to have " << endl 
	    << "	the messages logged to the DCE Router.  Defaults to stdout." << endl
            << "-D directory		-- If a directory is specified, it will look for it's image " << endl
	    << "	and config files here rather than requesting from the server." << endl
            << "-L 			-- Local mode only.  Do not connect to the server.  All messages will just loop back." << endl
            << "-F			-- Full screen."
			<< "-W/H			-- Width/Height default to full screen." << endl;
			
        exit(0);
    }

	g_pPlutoLogger = NULL;

    try
    {
        if( sLogger=="dcerouter" )
            g_pPlutoLogger = new ServerLogger(PK_Device, Orbiter::PK_DeviceTemplate_get_static(), sRouter_IP);
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

    if( sLocalDirectory.length()>0 && sLocalDirectory[ sLocalDirectory.length()-1 ]!='/' )
        sLocalDirectory += "/";

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
		delete g_pPlutoLogger;
        exit(1);
    }
#endif

	bool bReload=false;
    try
    {
g_pPlutoLogger->Write(LV_STATUS, "about to call StartOrbiter");
        bReload = StartOrbiter(PK_Device,sRouter_IP,sLocalDirectory,bLocalMode,Width,Height,bFullScreen);
g_pPlutoLogger->Write(LV_STATUS, "StartOrbiter finished with reload: %s",(bReload ? "Y" : "N"));
    }
    catch(string s)
    {
        cerr << "Caught exception: " << s << endl;
        Sleep(2000);
		delete g_pPlutoLogger;
        exit(1);
    }
    catch(const char *s)
    {
        cerr << "Caught exception: " << s << endl;
        Sleep(2000);
		delete g_pPlutoLogger;
        exit(1);
    }

#ifdef WIN32
    WSACleanup();
#endif
g_pPlutoLogger->Write(LV_STATUS, "Orbiter ready to return and die");

	delete g_pPlutoLogger;

	if( bReload )
		return 2;
	else
	    return 0;
}
//<-dceag-main-e->
