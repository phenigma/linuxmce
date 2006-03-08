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
#  include "OrbiterSDL.h"
#  include "MainDialog.h"
#else
#  include "../Linux/OrbiterLinux.h"
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
      g_pPlutoLogger->Write(LV_CRITICAL,"Deadlock problem.  %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
    g_pCommand_Impl->OnReload();
  }
}
void SocketCrashHandler(Socket *pSocket)
{
  // This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
  if( g_pCommand_Impl )
  {
    if( g_pPlutoLogger )
      g_pPlutoLogger->Write(LV_CRITICAL,"Socket problem. %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
    g_pCommand_Impl->OnReload();
  }
}

void translateSDLEventToOrbiterEvent(SDL_Event &sdlEvent, Orbiter::Event *orbiterEvent, struct keyboardState *kbdState)
{
  orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;

  switch ( sdlEvent.type )
  {
  case SDL_QUIT: orbiterEvent->type = Orbiter::Event::QUIT;	break;

  case SDL_KEYDOWN:
  case SDL_KEYUP:

    orbiterEvent->type = SDL_KEYDOWN != sdlEvent.type ? Orbiter::Event::BUTTON_UP :
                         Orbiter::Event::BUTTON_DOWN;

    switch (sdlEvent.key.keysym.sym)
    {
      case SDLK_LSHIFT:   case SDLK_RSHIFT:       kbdState->bShiftDown = SDL_KEYDOWN == sdlEvent.type;      break;
      case SDLK_LCTRL:    case SDLK_RCTRL:		kbdState->bControlDown = SDL_KEYDOWN == sdlEvent.type;				break;
      case SDLK_LALT:     case SDLK_RALT:			kbdState->bAltDown = SDL_KEYDOWN == sdlEvent.type;					break;
    default:
      break; //nothing here
    }

    if(SDL_KEYDOWN == sdlEvent.type && SDLK_CAPSLOCK == sdlEvent.key.keysym.sym)
      kbdState->bCapsLock = !kbdState->bCapsLock;

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS, "key %s %d shif: %d ctrl: %d alt: %d caps: %d",
                          SDL_KEYDOWN == sdlEvent.type ? "down" : "up",
                          (int) sdlEvent.key.keysym.sym,
                          (int) kbdState->bShiftDown,
                          (int) kbdState->bControlDown,
                          (int) kbdState->bAltDown,
                          (int) kbdState->bCapsLock);
#endif

#ifndef PHONEKEYS
    if ( SDLK_a <= sdlEvent.key.keysym.sym && sdlEvent.key.keysym.sym <= SDLK_z )
    {
      if( ( ! kbdState->bCapsLock && ! kbdState->bShiftDown ) ||
          (   kbdState->bCapsLock &&   kbdState->bShiftDown ) )
        orbiterEvent->data.button.m_iPK_Button = BUTTON_a_CONST + sdlEvent.key.keysym.sym - SDLK_a;
      else
        orbiterEvent->data.button.m_iPK_Button = BUTTON_A_CONST + sdlEvent.key.keysym.sym - SDLK_a;
    }
    else
#endif
/*
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

      else*/

      if( ! kbdState->bShiftDown && ! kbdState->bControlDown && ! kbdState->bAltDown )
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
		case SDLK_SPACE:		orbiterEvent->data.button.m_iPK_Button = BUTTON_space_CONST; break;

        case SDLK_BACKSPACE:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Back_CONST;  break;

		case SDLK_UNDERSCORE:   orbiterEvent->data.button.m_iPK_Button = BUTTON_underscore_CONST;  break;
		case SDLK_MINUS:	case SDLK_KP_MINUS:   orbiterEvent->data.button.m_iPK_Button = BUTTON_dash_CONST;  break;
		case SDLK_PLUS:		case SDLK_KP_PLUS:		orbiterEvent->data.button.m_iPK_Button = BUTTON_plus_CONST;  break;
		case SDLK_EQUALS:	case SDLK_KP_EQUALS:   orbiterEvent->data.button.m_iPK_Button = BUTTON_equals_sign_CONST;  break;

		case SDLK_PERIOD:	case SDLK_KP_PERIOD:	orbiterEvent->data.button.m_iPK_Button = BUTTON_dot_CONST;  break;
		case SDLK_COMMA:	orbiterEvent->data.button.m_iPK_Button = BUTTON_comma_CONST; break;
		case SDLK_COLON:	 orbiterEvent->data.button.m_iPK_Button = BUTTON_colon_CONST; break;
		case SDLK_SEMICOLON:	orbiterEvent->data.button.m_iPK_Button = BUTTON_semicolumn_CONST; break;
        case SDLK_QUOTE:     orbiterEvent->data.button.m_iPK_Button = BUTTON_single_quote_CONST; break;
		case SDLK_QUOTEDBL:    orbiterEvent->data.button.m_iPK_Button = BUTTON_double_quote_CONST; break;

        default:
          orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
          g_pPlutoLogger->Write(LV_STATUS, "Unknown key: %d", (int) sdlEvent.key.keysym.sym);
        };
      } // else if( !bShiftDown && !bControlDown && !bAltDown && !bRepeat )
      else if ( kbdState->bShiftDown && ! kbdState->bControlDown && ! kbdState->bAltDown )
      {
        // The Shift key was pressed
        switch (sdlEvent.key.keysym.sym)
        {
        case SDLK_UP:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Up_Arrow_CONST; break;
        case SDLK_DOWN:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Down_Arrow_CONST; break;
        case SDLK_LEFT:		orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Left_Arrow_CONST; break;
        case SDLK_RIGHT:	orbiterEvent->data.button.m_iPK_Button = BUTTON_Shift_Right_Arrow_CONST; break;

		case SDLK_MINUS: case SDLK_KP_MINUS:  orbiterEvent->data.button.m_iPK_Button = BUTTON_underscore_CONST;  break;
		case SDLK_EQUALS:     case SDLK_KP_EQUALS:   orbiterEvent->data.button.m_iPK_Button = BUTTON_plus_CONST;  break;

		case SDLK_0: case SDLK_KP0:   orbiterEvent->data.button.m_iPK_Button = BUTTON_right_parenthesis_CONST; break;
    	case SDLK_1: case SDLK_KP1:   orbiterEvent->data.button.m_iPK_Button = BUTTON_exclamation_point_CONST; break;
      	case SDLK_2: case SDLK_KP2:   orbiterEvent->data.button.m_iPK_Button = BUTTON_at_sign_CONST; break;
      	case SDLK_3: case SDLK_KP3:   orbiterEvent->data.button.m_iPK_Button = BUTTON_Pound_CONST; break;
      	case SDLK_4: case SDLK_KP4:   orbiterEvent->data.button.m_iPK_Button = BUTTON_dollar_CONST; break;
      	case SDLK_5: case SDLK_KP5:   orbiterEvent->data.button.m_iPK_Button = BUTTON_percent_CONST; break;
      	case SDLK_6: case SDLK_KP6:   orbiterEvent->data.button.m_iPK_Button = BUTTON_caret_CONST; break;
      	case SDLK_7: case SDLK_KP7:   orbiterEvent->data.button.m_iPK_Button = BUTTON_ampersand_CONST; break;
      	case SDLK_8: case SDLK_KP8:   orbiterEvent->data.button.m_iPK_Button = BUTTON_Asterisk_CONST; break;
      	case SDLK_9: case SDLK_KP9:   orbiterEvent->data.button.m_iPK_Button = BUTTON_left_parenthesis_CONST; break;

		case SDLK_SEMICOLON:    orbiterEvent->data.button.m_iPK_Button = BUTTON_colon_CONST; break;
		case SDLK_QUOTE:     orbiterEvent->data.button.m_iPK_Button = BUTTON_double_quote_CONST; break;

        default:
          orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
        };
      }
    break;

  case SDL_MOUSEBUTTONDOWN:
