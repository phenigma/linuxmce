#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"

#define PHONEKEYS
#define AUDIDEMO

#ifdef WIN32
#include "OrbiterSDL.h"
#else
#include "../Linux/OrbiterLinux.h"
#endif

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h"

void StartOrbiter(int PK_Device,string sRouter_IP,string sLocalDirectory,bool bLocalMode,int Width,int Height)
{
#ifdef WIN32
    OrbiterSDL *pCLinux =
        new OrbiterSDL(
        PK_Device, sRouter_IP,
        sLocalDirectory, bLocalMode, Width, Height);
#else
//     OrbiterLinux *pOrbiterLinux =
    OrbiterSDL *pCLinux =
        new OrbiterLinux(
        PK_Device, sRouter_IP,
        sLocalDirectory, bLocalMode, Width, Height);

//  pOrbiterLinux->setDesktopDisplayName(NestedDesktop);
//  pOrbiterLinux->setDisplayName(getenv("DISPLAY"));
//  pOrbiterLinux->bootNestedXServer();
//     OrbiterSDL *pCLinux = pOrbiterLinux;
#endif

    if (bLocalMode || pCLinux->Connect())
    {
        g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
        pCLinux->Initialize(gtSDLGraphic);

        if (!bLocalMode)
            pCLinux->CreateChildren();

        //pCLinux->Initialize_Display();

        g_pPlutoLogger->Write(LV_STATUS, "Starting processing events");
        SDL_Event Event;

        // temporary hack --
        // have to figure out what should be the default behavior of the arrows, moving the highlighted object, or scrolling a grid
        // For now I'll assume that shift + arrows scrolls a grid
        bool bShiftDown=false,bControlDown=false,bAltDown=false,bRepeat=false,bCapsLock=false;
        clock_t cKeyDown=0;
        while (true)
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
                    pCLinux->ButtonDown(BUTTON_Up_Arrow_CONST);
                else if( Event.button.button==5 )
                    pCLinux->ButtonDown(BUTTON_Down_Arrow_CONST);
                else if( Event.button.button==2 )
                    pCLinux->ButtonDown(BUTTON_Enter_CONST);
                else if( Event.button.button==1 )
                    pCLinux->ButtonDown(BUTTON_4_CONST);
                else if( Event.button.button==3 )
                    pCLinux->ButtonDown(BUTTON_5_CONST);
                else if( Event.button.button==7 )
                    pCLinux->ButtonDown(BUTTON_1_CONST);
                else if( Event.button.button==6 )
                    pCLinux->ButtonDown(BUTTON_2_CONST);
                else
                    g_pPlutoLogger->Write(LV_WARNING, "========================================== Mouse button not handled!");
            }
#else
            if (Event.type == SDL_MOUSEBUTTONDOWN)
                pCLinux->RegionDown(Event.button.x, Event.button.y);
#endif
            else if (Event.type == SDL_MOUSEMOTION)
                int k=2; //pCLinux->RegionDown(Event.button.x,Event.button.y);
            else if (Event.type == SDL_KEYDOWN)
            {
                switch (Event.key.keysym.sym)
                {
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    bShiftDown=true;
                    break;
                case SDLK_LCTRL:
                case SDLK_RCTRL:
                    bControlDown=true;
                    break;
                case SDLK_LALT:
                case SDLK_RALT:
                    bAltDown=true;
                    break;
                case SDLK_CAPSLOCK:
                    bCapsLock = !bCapsLock;
                    break;
                default:
                    cKeyDown=clock();  // We don't care how long the shift, ctrl or alt are held down, but the other keys do matter
                    break;
                }
            }
            else if (Event.type == SDL_KEYUP)
            {
                bool bHandled=false;
                bRepeat = cKeyDown && clock()-cKeyDown > CLOCKS_PER_SEC/2;
                cKeyDown=0;

                g_pPlutoLogger->Write(LV_STATUS, "key up %d  rept: %d  shif: %d",(int) Event.key.keysym.sym, (int) bRepeat, (int) bShiftDown);
#ifndef PHONEKEYS
                if(Event.key.keysym.sym >= SDLK_a && Event.key.keysym.sym <= SDLK_z)
                {
                    if((!bCapsLock && !bShiftDown) || (bCapsLock && bShiftDown))
                        bHandled = pCLinux->ButtonDown(BUTTON_a_CONST + Event.key.keysym.sym - SDLK_a);
                    else
                        bHandled = pCLinux->ButtonDown(BUTTON_A_CONST + Event.key.keysym.sym - SDLK_a);
                }
                else
#endif
                if( Event.key.keysym.sym==SDLK_LSHIFT || Event.key.keysym.sym==SDLK_RSHIFT )
                    bShiftDown=false;

                else if( Event.key.keysym.sym==SDLK_LCTRL || Event.key.keysym.sym==SDLK_RCTRL )
                    bControlDown=false;
                else if( Event.key.keysym.sym==SDLK_LALT || Event.key.keysym.sym==SDLK_RALT )
                    bAltDown=false;
                else if( !bShiftDown && !bControlDown && !bAltDown && !bRepeat )
                {
                    switch (Event.key.keysym.sym)
                    {
                    case SDLK_0:
                    case SDLK_KP0:
                        bHandled=pCLinux->ButtonDown(BUTTON_0_CONST);
                        break;
                    case SDLK_1:
                    case SDLK_KP1:
                        bHandled=pCLinux->ButtonDown(BUTTON_1_CONST);
                        break;
                    case SDLK_2:
                    case SDLK_KP2:
                        bHandled=pCLinux->ButtonDown(BUTTON_2_CONST);
                        break;
                    case SDLK_3:
                    case SDLK_KP3:
                        bHandled=pCLinux->ButtonDown(BUTTON_3_CONST);
                        break;
                    case SDLK_4:
                    case SDLK_KP4:
                        bHandled=pCLinux->ButtonDown(BUTTON_4_CONST);
                        break;
                    case SDLK_5:
                    case SDLK_KP5:
                        bHandled=pCLinux->ButtonDown(BUTTON_5_CONST);
                        break;
                    case SDLK_6:
                    case SDLK_KP6:
                        bHandled=pCLinux->ButtonDown(BUTTON_6_CONST);
                        break;
                    case SDLK_7:
                    case SDLK_KP7:
                        bHandled=pCLinux->ButtonDown(BUTTON_7_CONST);
                        break;
                    case SDLK_8:
                    case SDLK_KP8:
                        bHandled=pCLinux->ButtonDown(BUTTON_8_CONST);
                        break;
                    case SDLK_9:
                    case SDLK_KP9:
                        bHandled=pCLinux->ButtonDown(BUTTON_9_CONST);
                        break;

#ifdef PHONEKEYS
                    case SDLK_c:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_C_CONST);
                        break;
                    case SDLK_p:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_Pencil_CONST);
                        break;
                    case SDLK_t:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_Talk_CONST);
                        break;
                    case SDLK_e:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_End_CONST);
                        break;
                    case SDLK_l:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_Soft_left_CONST);
                        break;
                    case SDLK_r:
                        bHandled=pCLinux->ButtonDown(BUTTON_Phone_Soft_right_CONST);
                        break;
                    case SDLK_ASTERISK:
                        bHandled=pCLinux->ButtonDown(BUTTON_Asterisk_CONST);
                        break;
                    case SDLK_HASH:
                        bHandled=pCLinux->ButtonDown(BUTTON_Pound_CONST);
                        break;
