#include "StartOrbiter.h"

#include "PlutoUtils/CommonIncludes.h"	
#include "Simulator.h"
#include "Win32/OrbiterWin32Defs.h"

using namespace DCE;
using namespace std;

#include <string>
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "MainDialog.h"
//-----------------------------------------------------------------------------------------------------
extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
//-----------------------------------------------------------------------------------------------------
void DeadlockHandler(PlutoLock *pPlutoLock)
{
	// This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{   
        ::MessageBox(NULL, TEXT("Deadlock problem.  Going to reload and quit"), TEXT("Orbiter"), MB_OK);

		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Deadlock problem.  %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
		g_pCommand_Impl->OnReload();
	}
}
//-----------------------------------------------------------------------------------------------------
void SocketCrashHandler(Socket *pSocket)
{
	// This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{
        ::MessageBox(NULL, TEXT("Socket problem.  Going to reload and quit"), TEXT("Orbiter"), MB_OK);

		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Socket problem. %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
		g_pCommand_Impl->OnReload();
	}
}
//-----------------------------------------------------------------------------------------------------
enum OrbiterStages
{
	osConnect,
	osErrorReconnect,
	osRun,
	osQuit
};
//-----------------------------------------------------------------------------------------------------
bool EventLoop(ORBITER_CLASS* pOrbiter);
//-----------------------------------------------------------------------------------------------------

ORBITER_CLASS *Connect(int &PK_Device,int PK_DeviceTemplate, string sRouter_IP,string sLocalDirectory,bool bLocalMode, 
	int Width,int Height, bool bFullScreen, bool bUseOpenGL, bool &bMustQuit)
{
    bMustQuit = false;

	if(!bLocalMode)
		WriteStatusOutput("Connecting to DCERouter...");

	try
	{
		g_pPlutoLogger->Write(LV_STATUS, "About to cleanup Orbiter");
		ORBITER_CLASS::Cleanup();
		g_pPlutoLogger->Write(LV_STATUS, "Orbiter cleanup finished");

		WriteStatusOutput("Building a new orbiter");
		ORBITER_CLASS::BuildOrbiter(
			PK_Device, PK_DeviceTemplate, sRouter_IP,
			sLocalDirectory, bLocalMode, 
			Width, Height, bFullScreen, 
			bUseOpenGL
		); //the builder method
		g_pPlutoLogger->Write(LV_STATUS, "New orbiter created!");
	}
	catch(string s)
	{
        WriteStatusOutput(("ERROR: " + s).c_str());
		g_pPlutoLogger->Write(LV_STATUS,s.c_str());
		return NULL;
	}
	catch(const char *s)
	{
        WriteStatusOutput((string("ERROR: ") + s).c_str());
		g_pPlutoLogger->Write(LV_STATUS, s);
		return NULL;
	}

	ORBITER_CLASS *pOrbiter = ORBITER_CLASS::GetInstance();

	if(!bLocalMode)
	{
		WriteStatusOutput("Getting config and connecting...");
		bool bConnected = pOrbiter->GetConfig() && pOrbiter->Connect(0);  // Don't validate the device template, since the same binary is used for lots of devices
		PK_Device = pOrbiter->m_dwPK_Device;
        if(!bConnected)
        {
            if(pOrbiter->m_bQuit)
                bMustQuit = true;
            else
            {
                ORBITER_CLASS::Cleanup();
                return NULL;
            }
        }
	}

	return pOrbiter;
}
//-----------------------------------------------------------------------------------------------------
bool Run(ORBITER_CLASS* pOrbiter, bool bLocalMode)
{
	WriteStatusOutput("Parsing configuration data...");
	pOrbiter->WriteStatusOutput("Parsing configuration data...");

#ifndef POCKETFROG
    pOrbiter->Initialize(gtSDLGraphic);

	if(pOrbiter->m_bQuit)
		return false;

	if(pOrbiter->m_bReload)
		return false;

    if (!bLocalMode)
        pOrbiter->CreateChildren();

    g_pPlutoLogger->Write(LV_STATUS, "Starting processing events");

	WriteStatusOutput("Starting processing events...");
	pOrbiter->WriteStatusOutput("Starting processing events...");
#endif

	return EventLoop(pOrbiter);
}
//-----------------------------------------------------------------------------------------------------
bool EventLoop(ORBITER_CLASS* pOrbiter)
{
	if(Simulator::GetInstance()->m_bEnableGenerator)
		Simulator::GetInstance()->StartRandomEventGenerator();

#ifdef POCKETFROG
	pOrbiter->Run();
#else

	int SDL_Event_Pending = 0;

	SDL_Event Event;

   // For now I'll assume that shift + arrows scrolls a grid
    while (!pOrbiter->m_bQuit && !pOrbiter->m_bReload)
    {
		SDL_Event_Pending = SDL_PollEvent(&Event);

		if (SDL_Event_Pending)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Event: %d", Event.type);

			Orbiter::Event orbiterEvent;
			orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;

			if (Event.type == SDL_QUIT)
			{
				g_pPlutoLogger->Write(LV_WARNING, "Received sdl event SDL_QUIT");
				break;
			}

			if(Event.type == SDL_MOUSEMOTION)
			{
				orbiterEvent.type = Orbiter::Event::MOUSE_MOVE;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				pOrbiter->ProcessEvent(orbiterEvent);
			}

#ifdef AUDIDEMO
			if (Event.type == SDL_MOUSEBUTTONDOWN)
			{
				g_pPlutoLogger->Write(LV_WARNING, "================================= Mouse button pressed %d", Event.button.button);

				orbiterEvent.type = Orbiter::Event::BUTTON_DOWN;
				switch ( Event.button.button )
				{
					case 1:	orbiterEvent.data.button.m_iPK_Button = BUTTON_4_CONST; 			break;
					case 2: orbiterEvent.data.button.m_iPK_Button = BUTTON_Enter_CONST; 		break;
					case 3:	orbiterEvent.data.button.m_iPK_Button = BUTTON_5_CONST; 			break;
					case 4:	orbiterEvent.data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST; 		break;
					case 5:	orbiterEvent.data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST; 	break;
					case 6:	orbiterEvent.data.button.m_iPK_Button = BUTTON_2_CONST; 			break;
					case 7:	orbiterEvent.data.button.m_iPK_Button = BUTTON_1_CONST; 			break;

					default:
						g_pPlutoLogger->Write(LV_WARNING, "========================================== Mouse button not handled!");
						orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;
						break;
				}
				pOrbiter->ProcessEvent(orbiterEvent);
			}
	#else
			if (Event.type == SDL_MOUSEBUTTONDOWN)
			{
				orbiterEvent.type = Orbiter::Event::REGION_DOWN;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				pOrbiter->ProcessEvent(orbiterEvent);

				RecordMouseAction(Event.button.x, Event.button.y);
			}
			else if (Event.type == SDL_MOUSEBUTTONUP)
			{
				orbiterEvent.type = Orbiter::Event::REGION_UP;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				pOrbiter->ProcessEvent(orbiterEvent);
			}
	#endif
		} //if (SDL_Event_Pending)
		else
		{
			#ifdef ORBITER_OPENGL
					((Orbiter_OpenGL*) pOrbiter)->OnIdle();
			#else
        			((OrbiterSDL*) pOrbiter)->OnIdle();
			#endif
		}

    }  // while