#ifdef WIN32
    RecordMouseAction(sdlEvent.button.x, sdlEvent.button.y);
#endif
  case SDL_MOUSEBUTTONUP:

#ifdef AUDIDEMO
    orbiterEvent->type = (sdlEvent.type == SDL_MOUSEBUTTONDOWN) ? Orbiter::Event::BUTTON_DOWN : Orbiter::Event::BUTTON_UP;
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
    orbiterEvent->type = (sdlEvent.type == SDL_MOUSEBUTTONDOWN) ? Orbiter::Event::REGION_DOWN : Orbiter::Event::REGION_UP;
    orbiterEvent->data.region.m_iButton = sdlEvent.button.button;
    orbiterEvent->data.region.m_iX = sdlEvent.button.x;
    orbiterEvent->data.region.m_iY = sdlEvent.button.y;
#endif
    break;

  case SDL_MOUSEMOTION: // not handled
  default:
    orbiterEvent->type = Orbiter::Event::NOT_PROCESSED;
    break;
  }
}

OrbiterSDL *CreateOrbiter(int PK_Device,int PK_DeviceTemplate,string sRouter_IP,string sLocalDirectory,bool bLocalMode, int Width, int Height, bool bFullScreen)
{
	Simulator::GetInstance()->LoadConfigurationFile("/etc/Orbiter.conf");  
  
	OrbiterSDL *pCLinux =
	new OrbiterLinux(
		PK_Device, PK_DeviceTemplate, sRouter_IP,
		sLocalDirectory, bLocalMode, Width, Height, bFullScreen, Simulator::GetInstance()->m_bUseOpenGL);
	
	// Add a handler to take care of crashes
	g_pCommand_Impl = pCLinux;
	g_pDeadlockHandler=DeadlockHandler;
	g_pSocketCrashHandler=SocketCrashHandler;
	
	// Don't validate the device template, since the same binary is used for lots of devices
	if (bLocalMode || (pCLinux->GetConfig() && pCLinux->Connect(0))) 
	{
		g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
		pCLinux->Initialize(gtSDLGraphic);
		
		if (!bLocalMode)
		{
			pCLinux->CreateChildren();
			pCLinux->WaitForRelativesIfOSD();
		}
		
		//pCLinux->Initialize_Display();
		g_pPlutoLogger->Write(LV_STATUS, "Creating the simulator");
		Simulator::GetInstance()->m_pOrbiter = pCLinux;
		
		return pCLinux;
	}
	delete pCLinux;
	return NULL;
}

