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

	bool bFailed = false;

	try
	{
		OrbiterSDL_WinCE::Cleanup();
		OrbiterSDL_WinCE::BuildOrbiterSDL_WinCE(
			PK_Device, sRouter_IP,
			sLocalDirectory, bLocalMode, 
			Width, Height, bFullScreen
		); //the builder method
	}
	catch(...)
	{
		bFailed = true;
	}

	OrbiterSDL_WinCE *pOrbiter = NULL;
	
	if(!bFailed)
	{
		pOrbiter = OrbiterSDL_WinCE::GetInstance();

		if(!bLocalMode)
		{
			bool bConnected = pOrbiter->Connect();

			if(!bConnected)
			{
				delete pOrbiter;
				pOrbiter = NULL;
			}

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

	bool bResult = false;

	if(!pOrbiter->m_bReload)
	{
		if (!bLocalMode)
			pOrbiter->CreateChildren();

		g_pPlutoLogger->Write(LV_STATUS, "Starting processing events");

		WriteStatusOutput("Starting processing events...");
		pOrbiter->WriteStatusOutput("Starting processing events...");

		bResult = SDLEventLoop(pOrbiter);
	}

	return bResult;
}
//-----------------------------------------------------------------------------------------------------
bool SDLEventLoop(OrbiterSDL_WinCE* pOrbiter)
{
    SDL_Event Event;

    // temporary hack --
    // have to figure out what should be the default behavior of the arrows, moving the highlighted object, or scrolling a grid
    // For now I'll assume that shift + arrows scrolls a grid
    bool bShiftDown=false,bControlDown=false,bAltDown=false,bRepeat=false,bCapsLock=false;
    clock_t cKeyDown=0;
    while (!pOrbiter->m_bQuit && !pOrbiter->m_bReload)
    {
        SDL_WaitEvent(&Event);

        if (Event.type == SDL_QUIT)
            break;
#ifdef AUDIDEMO
        if (Event.type == SDL_MOUSEBUTTONDOWN)
        {
            g_pPlutoLogger->Write(LV_WARNING, "================================= Mouse button pressed %d", Event.button.button);
            if( Event.button.button==4 )
                pOrbiter->ButtonDown(BUTTON_Up_Arrow_CONST);
            else if( Event.button.button==5 )
                pOrbiter->ButtonDown(BUTTON_Down_Arrow_CONST);
            else if( Event.button.button==2 )
                pOrbiter->ButtonDown(BUTTON_Enter_CONST);
            else if( Event.button.button==1 )
                pOrbiter->ButtonDown(BUTTON_4_CONST);
            else if( Event.button.button==3 )
                pOrbiter->ButtonDown(BUTTON_5_CONST);
            else if( Event.button.button==7 )
                pOrbiter->ButtonDown(BUTTON_1_CONST);
            else if( Event.button.button==6 )
                pOrbiter->ButtonDown(BUTTON_2_CONST);
            else
                g_pPlutoLogger->Write(LV_WARNING, "========================================== Mouse button not handled!");
        }
#else
        if (Event.type == SDL_MOUSEBUTTONDOWN)
		{
            pOrbiter->RegionDown(Event.button.x, Event.button.y);
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
						g_pPlutoLogger->Write( LV_STATUS, "SOrbiter SelfUpdate: need to close orbiter" );
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