#endif
                    case SDLK_F1:
                        bHandled=pCLinux->ButtonDown(BUTTON_F1_CONST);
                        break;
                    case SDLK_F2:
                        bHandled=pCLinux->ButtonDown(BUTTON_F2_CONST);
                        break;
                    case SDLK_F3:
                        bHandled=pCLinux->ButtonDown(BUTTON_F3_CONST);
                        break;
                    case SDLK_F4:
                        bHandled=pCLinux->ButtonDown(BUTTON_F4_CONST);
                        break;
                    case SDLK_F5:
                        bHandled=pCLinux->ButtonDown(BUTTON_F5_CONST);
                        break;

                    case SDLK_UP:
                        bHandled=pCLinux->ButtonDown(BUTTON_Up_Arrow_CONST);
                        break;
                    case SDLK_DOWN:
                        bHandled=pCLinux->ButtonDown(BUTTON_Down_Arrow_CONST);
                        break;
                    case SDLK_LEFT:
                        bHandled=pCLinux->ButtonDown(BUTTON_Left_Arrow_CONST);
                        break;
                    case SDLK_RIGHT:
                        bHandled=pCLinux->ButtonDown(BUTTON_Right_Arrow_CONST);
                        break;
                    case SDLK_KP_ENTER:
                    case SDLK_RETURN:
                        bHandled=pCLinux->ButtonDown(BUTTON_Enter_CONST);
                        break;
                    case SDLK_BACKSPACE:
                        bHandled=pCLinux->ButtonDown(BUTTON_Back_CONST);
                        break;
                    default:
                        g_pPlutoLogger->Write(LV_STATUS, "Unknown key: %d", (int) Event.key.keysym.sym);
                        continue;
                    };
                } // else if( !bShiftDown && !bControlDown && !bAltDown && !bRepeat )
                else if( bShiftDown && !bControlDown && !bAltDown && !bRepeat )
                {
                    switch (Event.key.keysym.sym)
                    {
                    case SDLK_UP:
                        bHandled=pCLinux->ButtonDown(BUTTON_Shift_Up_Arrow_CONST);
                        break;
                    case SDLK_DOWN:
                        bHandled=pCLinux->ButtonDown(BUTTON_Shift_Down_Arrow_CONST);
                        break;
                    case SDLK_LEFT:
                        bHandled=pCLinux->ButtonDown(BUTTON_Shift_Left_Arrow_CONST);
                        break;
                    case SDLK_RIGHT:
                        bHandled=pCLinux->ButtonDown(BUTTON_Shift_Right_Arrow_CONST);
                        break;
/*
                    case SDLK_0:
                        bHandled=pCLinux->ButtonDown(BUTTON_0_CONST);
                        break;
                    case SDLK_1:
                        bHandled=pCLinux->ButtonDown(BUTTON_1_CONST);
                        break;
                    case SDLK_2:
                        bHandled=pCLinux->ButtonDown(BUTTON_2_CONST);
                        break;
*/
                    case SDLK_3:
                        bHandled=pCLinux->ButtonDown(BUTTON_Pound_CONST);
                        break;
/*
                    case SDLK_4:
                        bHandled=pCLinux->ButtonDown(BUTTON_4_CONST);
                        break;
                    case SDLK_5:
                        bHandled=pCLinux->ButtonDown(BUTTON_5_CONST);
                        break;
                    case SDLK_6:
                        bHandled=pCLinux->ButtonDown(BUTTON_6_CONST);
                        break;
                    case SDLK_7:
                        bHandled=pCLinux->ButtonDown(BUTTON_7_CONST);
                        break;
*/
                    case SDLK_8:
                        bHandled=pCLinux->ButtonDown(BUTTON_Asterisk_CONST);
                        break;
/*
                    case SDLK_9:
                        bHandled=pCLinux->ButtonDown(BUTTON_9_CONST);
                        break;
*/
                    };
                }
                else if( bRepeat )
                {
                    switch (Event.key.keysym.sym)
                    {
                    case SDLK_0:
                    case SDLK_KP0:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_0_CONST);
                        break;
                    case SDLK_1:
                    case SDLK_KP1:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_1_CONST);
                        break;
                    case SDLK_2:
                    case SDLK_KP2:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_2_CONST);
                        break;
                    case SDLK_3:
                    case SDLK_KP3:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_3_CONST);
                        break;
                    case SDLK_4:
                    case SDLK_KP4:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_4_CONST);
                        break;
                    case SDLK_5:
                    case SDLK_KP5:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_5_CONST);
                        break;
                    case SDLK_6:
                    case SDLK_KP6:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_6_CONST);
                        break;
                    case SDLK_7:
                    case SDLK_KP7:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_7_CONST);
                        break;
                    case SDLK_8:
                    case SDLK_KP8:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_8_CONST);
                        break;
                    case SDLK_9:
                    case SDLK_KP9:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_9_CONST);
                        break;

#ifdef PHONEKEYS
                    case SDLK_c:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_C_CONST);
                        break;
                    case SDLK_p:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_Pencil_CONST);
                        break;
                    case SDLK_t:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_Talk_CONST);
                        break;
                    case SDLK_e:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_End_CONST);
                        break;
                    case SDLK_l:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_Soft_left_CONST);
                        break;
                    case SDLK_r:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Phone_Soft_right_CONST);
                        break;
                    case SDLK_ASTERISK:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Asterisk_CONST);
                        break;
                    case SDLK_HASH:
                        bHandled=pCLinux->ButtonDown(BUTTON_Rept_Pound_CONST);
                        break;
#endif
                    }
                }
            }
        }  // while
    } // if connect
    delete pCLinux;
}