bool SDL_Event_Process(SDL_Event_Loop_Data &sdl_event_loop_data)
{
  if (sdl_event_loop_data.pOrbiter->m_bQuit)
    return false;
  if(SDL_PollEvent(&sdl_event_loop_data.event))
  {
    // convert the SDL into what we know to interpret.
    translateSDLEventToOrbiterEvent(sdl_event_loop_data.event, &sdl_event_loop_data.orbiterEvent, &sdl_event_loop_data.kbdState);

    if ( sdl_event_loop_data.orbiterEvent.type == Orbiter::Event::QUIT )
      return false;

    sdl_event_loop_data.pOrbiter->ProcessEvent(sdl_event_loop_data.orbiterEvent);
  }
  else
	((OrbiterSDL*)sdl_event_loop_data.pOrbiter)->OnIdle();
  return true;
}

bool SDL_Event_Loop_End(SDL_Event_Loop_Data &sdl_event_loop_data)
{
    bool bReload = sdl_event_loop_data.pOrbiter->m_bReload;
    g_pPlutoLogger->Write(LV_STATUS, "ready to delete instance End of SDL loop with reload: %s",(bReload ? "Y" : "N"));
    delete sdl_event_loop_data.pOrbiter;
    g_pPlutoLogger->Write(LV_STATUS, "finished deleting pcLinux");
    return bReload;
}
