//<-dceag-incl-b->
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#define  Orbiter_VERSION "testing"

namespace DCE
{
	Logger *g_pPlutoLogger;
}
using namespace DCE;
//<-dceag-incl-e->

// For whatever reason, if I put: #include "OrbiterImageSDL.h" in this file,
// then Microsoft won't compile, reporting an unresolved symbol _main.  It is something
// to do with SDL.  Moving those references to another cpp file solves the problems.  See StartOrbiterSDL.cpp
void StartOrbiter(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,int Width,int Height);



//<-dceag-plug-b->! never a plug-in

//<-dceag-main-b->!  **DON'T AUTOMATICALLY OVERWRITE THIS SECTIONS, IT'S CUSTOM
int main(int argc, char* argv[])
{
    string sRouter_IP="dce_router";
    int PK_Device=0;
    string sLogger="stdout";
    int Width=800,Height=600;
    bool bLocalMode=false; // If true, it will not connect to PlutoServer but will look for it's files in the local directory
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
        default:
            bError=true;
            break;
        };
    }

    if (bError)
    {
        cout << "Orbiter, v." << Orbiter_VERSION << endl
            << "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
            << "Usage: Orbiter [-r Router's IP] [-d My Device ID] [-l dce_router|stdout|null|filename] [-D Directory] [-L] [-W Width] [-H Height]" << endl
            << "-r -- the IP address of the DCE Router  Defaults to 'dce_router'." << endl
            << "-d -- This device's ID number.  If not specified, it will be requested from the router based on our IP address." << endl
            << "-l -- Where to save the log files.  Specify 'dce_router' to have the messages logged to the DCE Router.  Defaults to stdout." << endl
            << "-D -- If a directory is specified, it will look for it's image and config files here rather than requesting from the server." << endl
            << "-L -- Local mode only.  Do not connect to the server.  All messages will just loop back." << endl
            << "-W/H -- Width/Height default to full screen." << endl;
        exit(0);
    }

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
        exit(1);
    }
#endif

    try
    {
        StartOrbiter(PK_Device,sRouter_IP,sLocalDirectory,bLocalMode,Width,Height);
    }
    catch(string s)
    {
        cerr << "Caught exception: " << s << endl;
        Sleep(2000);
        exit(1);
    }
    catch(const char *s)
    {
        cerr << "Caught exception: " << s << endl;
        Sleep(2000);
        exit(1);
    }

#ifdef WIN32
    WSACleanup();
#endif

    return 0;
}
//<-dceag-main-e->
