#include "StartOrbiterSDLBluetooth.h"

#include "OrbiterSDLBluetooth.h"

using namespace DCE;

//-----------------------------------------------------------------------------------------------------
OrbiterSDLBluetooth *StartOrbiterSDLBluetooth(
	class BDCommandProcessor *pBDCommandProcessor,
	int PK_Device, 
	string sRouter_IP, 
	string sLocalDirectory, 
	bool bLocalMode, 
	int Width,
	int Height
)
{
	OrbiterSDLBluetooth * pOrbiterSDLBluetooth = 
		new OrbiterSDLBluetooth(
				pBDCommandProcessor,
				PK_Device, sRouter_IP, 
				sLocalDirectory, bLocalMode, Width, Height);

	if (bLocalMode || pOrbiterSDLBluetooth->Connect())
	{
		g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
		pOrbiterSDLBluetooth->Initialize(gtSDLGraphic);
		
		if( !bLocalMode  )
			pOrbiterSDLBluetooth->CreateChildren();
/*		
		
		g_pPlutoLogger->Write(LV_STATUS, "Starting processing events");
		SDL_Event Event;

		while(1)
		{
			SDL_WaitEvent(&Event);

			if (Event.type == SDL_QUIT)
				break;
		}
*/
		/*

			if( Event.type == SDL_MOUSEBUTTONDOWN )
				pOrbiterSDLBluetooth->RegionDown(Event.button.x,Event.button.y);
			else if( Event.type == SDL_MOUSEMOTION )
				int k=2; //pOrbiterSDLBluetooth->RegionDown(Event.button.x,Event.button.y);
			else if( Event.type == SDL_KEYDOWN )
			{
				switch( Event.key.keysym.sym )
				{
				case SDLK_0:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_0_CONST);
					break;
				case SDLK_1:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_1_CONST);
					break;
				case SDLK_2:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_2_CONST);
					break;
				case SDLK_3:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_3_CONST);
					break;
				case SDLK_4:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_4_CONST);
					break;
				case SDLK_5:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_5_CONST);
					break;
				case SDLK_6:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_6_CONST);
					break;
				case SDLK_7:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_7_CONST);
					break;
				case SDLK_8:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_8_CONST);
					break;
				case SDLK_9:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_9_CONST);
					break;

				case SDLK_UP:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_Up_Arrow_CONST);
					break;
				case SDLK_DOWN:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_Down_Arrow_CONST);
					break;
				case SDLK_LEFT:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_Left_Arrow_CONST);
					break;
				case SDLK_RIGHT:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_Right_Arrow_CONST);
					break;
				case SDLK_KP_ENTER:
					pOrbiterSDLBluetooth->ButtonDown(BUTTON_Enter_CONST);
					break;
				default:
					g_pPlutoLogger->Write(LV_STATUS,"Unknown key: %d",(int) Event.key.keysym.sym);
					continue;
				};
			}
		}
		*/
	}

	return pOrbiterSDLBluetooth;
}
//-----------------------------------------------------------------------------------------------------