#endif //POCKETFROG vs. SDL

	g_pPlutoLogger->Write(LV_STATUS, "About to quit EventLoop. Reload %d, ConnectionLost %d, Quit %d", 
		(int) pOrbiter->m_bReload, (int) pOrbiter->m_bConnectionLost, (int) pOrbiter->m_bQuit);

	return !pOrbiter->m_bReload;
}
//-----------------------------------------------------------------------------------------------------
void StartOrbiter(int PK_Device,int PK_DeviceTemplate,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
					int Width,int Height, bool bFullScreen, bool bUseOpenGL)
{
	ORBITER_CLASS *pOrbiter = NULL;
	OrbiterStages stage = osConnect;

	g_pDeadlockHandler=DeadlockHandler;
	g_pSocketCrashHandler=SocketCrashHandler; 

	while(stage != osQuit)
	{
		switch(stage)
		{
			case osConnect:
                {
					WriteStatusOutput("Stage connect");
				    g_pPlutoLogger->Write(LV_STATUS, "Stage connect");
                    bool bMustQuit = false;
				    pOrbiter = Connect(PK_Device, PK_DeviceTemplate, sRouter_IP, sLocalDirectory, bLocalMode, 
						Width, Height, bFullScreen, bUseOpenGL, bMustQuit);

				    if(pOrbiter != NULL && !bMustQuit)
				    {
			            g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
					    stage = osRun;

					    g_pPlutoLogger->Write( LV_STATUS, "Orbiter SelfUpdate: starting" );
					    if(pOrbiter->SelfUpdate())
					    {
						    g_pPlutoLogger->Write( LV_STATUS, "Orbiter SelfUpdate: need to close orbiter" );
						    stage = osQuit;
					    }
				    }
				    else if(!bMustQuit)
					    stage = osErrorReconnect;
                    else
                        stage = osQuit;
                }
				break;

			case osRun:
				g_pPlutoLogger->Write(LV_STATUS, "Stage run");
				if(Run(pOrbiter, bLocalMode))
					stage = osQuit; 
				else
					stage = osErrorReconnect;
				break;

			case osErrorReconnect:
//				ShowPopup("Pluto Orbiter", "Failed to connect to DCERouter. Trying to reconnect...");

				WriteStatusOutput("Failed to connect to DCERouter. I'll try again in 2 seconds...");
				Sleep(2000);
				stage = osConnect;
				break; 
		}
	}

	g_pPlutoLogger->Write(LV_STATUS, "Stage is now osQuit.");	
    
	pOrbiter = NULL;
}
//-----------------------------------------------------------------------------------------------------
