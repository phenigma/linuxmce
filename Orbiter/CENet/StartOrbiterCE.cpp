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
//-----------------------------------------------------------------------------------------------------
void StartOrbiterCE(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
					int Width,int Height, bool bFullScreen)
{
	OrbiterSDL_WinCE::BuildOrbiterSDL_WinCE(
		PK_Device, sRouter_IP,
		sLocalDirectory, bLocalMode, 
		Width, Height, bFullScreen
	); //the builder method

	OrbiterSDL_WinCE *pOrbiter = OrbiterSDL_WinCE::GetInstance();

	pOrbiter->WriteStatusOutput("Connecting to DCERouter...");
	Sleep(500);

    if (bLocalMode || pOrbiter->Connect())
    {
        g_pPlutoLogger->Write(LV_STATUS, "Connect OK");

		pOrbiter->WriteStatusOutput("Parsing configuration data...");
        pOrbiter->Initialize(gtSDLGraphic);

        if (!bLocalMode)
            pOrbiter->CreateChildren();

        g_pPlutoLogger->Write(LV_STATUS, "Starting processing events");
        SDL_Event Event;

		pOrbiter->WriteStatusOutput("Starting processing events...");

        // temporary hack --
        // have to figure out what should be the default behavior of the arrows, moving the highlighted object, or scrolling a grid
        // For now I'll assume that shift + arrows scrolls a grid
        bool bShiftDown=false,bControlDown=false,bAltDown=false,bRepeat=false,bCapsLock=false;
        clock_t cKeyDown=0;
        while (!pOrbiter->m_bQuitWinCE)
        {
//g_pPlutoLogger->Write(LV_STATUS,"Before wait for event");
            SDL_WaitEvent(&Event);
//g_pPlutoLogger->Write(LV_STATUS,"wait for event returned %d",Event.type);

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
                pOrbiter->RegionDown(Event.button.x, Event.button.y);
#endif
            else if (Event.type == SDL_MOUSEMOTION)
                int k=2; //pOrbiter->RegionDown(Event.button.x,Event.button.y);
        }  // while
    } // if connect

    
	//hack: if we'll try to delete pOrbiter, we'll stuck here
	pOrbiter->~OrbiterSDL_WinCE(); 

	pOrbiter = NULL;
}
//-----------------------------------------------------------------------------------------------------
