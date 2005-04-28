/*
 StartOrbiterSDL

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "DCE/Logger.h"

//#define PHONEKEYS
//#define AUDIDEMO

#ifdef WIN32
#include "OrbiterSDL.h"
#include "MainDialog.h"
#else
#include "../Linux/OrbiterLinux.h"
#endif

#include "StartOrbiterSDL.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h"
#include "../Simulator.h"

extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
void DeadlockHandler(PlutoLock *pPlutoLock)
{
	// This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Deadlock problem.  Going to reload and quit");
		g_pCommand_Impl->OnReload();
	}
}
void SocketCrashHandler(Socket *pSocket)
{
	// This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Socket problem.  Going to reload and quit");
		g_pCommand_Impl->OnReload();
	}
}

struct keyboardState
{
	bool bShiftDown;
	bool bControlDown;
	bool bAltDown;
	bool bRepeat;
	bool bCapsLock;
	clock_t cKeyDown;
};

void translateSDLEventToOrbiterEvent(SDL_Event &sdlEvent, Orbiter::Event *orbiterEvent, struct keyboardState *kbdState)
{
	orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;

	switch ( sdlEvent.type )
	{
		case SDL_QUIT: orbiterEvent->type = Orbiter::Event::QUIT;	break;

		case SDL_MOUSEBUTTONDOWN:
#ifdef WIN32
			RecordMouseAction(sdlEvent.button.x, sdlEvent.button.y);
#endif
#ifdef AUDIDEMO
			orbiterEvent->type = Orbiter::Event::BUTTON_DOWN;
			switch ( sdlEvent.button.button )
			{
				case 1:	orbiterEvent->data.button.m_iPK_Button = BUTTON_4_CONST; 			break;
				case 2: orbiterEvent->data.button.m_iPK_Button = BUTTON_Enter_CONST; 		break;
				case 3:	orbiterEvent->data.button.m_iPK_Button = BUTTON_5_CONST; 			break;
				case 4:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST; 	break;
				case 5:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST; 	break;
				case 6:	orbiterEvent->data.button.m_iPK_Button = BUTTON_2_CONST; 			break;
				case 7:	orbiterEvent->data.button.m_iPK_Button = BUTTON_1_CONST; 			break;

				default:
					orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
			}
#else
			orbiterEvent->type = Orbiter::Event::REGION_DOWN;
			orbiterEvent->data.region.m_iButton = sdlEvent.button.button;
			orbiterEvent->data.region.m_iX = sdlEvent.button.x;
			orbiterEvent->data.region.m_iY = sdlEvent.button.y;
#endif
			break;

		case SDL_MOUSEBUTTONUP:
			orbiterEvent->type = Orbiter::Event::REGION_UP;
			orbiterEvent->data.region.m_iButton = sdlEvent.button.button;
			orbiterEvent->data.region.m_iX = sdlEvent.button.x;
			orbiterEvent->data.region.m_iY = sdlEvent.button.y;
			break;

		case SDL_KEYDOWN:
			// on key up we update the keyboard state
			// orbiterEvent->type = Orbiter::Event::BUTTON_DOWN;

			switch (sdlEvent.key.keysym.sym)
			{
                case SDLK_LSHIFT: case SDLK_RSHIFT:		kbdState->bShiftDown = true; 				break;
                case SDLK_LCTRL: case SDLK_RCTRL:		kbdState->bControlDown = true;				break;
                case SDLK_LALT: case SDLK_RALT:			kbdState->bAltDown = true;					break;
                case SDLK_CAPSLOCK:						kbdState->bCapsLock = !kbdState->bCapsLock;	break;
                default:
                    kbdState->cKeyDown=clock();  // We don't care how long the shift, ctrl or alt are held down, but the other keys do matter
                    break;
			}
			break;

		case SDL_KEYUP:
#ifdef WIN32
			RecordKeyboardAction(Event.key.keysym.sym);
#endif
			orbiterEvent->type = Orbiter::Event::BUTTON_DOWN;

			kbdState->bRepeat = (kbdState->cKeyDown && (clock() - kbdState->cKeyDown > CLOCKS_PER_SEC / 2) );
			kbdState->cKeyDown = 0;
			g_pPlutoLogger->Write(LV_STATUS, "key up %d  rept: %d  shif: %d ctrl: %d alt: %d caps: %d: downtime: %d",
					(int) sdlEvent.key.keysym.sym,
					(int) kbdState->bRepeat,
					(int) kbdState->bShiftDown,
					(int) kbdState->bControlDown,
					(int) kbdState->bAltDown,
					(int) kbdState->bCapsLock,
					(int) kbdState->cKeyDown);

#ifndef PHONEKEYS
			if ( SDLK_a <= sdlEvent.key.keysym.sym && sdlEvent.key.keysym.sym <= SDLK_z )
			{
				orbiterEvent->type = Orbiter::Event::BUTTON_DOWN;
				if( ( ! kbdState->bCapsLock && ! kbdState->bShiftDown ) ||
					(   kbdState->bCapsLock &&   kbdState->bShiftDown ) )
					orbiterEvent->data.button.m_iPK_Button = BUTTON_a_CONST + sdlEvent.key.keysym.sym - SDLK_a;
                else
					orbiterEvent->data.button.m_iPK_Button = BUTTON_A_CONST + sdlEvent.key.keysym.sym - SDLK_a;
			}
			else
#endif
			if( sdlEvent.key.keysym.sym==SDLK_LSHIFT || sdlEvent.key.keysym.sym==SDLK_RSHIFT )
			{
				orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
				kbdState->bShiftDown=false;
			}
			else if( sdlEvent.key.keysym.sym==SDLK_LCTRL || sdlEvent.key.keysym.sym==SDLK_RCTRL )
			{
				orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
				kbdState->bControlDown=false;
			}
			else if( sdlEvent.key.keysym.sym==SDLK_LALT || sdlEvent.key.keysym.sym==SDLK_RALT )
			{
				orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
				kbdState->bAltDown=false;
			}
			else if( ! kbdState->bShiftDown && ! kbdState->bControlDown && ! kbdState->bAltDown && ! kbdState->bRepeat )
			{
				// No Modifiers were down
				switch (sdlEvent.key.keysym.sym )
				{
                    case SDLK_0: case SDLK_KP0: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_0_CONST; break;
                    case SDLK_1: case SDLK_KP1: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_1_CONST; break;
                    case SDLK_2: case SDLK_KP2: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_2_CONST; break;
                    case SDLK_3: case SDLK_KP3: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_3_CONST; break;
                    case SDLK_4: case SDLK_KP4: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_4_CONST; break;
                    case SDLK_5: case SDLK_KP5: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_5_CONST; break;
                    case SDLK_6: case SDLK_KP6: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_6_CONST; break;
                    case SDLK_7: case SDLK_KP7: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_7_CONST; break;
                    case SDLK_8: case SDLK_KP8: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_8_CONST; break;
                    case SDLK_9: case SDLK_KP9: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_9_CONST; break;
#ifdef PHONEKEYS
                    case SDLK_c:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_C_CONST; break;
                    case SDLK_p:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Pencil_CONST; break;
                    case SDLK_t:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Talk_CONST; break;
                    case SDLK_e:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_End_CONST; break;
                    case SDLK_l:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Soft_left_CONST; break;
                    case SDLK_r:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Phone_Soft_right_CONST; break;
					case SDLK_ASTERISK: orbiterEvent->data.button.m_iPK_Button = BUTTON_Asterisk_CONST; break;
                    case SDLK_HASH:     orbiterEvent->data.button.m_iPK_Button = BUTTON_Pound_CONST; break;
#endif
                    case SDLK_F1:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F1_CONST; break;
                    case SDLK_F2:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F2_CONST; break;
                    case SDLK_F3:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F3_CONST; break;
                    case SDLK_F4:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F4_CONST; break;
                    case SDLK_F5:		orbiterEvent->data.button.m_iPK_Button = BUTTON_F5_CONST; break;

                    case SDLK_UP:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST; break;
                    case SDLK_DOWN:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST; break;
                    case SDLK_LEFT:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Left_Arrow_CONST; break;
                    case SDLK_RIGHT:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Right_Arrow_CONST; break;

					case SDLK_KP_ENTER: case SDLK_RETURN: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Enter_CONST;  break;

					case SDLK_BACKSPACE:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Back_CONST;  break;

					default:
						orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
                        g_pPlutoLogger->Write(LV_STATUS, "Unknown key: %d", (int) sdlEvent.key.keysym.sym);
				};
			} // else if( !bShiftDown && !bControlDown && !bAltDown && !bRepeat )
        	else if ( kbdState->bShiftDown && ! kbdState->bControlDown && ! kbdState->bAltDown && ! kbdState->bRepeat )
            {
				// The Shift key was pressed
				switch (sdlEvent.key.keysym.sym)
				{
					case SDLK_UP:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Up_Arrow_CONST; break;
					case SDLK_DOWN:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Down_Arrow_CONST; break;
					case SDLK_LEFT:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Left_Arrow_CONST; break;
					case SDLK_RIGHT:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Right_Arrow_CONST; break;

//					case SDLK_0:  		orbiterEvent->data.button.m_iPK_Button = BUTTON_0_CONST; break;
//					case SDLK_1:  		orbiterEvent->data.button.m_iPK_Button = BUTTON_1_CONST; break;
//					case SDLK_2: 		orbiterEvent->data.button.m_iPK_Button = BUTTON_2_CONST; break;
					case SDLK_3: 		orbiterEvent->data.button.m_iPK_Button = BUTTON_Pound_CONST; break;
// 					case SDLK_4: 		orbiterEvent->data.button.m_iPK_Button = BUTTON_4_CONST; break;
// 					case SDLK_5: 		orbiterEvent->data.button.m_iPK_Button = BUTTON_5_CONST; break;
// 					case SDLK_6: 		orbiterEvent->data.button.m_iPK_Button = BUTTON_6_CONST; break;
// 					case SDLK_7: 		orbiterEvent->data.button.m_iPK_Button = BUTTON_7_CONST; break;
 					case SDLK_8: 		orbiterEvent->data.button.m_iPK_Button = BUTTON_Asterisk_CONST; break;
// 					case SDLK_9: 		orbiterEvent->data.button.m_iPK_Button = BUTTON_9_CONST; break;

					default:
						orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
				};
			}
            else if( kbdState->bRepeat ) // if we need to repeat the key
            {
				switch (sdlEvent.key.keysym.sym)
                {
                    case SDLK_0: case SDLK_KP0: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_0_CONST; break;
                    case SDLK_1: case SDLK_KP1: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_1_CONST; break;
                    case SDLK_2: case SDLK_KP2: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_2_CONST; break;
                    case SDLK_3: case SDLK_KP3: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_3_CONST; break;
                    case SDLK_4: case SDLK_KP4: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_4_CONST; break;
                    case SDLK_5: case SDLK_KP5: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_5_CONST; break;
                    case SDLK_6: case SDLK_KP6: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_6_CONST; break;
                    case SDLK_7: case SDLK_KP7: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_7_CONST; break;
                    case SDLK_8: case SDLK_KP8: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_8_CONST; break;
                    case SDLK_9: case SDLK_KP9: 	orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_9_CONST; break;
#ifdef PHONEKEYS
                    case SDLK_c:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_Phone_C_CONST; break;
                    case SDLK_p:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_Phone_Pencil_CONST; break;
                    case SDLK_t:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_Phone_Talk_CONST; break;
                    case SDLK_e:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_Phone_End_CONST; break;
                    case SDLK_l:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_Phone_Soft_left_CONST; break;
                    case SDLK_r:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_Phone_Soft_right_CONST; break;
					case SDLK_ASTERISK: orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_Asterisk_CONST; break;
                    case SDLK_HASH:     orbiterEvent->data.button.m_iPK_Button = BUTTON_Rept_Pound_CONST; break;
#endif
					default:
						orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
				}
			}

			break; // case SDL_KEYUP

		case SDL_MOUSEMOTION: // not handled
		default:
			orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
			break;
	}
}

bool StartOrbiter(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
				  int Width, int Height, bool bFullScreen)
{
#ifdef WIN32
    OrbiterSDL *pCLinux =
        new OrbiterSDL(
        PK_Device, sRouter_IP,
        sLocalDirectory, bLocalMode, Width, Height, bFullScreen);
#else
    OrbiterSDL *pCLinux =
        new OrbiterLinux(
        PK_Device, sRouter_IP,
        sLocalDirectory, bLocalMode, Width, Height);
#endif

	// Add a handler to take care of crashes
	g_pCommand_Impl = pCLinux;
	g_pDeadlockHandler=DeadlockHandler;
	g_pSocketCrashHandler=SocketCrashHandler;

    if (bLocalMode || pCLinux->Connect(0)) // Don't validate the device template, since the same binary is used for lots of devices
    {
        g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
        pCLinux->Initialize(gtSDLGraphic);

        if (!bLocalMode)
            pCLinux->CreateChildren();

        //pCLinux->Initialize_Display();
		g_pPlutoLogger->Write(LV_STATUS, "Creating the simulator");
		Simulator::GetInstance()->m_pOrbiter = pCLinux;
		Simulator::GetInstance()->LoadConfigurationFile("/etc/Orbiter.conf");

        g_pPlutoLogger->Write(LV_STATUS, "Starting processing events");

        // temporary hack --
        // have to figure out what should be the default behavior of the arrows, moving the highlighted object, or scrolling a grid
        // For now I'll assume that shift + arrows scrolls a grid

        SDL_Event Event;
		Orbiter::Event orbiterEvent;
		struct keyboardState keyboardState; // keep the state of the Ctrl/Shift etc if we need it in the furter calls.

		keyboardState.bShiftDown = keyboardState.bControlDown = keyboardState.bAltDown = 0;
		keyboardState.bRepeat = keyboardState.bCapsLock = 0;
		keyboardState.cKeyDown = 0;

		while (!pCLinux->m_bQuit)
        {
//g_pPlutoLogger->Write(LV_STATUS,"Before wait for event");
            SDL_WaitEvent(&Event);
//g_pPlutoLogger->Write(LV_STATUS,"wait for event returned %d",Event.type);

			// convert the SDL into what we know to interpret.
			translateSDLEventToOrbiterEvent(Event, &orbiterEvent, &keyboardState);

			if ( orbiterEvent.type == Orbiter::Event::QUIT )
				break;

			pCLinux->ProcessEvent(orbiterEvent);
        }  // while

	g_pPlutoLogger->Write(LV_STATUS, "Orbiter quit is %d",(int) pCLinux->m_bQuit);

    } // if connect
	bool bReload = pCLinux->m_bReload;
g_pPlutoLogger->Write(LV_STATUS, "ready to delete instance End of SDL loop with reload: %s",(bReload ? "Y" : "N"));
    delete pCLinux;
g_pPlutoLogger->Write(LV_STATUS, "finished deleting pcLinux");
	return bReload;
}

