// Orbiter.cpp : Defines the entry point for the application.
//

#ifdef WINCE
#include "stdafx.h"
#include <commctrl.h>
#endif

#include "main.h"
#include "MainDialog.h"
#include "DCE/Logger.h"
#include "Simulator.h"

#define  VERSION "<=version=>"
const char *g_szCompile_Date="<=compile_date=>";
/*SVN_REVISION*/

extern HWND	g_hWndList; //maindialog logger list

#ifdef WINCE
	#include <aygshell.h>                      // Add Pocket PC includes
	#pragma comment( lib, "aygshell" )         // Link Pocket PC library
	
	#ifdef POCKETFROG
		#include "Orbiter_PocketFrog.h"
	#else
		#include "OrbiterSDL_WinCE.h"
	#endif
#else
#include "OrbiterSDL_Win32.h"
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
	g_sBinary = "OrbiterWin.exe";
	g_sBinaryPath = "";

	string sRouter_IP="dcerouter";
    int PK_Device=0;
	string sLogger="orbiter";

    int Width=800,Height=600;
    bool bLocalMode=false; // If true, it will not connect to PlutoServer but will look for it's files in the local directory
	bool bFullScreen=false;
    string sLocalDirectory="";
    string sNestedDisplay = "";

	bool bDeviceIDSpecified = false;
	bool bRouterIPSpecified = false;

    bool bError=false; // An error parsing the command line

#ifdef WINCE
	bFullScreen = true; //full-screen by default
	sLogger = "/Storage card/Orbiter.log";
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

		try
		{
			if( sLogger=="null" )
				g_pPlutoLogger = new NullLogger();
			else 
				if( sLogger == "orbiter" )
					g_pPlutoLogger = new WinOrbiterLogger(g_hWndList);
				else
					g_pPlutoLogger = new FileLogger(sLogger.c_str());
		}
		catch(...)
		{
			printf("Unable to create logger\n");
			return -1;
		}

#ifdef WINCE
		Simulator::GetInstance()->LoadConfigurationFile("/Storage Card/Orbiter.conf");
#else
		Simulator::GetInstance()->LoadConfigurationFile("Orbiter.conf");
#endif

		if(!CmdLineParams.bDeviceIDSpecified)
			if(!Simulator::GetInstance()->m_bTryToDetermineAutomatically)
			{
				try
				{
					CmdLineParams.PK_Device = atoi(Simulator::GetInstance()->m_sDeviceID.c_str());
				}
				catch(...)
				{
				}
			}

		if(!CmdLineParams.bRouterIPSpecified)
			if(!Simulator::GetInstance()->m_bTryToDetermineAutomatically)
				CmdLineParams.sRouter_IP = Simulator::GetInstance()->m_sRouterIP;

		g_pPlutoLogger->Write(LV_STATUS, "Device: %d starting.  Connecting to: %s",CmdLineParams.PK_Device,
			CmdLineParams.sRouter_IP.c_str());

		//now it's safe to start orbiter's thread
		StartOrbiterThread();

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		g_pPlutoLogger->Write(LV_STATUS, "Exited process messages loop. We are shutting down....");

#ifdef WINCE
		Simulator::GetInstance()->SaveConfigurationFile("/Storage Card/Orbiter.conf");
		Simulator::Cleanup();

		g_pPlutoLogger->Write(LV_STATUS, "About to cleanup orbiter.");

		#ifdef POCKETFROG
				Orbiter_PocketFrog::Cleanup();
		#else
				OrbiterSDL_WinCE::Cleanup();
		#endif

#else
		Simulator::GetInstance()->SaveConfigurationFile("Orbiter.conf");
		Simulator::Cleanup();

		g_pPlutoLogger->Write(LV_STATUS, "About to cleanup 2");
		OrbiterSDL_Win32::Cleanup();
#endif

		g_pPlutoLogger->Write(LV_STATUS, "About to delete logger. Logger out.");
		delete g_pPlutoLogger;

		return int(msg.wParam);
	}


    WSACleanup();

	return 0;
}

