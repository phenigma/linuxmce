#include "StartOrbiterCE.h"

#include "PlutoUtils/CommonIncludes.h"	
#include "OrbiterSDL_WinCE.h"

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
enum OrbiterStages
{
	osConnect,
	osErrorReconnect,
	osRun,
	osQuit
};
//-----------------------------------------------------------------------------------------------------
bool SDLEventLoop(OrbiterSDL_WinCE* pOrbiter);
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_WinCE* Connect(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
					int Width,int Height, bool bFullScreen)
{
	if(!bLocalMode)
		WriteStatusOutput("Connecting to DCERouter...");

	try
	{
		g_pPlutoLogger->Write(LV_STATUS, "About to cleanup Orbiter");
		
		OrbiterSDL_WinCE::Cleanup();
	
		g_pPlutoLogger->Write(LV_STATUS, "Orbiter cleanup finished");

		OrbiterSDL_WinCE::BuildOrbiterSDL_WinCE(
			PK_Device, sRouter_IP,
			sLocalDirectory, bLocalMode, 
			Width, Height, bFullScreen
		); //the builder method
		g_pPlutoLogger->Write(LV_STATUS, "New orbiter created!");
	}
	catch(string s)
	{
		g_pPlutoLogger->Write(LV_STATUS,s.c_str());
		return NULL;
	}
	catch(const char *s)
	{
		g_pPlutoLogger->Write(LV_STATUS, s);
		return NULL;
	}
	catch(...)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Unknown exception!!");
		return NULL;
	}

	OrbiterSDL_WinCE *pOrbiter = OrbiterSDL_WinCE::GetInstance();

	if(!bLocalMode)
	{
		bool bConnected = pOrbiter->Connect(0);  // Don't validate the device template, since the same binary is used for lots of devices

		if(!bConnected)
		{
			g_pPlutoLogger->Write(LV_STATUS, "ready to delete instance 2");
			delete pOrbiter;
			pOrbiter = NULL;
		}

	}

	return pOrbiter;
}
//-----------------------------------------------------------------------------------------------------
bool Run(OrbiterSDL_WinCE* pOrbiter, bool bLocalMode)
{
	WriteStatusOutput("Parsing configuration data...");
	pOrbiter->WriteStatusOutput("Parsing configuration data...");

    pOrbiter->Initialize(gtSDLGraphic);

	if(pOrbiter->m_bQuit)
		return true;

	if(pOrbiter->m_bReload)
		return true;

    if (!bLocalMode)
        pOrbiter->CreateChildren();

    g_pPlutoLogger->Write(LV_STATUS, "Starting processing events");

	WriteStatusOutput("Starting processing events...");
	pOrbiter->WriteStatusOutput("Starting processing events...");

	return SDLEventLoop(pOrbiter);
}
//-----------------------------------------------------------------------------------------------------
bool SDLEventLoop(OrbiterSDL_WinCE* pOrbiter)
{
    SDL_Event Event;

    // temporary hack --
    // have to figure out what should be the default behavior of the arrows, moving the highlighted object, or scrolling a grid
    // For now I'll assume that shift + arrows scrolls a grid
    while (!pOrbiter->m_bQuit && !pOrbiter->m_bReload)
    {
		try
		{
			SDL_WaitEvent(&Event);
		}
		catch(...) {}

		Orbiter::Event orbiterEvent;
		orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;

        if (Event.type == SDL_QUIT)
            break;

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
#endif
    }  // while

	return !pOrbiter->m_bConnectionLost;
}
//-----------------------------------------------------------------------------------------------------
void StartOrbiterCE(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
					int Width,int Height, bool bFullScreen)
{
	OrbiterSDL_WinCE *pOrbiter = NULL;
	OrbiterStages stage = osConnect;

	while(stage != osQuit)
	{
		switch(stage)
		{
			case osConnect:
				pOrbiter = Connect(PK_Device, sRouter_IP, sLocalDirectory, bLocalMode, Width, Height, bFullScreen);

				if(pOrbiter != NULL)
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
				else
					stage = osErrorReconnect;
				break;

			case osRun:
				if(Run(pOrbiter, bLocalMode))
					stage = osQuit; 
				else
					stage = osErrorReconnect;
				break;

			case osErrorReconnect:
				WriteStatusOutput("Failed to connect to DCERouter. I'll try again in 2 seconds...");
				Sleep(2000);
				stage = osConnect;
				break; //not implemented yet
		}
	}
    
	OrbiterSDL_WinCE::Cleanup();

	pOrbiter = NULL;
}
//-----------------------------------------------------------------------------------------------------
