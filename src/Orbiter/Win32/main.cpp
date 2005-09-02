// Orbiter.cpp : Defines the entry point for the application.
//

#ifdef WINCE
#include "stdafx.h"
#include <commctrl.h>
#endif

#include "main.h"
#include "MainDialog.h"

#include "DCE/Logger.h"
#include "DCE/ServerLogger.h"

#include "Simulator.h"

#define  VERSION "<=version=>"
const char *g_szCompile_Date="<=compile_date=>";
/*SVN_REVISION*/

extern HWND	g_hWndList; //maindialog logger list

#ifdef WINCE
    static const string g_sOrbiterConfName = "Orbiter.txt";
#else
    static const string g_sOrbiterConfName = "Orbiter.conf";
#endif

#ifdef POCKETFROG
	#include "Orbiter_PocketFrog.h"
#else
	#ifdef WINCE
		#include "OrbiterSDL_WinCE.h"
	#else
		#include "OrbiterSDL_Win32.h"
	#endif
#endif

namespace DCE
{
	Logger *g_pPlutoLogger = NULL;
}

using namespace DCE; 

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	//get the binary name and the binary path
	char pFullPath[256];
#ifdef WINCE
	wchar_t pProcessNameW[256];
	::GetModuleFileName(NULL, pProcessNameW, sizeof(pProcessNameW));
	wcstombs(pFullPath, pProcessNameW, 256);
#else
    ::GetModuleFileName(NULL, pFullPath, 256);
#endif
	g_sBinary = FileUtils::FilenameWithoutPath(pFullPath);
	g_sBinaryPath = FileUtils::BasePath(pFullPath) + "/";

	string sRouter_IP="dcerouter";
    int PK_Device=0,PK_DeviceTemplate=0;
	string sLogger="orbiter.log";

    int Width=800,Height=600;

    bool bLocalMode=false; // If true, it will not connect to PlutoServer but will look for it's files in the local directory
	bool bFullScreen=false;
    string sLocalDirectory="";
    string sNestedDisplay = "";

	bool bDeviceIDSpecified = false;
	bool bRouterIPSpecified = false;
	bool bFullScreenSpecified = false;

    bool bError=false; // An error parsing the command line

#ifdef WINCE
	bFullScreen = true; //full-screen by default
	sLogger = g_sBinaryPath + "Orbiter.log";
#endif

	//parse command line
	string::size_type pos = 0;

#ifdef WINCE
	char pCmdLine[256];
	wcstombs(pCmdLine, lpCmdLine, 256);
	#define CMD_LINE pCmdLine
#else
	#define CMD_LINE lpCmdLine
