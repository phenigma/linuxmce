#include "StartOrbiter_Win32.h"

#include "PlutoUtils/CommonIncludes.h"	

#ifdef WINCE
	#include "OrbiterSDL_Win32.h"
#else
	#include "OrbiterSDL_Win32.h"
#endif

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
bool SDLEventLoop(OrbiterSDL_Win32* pOrbiter);
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_Win32* Connect(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
					int Width,int Height, bool bFullScreen)
{
	if(!bLocalMode)
		WriteStatusOutput("Connecting to DCERouter...");

	try
	{
		OrbiterSDL_Win32::BuildOrbiterSDL_Win32(
			PK_Device, sRouter_IP,
			sLocalDirectory, bLocalMode, 
			Width, Height, bFullScreen
		); //the builder method
	}
	catch(...)
	{
		return NULL;
	}

	OrbiterSDL_Win32 *pOrbiter = OrbiterSDL_Win32::GetInstance();

	if(!bLocalMode)
	{
		bool bConnected = pOrbiter->Connect();

		if(!bConnected)
		{
			delete pOrbiter;
			pOrbiter = NULL;
		}

	}

	return pOrbiter;
}
//-----------------------------------------------------------------------------------------------------
bool Run(OrbiterSDL_Win32* pOrbiter, bool bLocalMode)
{
	WriteStatusOutput("Parsing configuration data...");
	pOrbiter->WriteStatusOutput("Parsing configuration data...");

    pOrbiter->Initialize(gtSDLGraphic);

	if(pOrbiter->m_bReload)
		return false;

    if (!bLocalMode)
        pOrbiter->CreateChildren();

    g_pPlutoLogger->Write(LV_STATUS, "Starting processing events");

	WriteStatusOutput("Starting processing events...");
	pOrbiter->WriteStatusOutput("Parsing configuration data...");

	return SDLEventLoop(pOrbiter);
}
//-----------------------------------------------------------------------------------------------------
bool SDLEventLoop(OrbiterSDL_Win32* pOrbiter)
{
    SDL_Event Event;

    // temporary hack --
    // have to figure out what should be the default behavior of the arrows, moving the highlighted object, or scrolling a grid
    // For now I'll assume that shift + arrows scrolls a grid
    bool bShiftDown=false,bControlDown=false,bAltDown=false,bRepeat=false,bCapsLock=false;
    clock_t cKeyDown=0;
    while (!pOrbiter->m_bQuitWinCE && !pOrbiter->m_bReload)
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

	return true; //all ok
}
//-----------------------------------------------------------------------------------------------------
void StartOrbiter_Win32(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
					int Width,int Height, bool bFullScreen)
{
	OrbiterSDL_Win32 *pOrbiter = NULL;
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
    
	//hack: if we'll try to delete pOrbiter, we'll stuck here
	pOrbiter->~OrbiterSDL_Win32(); 

	pOrbiter = NULL;
}
//-----------------------------------------------------------------------------------------------------