#endif

	string command_line = CMD_LINE;
	string token;
	do
	{
		token = StringUtils::Tokenize(command_line, " ", pos);

		if(token != "" && token.size() >= 2)
			switch(token[1])
			{	
				case 'r':
					sRouter_IP = StringUtils::Tokenize(command_line, " ", pos);
					bRouterIPSpecified = true;
					break;
				case 'd':
					PK_Device = atoi(StringUtils::Tokenize(command_line, " ", pos).c_str());
					bDeviceIDSpecified = true;
					break;
				case 'T':
					PK_DeviceTemplate = atoi(StringUtils::Tokenize(command_line, " ", pos).c_str());
					bDeviceIDSpecified = true;
					break;
				case 'l':
					//sLogger = StringUtils::Tokenize(command_line, " ", pos);
					{
						if(command_line[pos] == '\"')
						{
							pos++;
							sLogger = StringUtils::Tokenize(command_line, "\"", pos);
							pos++;
						}
						else
							sLogger = StringUtils::Tokenize(command_line, " ", pos);
					}
					break;
				case 'D':
					sLocalDirectory = StringUtils::Tokenize(command_line, " ", pos);
					break;
				case 'L':
					bLocalMode = true;
					break;
				case 'W':
					Width = atoi(StringUtils::Tokenize(command_line, " ", pos).c_str());
					break;
				case 'H':
					Height = atoi(StringUtils::Tokenize(command_line, " ", pos).c_str());
					break;
				case 'F':
					bFullScreenSpecified = true;
					bFullScreen = true;
					break;
				default:
					bError=true;
					break;
		}
	}
	while(pos<command_line.length());

    if (bError)
    {
		printf("Orbiter, v. %s\n", VERSION);
		printf("A Pluto DCE Device.  See www.plutohome.com/dce for details.\n");
        printf("Usage: Orbiter [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]\n"); 
	    printf("[-D Directory] [-L] [-W Width] [-H Height]\n");
        printf("-r router's IP	-- the IP address of the DCE Router  Defaults to 'dcerouter'.\n");
        printf("-d my device ID	-- This device's ID number.  If not specified, it will be requested\n");
	    printf("	               from the router based on our IP address.\n");
        printf("-l dcerouter	-- Where to save the log files.  Specify 'dcerouter' to have \n"); 
	    printf("	               the messages logged to the DCE Router.  Defaults to stdout.\n");
        printf("-D directory	-- If a directory is specified, it will look for it's image \n");
	    printf("	               and config files here rather than requesting from the server.\n");
        printf("-L              -- Local mode only.  Do not connect to the server.  All messages will just loop back.\n");
        printf("-F              -- Full screen.\n");
		printf("-W/H            -- Width/Height default to full screen.\n");

    }
	else
	{
		if( sLocalDirectory.length()>0 && sLocalDirectory[ sLocalDirectory.length()-1 ]!='/' )
			sLocalDirectory += "/";

		WORD    wVersion;
		WSADATA wsaData;
		wVersion = MAKEWORD( 1, 1 );
		if (WSAStartup(wVersion, &wsaData)!=0)
		{
			int ec = WSAGetLastError();
			printf("WSAStartup err %d", ec);
			return 0;
		}

		MSG msg;

		CmdLineParams.sRouter_IP		= sRouter_IP;
		CmdLineParams.PK_Device			= PK_Device;
		CmdLineParams.PK_DeviceTemplate	= PK_DeviceTemplate;
		CmdLineParams.sLogger			= sLogger;
		CmdLineParams.Width				= Width;
		CmdLineParams.Height			= Height;
		CmdLineParams.bLocalMode		= bLocalMode; 
		CmdLineParams.bFullScreen		= bFullScreen;
		CmdLineParams.sLocalDirectory	= sLocalDirectory;
		CmdLineParams.sNestedDisplay    = sNestedDisplay;
		CmdLineParams.bDeviceIDSpecified = bDeviceIDSpecified;
		CmdLineParams.bRouterIPSpecified = bRouterIPSpecified;

		// Perform application initialization:
		if (!InitInstance (hInstance, nCmdShow)) 
		{
			return FALSE;
		}

		//NOTE: the logger is not created yet. Do not try to use the logger at this moment!
		Simulator::GetInstance()->LoadConfigurationFile(g_sBinaryPath + g_sOrbiterConfName);

		if(!CmdLineParams.bDeviceIDSpecified)
			if(!(Simulator::GetInstance()->m_bTryToDetermineAutomatically))
				CmdLineParams.PK_Device = atoi(Simulator::GetInstance()->m_sDeviceID.c_str());

		if(!CmdLineParams.bRouterIPSpecified)
			if(!(Simulator::GetInstance()->m_bTryToDetermineAutomatically))
				CmdLineParams.sRouter_IP = Simulator::GetInstance()->m_sRouterIP;

		if(!bFullScreenSpecified)
			CmdLineParams.bFullScreen = Simulator::GetInstance()->m_bFullScreen;

		//Creating the logger
		if(Simulator::GetInstance()->m_bLogToServer)
			g_pPlutoLogger = new ServerLogger(CmdLineParams.PK_Device, 0, CmdLineParams.sRouter_IP.c_str());
		else
		{
			try
			{
				if( sLogger=="null" )
					g_pPlutoLogger = new NullLogger();
				else 
					if( sLogger == "orbiter" )
						g_pPlutoLogger = new WinOrbiterLogger(g_hWndList);
					else
                    {
#ifndef DEBUG    
                        #ifdef WINCE
                            wchar_t LoggerFileNameW[256];
                            mbstowcs(LoggerFileNameW, sLogger.c_str(), 256);
                            ::DeleteFile(LoggerFileNameW);
                        #else
                            ::DeleteFile(sLogger.c_str());
                        #endif		
#endif
						g_pPlutoLogger = new FileLogger(sLogger.c_str());
                    }
			}
			catch(...)
			{
				printf("Unable to create logger\n");
				return -1;
			}
		}

		//It's safe now to use the logger
		g_pPlutoLogger->Write(LV_STATUS, "Device: %d starting.  Connecting to: %s",CmdLineParams.PK_Device,
			CmdLineParams.sRouter_IP.c_str());

		//Starting orbiter's thread
		StartOrbiterThread();

		SyncConfigurationData();
		LoadUI_From_ConfigurationData();

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

#ifdef POCKETFROG
        //if the simulator is still running
        //we want to shutdown pocketfrog gracefully, giving it time to process shutdown messages first.
        //1 sec should be enough

        if(Simulator::GetInstance()->IsRunning())
        {
            Orbiter_PocketFrog::GetInstance()->OnQuit();

            time_t tTime = time(NULL);
            while(true)
            {
                if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                if( tTime + 1 < time(NULL) )
                    break;
            }
        }
#endif        

		g_pPlutoLogger->Write(LV_STATUS, "Exited process messages loop. We are shutting down....");

		Simulator::GetInstance()->SaveConfigurationFile(g_sBinaryPath + g_sOrbiterConfName);
		Simulator::Cleanup();

		#ifdef POCKETFROG
				Orbiter_PocketFrog::Cleanup();
		#else
            #ifdef WINCE
                OrbiterSDL_WinCE::Cleanup();
            #else
                OrbiterSDL_Win32::Cleanup();
            #endif
		#endif

#ifdef WINCE
		HWND hTaskBarWindow = ::FindWindow(TEXT("HHTaskBar"), NULL);
		::ShowWindow(hTaskBarWindow, SW_SHOWNORMAL);
#endif

		g_pPlutoLogger->Write(LV_STATUS, "About to delete logger. Logger out.");
		delete g_pPlutoLogger;

		return int(msg.wParam);
	}


    WSACleanup();
	return 0;
}

