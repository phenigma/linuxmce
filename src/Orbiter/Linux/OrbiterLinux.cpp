/*
  OrbiterLinux

  Copyright (C) 2004 Pluto, Inc., a Florida Corporation

  www.plutohome.com

  Phone: +1 (877) 758-8648

  This program is distributed according to the terms of the Pluto Public License, available at:
  http://plutohome.com/index.php?section=public_license

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include <stdlib.h>
#define USE_X11_LIB (! USE_WX_LIB)

#if (USE_WX_LIB)
#include "../wxAppMain/wx_all_include.cpp"
#include "../wxAppMain/wx_safe_dialog.h"
#include "../wxAppMain/wxdialog_waitgrid.h"
#include "../wxAppMain/wxdialog_waitlist.h"
#include "../wxAppMain/wxdialog_waituser.h"
#define USE_TASK_MANAGER true
#include "../Task.h"
#include "../TaskManager.h"
#endif // (USE_WX_LIB)

#include "DCE/Logger.h"
#include "OrbiterLinux.h"
#include "pluto_main/Define_DesignObj.h"

#include <iomanip>
#include <sstream>
#include <sys/time.h>

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include <SDL/SDL_syswm.h>

#include "pluto_main/Define_Button.h"
#include "PlutoUtils/PlutoButtonsToX.h"
#include "OSDScreenHandler.h"
#include "MouseBehavior_Linux.h"

#include "../CallBackTypes.h"

#include "../dialog_types.h"

using namespace std;

OrbiterLinux::OrbiterLinux(int DeviceID, int PK_DeviceTemplate,
                           string ServerAddress, string sLocalDirectory,
                           bool bLocalMode,
                           int nImageWidth, int nImageHeight,
						   bool bFullScreen,
						   bool bUseOpenGL)
        : OrbiterSDL(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory, bLocalMode,
                     nImageWidth, nImageHeight, bFullScreen, NULL, bUseOpenGL),
          // defaults
          /**
           * @hack to make it work for the short term. We need to find a way to set the class name properly or use the window ID if we can find it.
           * The reason this is hack is because there could be potentially multiple SDL_Applications running at the same time which could break the controlling code.
           */
          m_strWindowName("Orbiter"),
          m_strDisplayName(getenv("DISPLAY")),

          // initializations
          m_pProgressWnd(NULL),
          m_pWaitGrid(NULL),
          m_bButtonPressed_WaitGrid(false),
          m_pWaitList(NULL),
          m_bButtonPressed_WaitList(false),
          m_pWaitUser(NULL),
          m_pWinListManager(NULL),
          m_bOrbiterReady(false),
          m_bIsExclusiveMode(true),
          m_pRecordHandler(NULL),
          m_pWMController(NULL),
          m_pX11(NULL),
          m_pDisplay_SDL(NULL)

{
    m_nProgressWidth = 400;
    m_nProgressHeight = 200;
}

void *HackThread(void *p)
{
    // For some reason X can fail to properly die????  TODO - HACK
    cout << "Inside Hackthread" << endl;
    Sleep(30000);
    cout << "Big problem -- this app should have died and didn't.  Send ourselves a seg fault so we log a coredump and can fix this" << endl;
    kill(getpid(), SIGSEGV);
    return NULL;
}

OrbiterLinux::~OrbiterLinux()
{
    pthread_t hackthread;
    pthread_create(&hackthread, NULL, HackThread, (void*)this);

#if (USE_X11_LIB)
    if (m_pProgressWnd)
    {
        m_pProgressWnd->Terminate();
        delete m_pProgressWnd;
    }
#endif // (USE_X11_LIB)
#if (USE_WX_LIB)
    if (m_pWaitGrid)
        Safe_Close<wxDialog_WaitGrid>(m_pWaitGrid);
    if (m_pWaitList)
        Safe_Close<wxDialog_WaitList>(m_pWaitList);
    if (m_pWaitUser)
        Safe_Close<wxDialog_WaitUser>(m_pWaitUser);
#endif // (USE_WX_LIB)

    KillMaintThread();

    Destroy();
}

void OrbiterLinux::HideOtherWindows()
{
	g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::HideOtherWindows: Hidding other windows...");

	list<WinInfo> listWinInfo;
	m_pWinListManager->GetWindows(listWinInfo);

	for(list<WinInfo>::iterator it = listWinInfo.begin(); it != listWinInfo.end(); ++it)
	{
		string sClassName = it->sClassName;
        // skip the pointer-constrain window
        if (
            (sClassName == "pointer_constrain_name") ||
            (sClassName == "pointer_constrain_class") ||
            (sClassName == "pointer_constrain_name.pointer_constrain_class")
            )
        {
            continue;
        }
		if(sClassName != "" && string::npos == sClassName.find("Orbiter"))
		{
			g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::HideOtherWindows, hidding %s", sClassName.c_str());
			m_pWinListManager->HideWindow(sClassName);
		}
	}
}

void OrbiterLinux::reinitGraphics()
{
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::reinitGraphics()");

    m_pWinListManager->ShowSdlWindow(false);

    OrbiterCallBack callback = (OrbiterCallBack)&OrbiterLinux::setInputFocusToMe;
    CallMaintenanceInMiliseconds( 3000, callback, NULL, pe_ALL );
}

void OrbiterLinux::setInputFocusToMe(void *)
{
    CallMaintenanceInMiliseconds( 7000, (OrbiterCallBack)&OrbiterLinux::setInputFocusToMe, NULL, pe_ALL ); // do this every 7 seconds
}

void OrbiterLinux::setWindowName(string strWindowName)
{
    m_strWindowName = strWindowName;
}

void OrbiterLinux::setDisplayName(string strDisplayName)
{
    m_strDisplayName = strDisplayName;
}

bool OrbiterLinux::X11_Init()
{
    if ((m_pX11 != NULL) && m_pX11->IsInitialized())
    {
        g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::X11_Init(): already initialized");
        return true;
    }
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::X11_Init()");
    OrbiterSDL *pOrbiterSDL = ptrOrbiterSDL();
    if (pOrbiterSDL == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::X11_Init() : NULL dynamic_cast<OrbiterSDL *>(%p)",
                              this
                              );
    }
    // we need to use the same Display and Window with SDL
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version); // this is important!
    bool bResult = SDL_GetWMInfo(&info);
    if (bResult == false)
    {
        g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::X11_Init() : error in SDL_GetWMInfo()");
    }
    // save the SDL display
    m_pDisplay_SDL = info.info.x11.display;
    // initialize the X11wrapper
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::X11_Init() : X11wrapper");
    m_pX11 = new X11wrapper();
    m_pX11->Assign_MainWindow(info.info.x11.window);
    m_pX11->Display_Open();
    m_pX11->GetDisplaySize(m_nDesktopWidth, m_nDesktopHeight);
    // initialize other classes
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::X11_Init() : WMController");
    m_pWMController = new WMControllerImpl();
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::X11_Init() : WinListManager");
    m_pWinListManager = new WinListManager(m_pWMController, m_strWindowName);
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::X11_Init() : XRecordExtensionHandler");
    m_pRecordHandler = new XRecordExtensionHandler(m_strDisplayName);
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::X11_Init() : done");
    return true;
}

bool OrbiterLinux::X11_Exit()
{
    if (m_pX11 == NULL)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "OrbiterLinux::X11_Exit() : NULL pointer");
        return false;
    }
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::X11_Exit()");
    delete m_pRecordHandler;
    m_pRecordHandler = NULL;
    delete m_pWinListManager;
    m_pWinListManager = NULL;
    delete m_pWMController;
    m_pWMController = NULL;
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::X11_Exit() : done");
    // display was opened by SDL, it should be closed by SDL
    delete m_pX11;
    m_pX11 = NULL;
    m_pDisplay_SDL = NULL;
    return true;
}

Display * OrbiterLinux::GetDisplay()
{
    //m_pX11 should be created by now
    return m_pX11->GetDisplay();
}

Window OrbiterLinux::GetMainWindow()
{
    //m_pX11 should be created by now
    return m_pX11->GetMainWindow();
}

Display * OrbiterLinux::GetDisplay_MainWindow()
{
    return m_pDisplay_SDL;
}

OrbiterSDL * OrbiterLinux::ptrOrbiterSDL()
{
    OrbiterSDL * pOrbiterSDL = dynamic_cast<OrbiterSDL *>(this);
    if (pOrbiterSDL == NULL)
    {
        g_pPlutoLogger->Write(
            LV_CRITICAL, "OrbiterLinux::ptrOrbiterSDL() : NULL dynamic_cast<OrbiterSDL *>(%p)",
            this
            );
        return NULL;
    }
    return pOrbiterSDL;
}

bool OrbiterLinux::RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, PlutoPoint point )
{
    m_bOrbiterReady = true;
    m_bIsExclusiveMode = false;

    //Orbiter::RenderDesktop(pObj, rectTotal, point);
    vector<int> vectButtonMaps;
    GetButtonsInObject(pObj,vectButtonMaps);

    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::RenderDesktop() : ptr=%p coord=[%d,%d,%d,%d]",
                          pObj,
                          rectTotal.X, rectTotal.Y, rectTotal.Width, rectTotal.Height
                          );
    m_pWinListManager->ShowSdlWindow(false);
    if (pObj->m_ObjectType == DESIGNOBJTYPE_App_Desktop_CONST)
    {
        //g_pPlutoLogger->Write(LV_CRITICAL,"OrbiterLinux::RenderDesktop rendering of %s",pObj->m_ObjectID.c_str());
        {
            // TODO : Set now playing is not sent in video wizard
            // we'll assume this is a xine for now
            if (m_pWinListManager->GetExternApplicationName() == "")
                m_pWinListManager->SetExternApplicationName("pluto-xine-playback-window.pluto-xine-playback-window");
        }
        m_pWinListManager->SetExternApplicationPosition(rectTotal);
        ActivateExternalWindowAsync(NULL);
    }

    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::RenderDesktop() : done");
    return true;
}

/*virtual*/ void OrbiterLinux::ActivateExternalWindowAsync(void *)
{
    string sWindowName = m_pWinListManager->GetExternApplicationName();
    PlutoRectangle rectTotal;
    m_pWinListManager->GetExternApplicationPosition(rectTotal);
	g_pPlutoLogger->Write(LV_WARNING, "Is '%s' window available?", sWindowName.c_str());
    bool bIsWindowAvailable = m_pWinListManager->IsWindowAvailable(sWindowName);
	g_pPlutoLogger->Write(LV_WARNING, "==> %s", bIsWindowAvailable ? "Yes, it is!" : "No, it's NOT!");
    if (bIsWindowAvailable)
    {
        if ( (rectTotal.Width == -1) && (rectTotal.Height == -1) )
        {
            g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::ActivateExternalWindowAsync() : maximize sWindowName='%s'", sWindowName.c_str());
            m_pWinListManager->MaximizeWindow(sWindowName);
        }
        else
        {
            g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::ActivateExternalWindowAsync() : position sWindowName='%s'", sWindowName.c_str());
            // HACK: stop activating xine or wxdialog several times per second
            // TODO: do the proper activating code for windows
            m_pWinListManager->PositionWindow(sWindowName, rectTotal.X, rectTotal.Y, rectTotal.Width, rectTotal.Height);
        }
    }
    else
        CallMaintenanceInMiliseconds( bIsWindowAvailable ? 1000 : 200, (OrbiterCallBack)&OrbiterLinux::ActivateExternalWindowAsync, NULL, pe_ALL );
}

// public interface implementations below

void OrbiterLinux::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::Initialize()");

    OrbiterSDL::Initialize(Type,iPK_Room,iPK_EntertainArea);

    //we know here the ui version!
    g_pPlutoLogger->Write(LV_WARNING, "Orbiter UI Version is %d", m_iUiVersion);

    if(UsesUIVersion2())
	{
		m_pMouseBehavior = new MouseBehavior_Linux(this);
	}
    m_pRecordHandler->enableRecording(this, true);

    reinitGraphics();

    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::Initialize() : done");
}

void OrbiterLinux::InitializeAfterSetVideoMode()
{
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterSetVideoMode()");
    X11_Init();
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : HideOtherWindows");
	HideOtherWindows();
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::InitializeAfterSetVideoMode() : done");
}

void OrbiterLinux::InitializeAfterRelatives()
{
    // allow initial "extern" dialogs to receive clicks until this point
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterRelatives()");
    GrabPointer(true);
    GrabKeyboard(true);
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::InitializeAfterRelatives() : done");
}

void OrbiterLinux::Destroy()
{
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::Destroy()");
    // crash fix
    if (m_pMouseBehavior)
    {
        delete m_pMouseBehavior;
        m_pMouseBehavior = NULL;
    }
    X11_Exit();
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::Destroy() : done");
}

void OrbiterLinux::RenderScreen( bool bRenderGraphicsOnly )
{
	if( bRenderGraphicsOnly )
	{
		OrbiterSDL::RenderScreen( bRenderGraphicsOnly );
		return;
	}

    m_pWinListManager->HideAllWindows();

    m_bIsExclusiveMode = true;
    {
        X11_Locker lock(GetDisplay());
        OrbiterSDL::RenderScreen(bRenderGraphicsOnly);
        XFlush(GetDisplay()); // TODO: test and remove this
    }

    if(m_bOrbiterReady)
        m_pWinListManager->ShowSdlWindow(m_bIsExclusiveMode);
}

bool OrbiterLinux::PreprocessEvent(Orbiter::Event &event)
{
    if ( event.type != Orbiter::Event::BUTTON_DOWN && event.type != Orbiter::Event::BUTTON_UP )
        return false;

    XKeyEvent  kevent;
    KeySym   keysym;
    char   buf[1];

    kevent.type = KeyPress;
    kevent.display = GetDisplay();
    kevent.state = 0;
    kevent.keycode = event.data.button.m_iPK_Button;;
    XLookupString(&kevent, buf, sizeof(buf), &keysym, 0);

#ifdef ENABLE_MOUSE_BEHAVIOR
	if(UsesUIVersion2())
	{
		switch (event.data.button.m_iPK_Button)
		{
			case 183:  event.data.button.m_iPK_Button = BUTTON_F6_CONST; g_pPlutoLogger->Write(LV_CRITICAL, "Key F6 (gyro) %s", event.type == Orbiter::Event::BUTTON_DOWN ? "down" : "up"); return true;
			case 184:  event.data.button.m_iPK_Button = BUTTON_F7_CONST; g_pPlutoLogger->Write(LV_CRITICAL, "Key F7 (gyro) %s", event.type == Orbiter::Event::BUTTON_DOWN ? "down" : "up"); return true;
			case 185:  event.data.button.m_iPK_Button = BUTTON_F8_CONST; g_pPlutoLogger->Write(LV_CRITICAL, "Key F8 (gyro) %s", event.type == Orbiter::Event::BUTTON_DOWN ? "down" : "up"); return true;

			default:
				break;
		}
	}
#endif

	printf("KEYCode: %d %x \n", keysym, keysym);

    switch ( keysym )
    {
        case XK_Shift_L:   case XK_Shift_R:
            m_keyboardState.bShiftDown = ( event.type == Orbiter::Event::BUTTON_DOWN );
            break;
        case XK_Control_L:    case XK_Control_R:
            m_keyboardState.bControlDown = ( event.type == Orbiter::Event::BUTTON_DOWN );
            break;
        case XK_Alt_L:    case XK_Alt_R:
            m_keyboardState.bAltDown = ( event.type == Orbiter::Event::BUTTON_DOWN );
            break;
        case XK_Caps_Lock:
            if ( event.type == Orbiter::Event::BUTTON_UP )
                m_keyboardState.bCapsLock = ! m_keyboardState.bCapsLock;
            break;

        case XK_F1:     event.data.button.m_iPK_Button = BUTTON_F1_CONST; break;
        case XK_F2:     event.data.button.m_iPK_Button = BUTTON_F2_CONST; break;
        case XK_F3:     event.data.button.m_iPK_Button = BUTTON_F3_CONST; break;
        case XK_F4:     event.data.button.m_iPK_Button = BUTTON_F4_CONST; break;
        case XK_F5:     event.data.button.m_iPK_Button = BUTTON_F5_CONST; break;
		case XK_F6:     event.data.button.m_iPK_Button = BUTTON_F6_CONST; g_pPlutoLogger->Write(LV_CRITICAL, "Key F6 %s", event.type == Orbiter::Event::BUTTON_DOWN ? "down" : "up"); break;
		case XK_F7:     event.data.button.m_iPK_Button = BUTTON_F7_CONST; g_pPlutoLogger->Write(LV_CRITICAL, "Key F7 %s", event.type == Orbiter::Event::BUTTON_DOWN ? "down" : "up"); break;
		case XK_F8:     event.data.button.m_iPK_Button = BUTTON_F8_CONST; g_pPlutoLogger->Write(LV_CRITICAL, "Key F8 %s", event.type == Orbiter::Event::BUTTON_DOWN ? "down" : "up"); break;

        case XK_0: case XK_KP_0:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_right_parenthesis_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_0_CONST;
            break;
        case XK_1: case XK_KP_1:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_exclamation_point_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_1_CONST;
            break;
        case XK_2: case XK_KP_2:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_at_sign_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_2_CONST;
            break;
        case XK_3: case XK_KP_3:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_Pound_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_3_CONST;
            break;
        case XK_4: case XK_KP_4:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_dollar_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_4_CONST;
            break;
        case XK_5: case XK_KP_5:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_percent_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_5_CONST;
            break;
        case XK_6: case XK_KP_6:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_caret_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_6_CONST;
            break;
        case XK_7: case XK_KP_7:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_ampersand_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_7_CONST;
            break;
        case XK_8: case XK_KP_8:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_Asterisk_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_8_CONST;
            break;
        case XK_9: case XK_KP_9:
            if(m_keyboardState.bShiftDown)
                event.data.button.m_iPK_Button = BUTTON_left_parenthesis_CONST;
            else
                event.data.button.m_iPK_Button = BUTTON_9_CONST;
            break;

        case XK_Up:
			if(m_keyboardState.bShiftDown)
				event.data.button.m_iPK_Button = BUTTON_Shift_Up_Arrow_CONST;
			else
				event.data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST;
			break;
        case XK_Down:
			if(m_keyboardState.bShiftDown)
				event.data.button.m_iPK_Button = BUTTON_Shift_Down_Arrow_CONST;
			else
				event.data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST;
			break;
        case XK_Left:
			if(m_keyboardState.bShiftDown)
				event.data.button.m_iPK_Button = BUTTON_Shift_Left_Arrow_CONST;
			else
		      	event.data.button.m_iPK_Button = BUTTON_Left_Arrow_CONST;
            break;
        case XK_Right:
			if(m_keyboardState.bShiftDown)
				event.data.button.m_iPK_Button = BUTTON_Shift_Right_Arrow_CONST;
			else
				event.data.button.m_iPK_Button = BUTTON_Right_Arrow_CONST;
			break;

/* modificari */
        case XK_F9:     event.data.button.m_iPK_Button = BUTTON_F9_CONST; break;
        case XK_F10:    event.data.button.m_iPK_Button = BUTTON_F10_CONST; break;
        case XK_Escape: event.data.button.m_iPK_Button = BUTTON_escape_CONST; break;
        case XK_Tab:    event.data.button.m_iPK_Button = BUTTON_tab_CONST; break;
        case XK_KP_Enter: case XK_Return:       event.data.button.m_iPK_Button = BUTTON_Enter_CONST;  break;
        case XK_KP_Space:       event.data.button.m_iPK_Button = BUTTON_space_CONST; break;
        case XK_BackSpace:      event.data.button.m_iPK_Button = BUTTON_Back_CONST;  break;
        case XK_underscore:     event.data.button.m_iPK_Button = BUTTON_underscore_CONST;  break;
        case XK_minus:  event.data.button.m_iPK_Button = BUTTON_dash_CONST;  break;
        case XK_plus:   event.data.button.m_iPK_Button = BUTTON_plus_CONST;  break;
        case XK_equal:  case XK_KP_Equal:       event.data.button.m_iPK_Button = BUTTON_equals_sign_CONST;  break;
        case XK_period: event.data.button.m_iPK_Button = BUTTON_dot_CONST;  break;
        case XK_comma:  case XK_KP_Separator:   event.data.button.m_iPK_Button =  BUTTON_comma_CONST; break;
        case XK_colon:  event.data.button.m_iPK_Button = BUTTON_colon_CONST; break;
        case XK_semicolon:      event.data.button.m_iPK_Button = BUTTON_semicolumn_CONST; break;
        case XK_quotedbl:       event.data.button.m_iPK_Button = BUTTON_double_quote_CONST; break;

/* end modificari */

        default:
            if ( XK_a <= keysym && keysym <= XK_z )
			{
				if (
                    (m_keyboardState.bShiftDown && m_keyboardState.bCapsLock) ||
                    (!m_keyboardState.bShiftDown && !m_keyboardState.bCapsLock)
                    )
                {
	                event.data.button.m_iPK_Button = BUTTON_a_CONST + keysym - XK_a;
                }
				else
                {
                    event.data.button.m_iPK_Button = BUTTON_A_CONST + keysym - XK_a;
                }
			}
            else if ( XK_A <= keysym && keysym <= XK_Z )
            {
                event.data.button.m_iPK_Button = BUTTON_A_CONST + keysym - XK_A;
                std::cout<<"kb what is this:"<<'A'+keysym - XK_A<<std::endl;
            }
            else
            {
                event.type = Orbiter::Event::NOT_PROCESSED;
            }
    }

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS, "The keysym was %d, the final event type %d", keysym, event.type);
#endif

	return true;
}

void OrbiterLinux::CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage)
{
	if( sOnOff!="X" )
		return;
    Display *dpy = GetDisplay();
    Window win = DefaultRootWindow (dpy);

    // TODO: clean this code
    SDL_SysWMinfo sdlinfo;
    SDL_VERSION(&sdlinfo.version);
    int r2=SDL_GetWMInfo(&sdlinfo);
    Window w2 = sdlinfo.info.x11.wmwindow;
    Window w3 = sdlinfo.info.x11.window;
    Window w4 = sdlinfo.info.x11.fswindow;
    g_pPlutoLogger->Write(LV_CRITICAL, "GRABBING MOUSE window %d, r2: %d, w2: %d w3: %d w5: %d",(int) win,r2,(int) w2,(int) w3,(int) w4);

    win = w3;
    Pixmap blank;
    XColor dummy;
    char data[1] = {0};
    Cursor cursor;

    /* make a blank cursor */
    X11_Locker lock(GetDisplay());
    blank = XCreateBitmapFromData (dpy, win, data, 1, 1);
    if(blank == None) fprintf(stderr, "error: out of memory.\n");
    cursor = XCreatePixmapCursor(dpy, blank, blank, &dummy, &dummy, 0, 0);
    XFreePixmap (dpy, blank);
}

void OrbiterLinux::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
{
    g_pPlutoLogger->Write(LV_WARNING, "CMD off not implemented on the orbiter yet");
}

void OrbiterLinux::CMD_Activate_Window(string sWindowName,string &sCMD_Result,Message *pMessage)
{
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterLinux::CMD_Activate_Window(%s)", sWindowName.c_str());
	m_pWinListManager->SetExternApplicationName(sWindowName);
}

void OrbiterLinux::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
{
    if( m_bYieldInput )
    {
        pair<bool,int> XKeySym = PlutoButtonsToX(atoi(sPK_Button.c_str()));
        g_pPlutoLogger->Write(LV_WARNING, "Need to forward pluto key %s to X key %d (shift %d)", sPK_Button.c_str(),XKeySym.second,XKeySym.first);

        // TODO: clean this code
        Display *dpy = GetDisplay();
        X11_Locker lock(dpy);

		if( XKeySym.first )
            XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XK_Shift_L), True, 0 );

        XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XKeySym.second), True, 0 );
        XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XKeySym.second), False, 0 );

        if( XKeySym.first )
            XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XK_Shift_L), False, 0 );
    }
    Orbiter::CMD_Simulate_Keypress(sPK_Button,sName,sCMD_Result,pMessage);
}

void OrbiterLinux::CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
{
    // TODO: clean this code
    //Display *dpy = GetDisplay();
    //X11_Locker lock(dpy);
    //Window rootwindow = DefaultRootWindow (dpy);
    //g_pPlutoLogger->Write(LV_STATUS, "Moving mouse (relative %d,%d)",iPosition_X,iPosition_Y);

    //XWarpPointer(dpy, rootwindow,0 , 0, 0, 0, 0, iPosition_X, iPosition_Y);

    g_pPlutoLogger->Write(LV_STATUS, "Moving mouse (relative %d,%d)", iPosition_X, iPosition_Y);
    m_pX11->Mouse_SetPosition(iPosition_X, iPosition_Y);
}

void OrbiterLinux::CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage)
{
    g_pPlutoLogger->Write(LV_STATUS, "Clicking mouse %s",sType.c_str());

    {
        Display *pDisplay = GetDisplay();
        X11_Locker lock(pDisplay);
        XTestFakeButtonEvent(pDisplay, 1, true, 0);
        XTestFakeButtonEvent(pDisplay, 1, false, 0);
    }

    // again ?
    // TODO: clean this code
    Display *dpy = XOpenDisplay(NULL);
    XTestFakeButtonEvent(dpy, 1, true, 0);
    XTestFakeButtonEvent(dpy, 1, false, 0);
    XCloseDisplay(dpy);
}

void OrbiterLinux::CMD_Surrender_to_OS(string sOnOff, bool bFully_release_keyboard, string &sCMD_Result, Message *pMessage)
{
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::CMD_Surrender_to_OS(%s, %d)", sOnOff.c_str(), bFully_release_keyboard);
    GrabPointer(sOnOff != "1");
    GrabKeyboard(sOnOff != "1");
    Orbiter::CMD_Surrender_to_OS(sOnOff, bFully_release_keyboard, sCMD_Result, pMessage);
}

bool OrbiterLinux::DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress)
{
    std::cout << "== DisplayProgress( " << sMessage << ", ChildDevices, " << nProgress << " );" << std::endl;
/////////////////////////////////////////////////////////////////
//  WX dialogs
/////////////////////////////////////////////////////////////////
#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
	static bool bDialogRunning = false;
	PositionCallBackData *pCallBackData = new WaitUserGridCallBackData(sMessage, mapChildDevices, nProgress);
    pCallBackData->m_bShowFullScreen = true;
	CallBackType callbackType = cbOnDialogRefresh;
	if(nProgress != -1)
	{
		if(!bDialogRunning)
		{
			callbackType = cbOnDialogCreate;
			bDialogRunning = true;
		}
		//else assuming it's a refresh
	}
	else
	{
		callbackType = cbOnDialogDelete;
		bDialogRunning = false;
	}
	Task *pTask = TaskManager::Instance().CreateTask(callbackType, E_Dialog_WaitGrid, pCallBackData);
    if (callbackType == cbOnDialogCreate)
    {
        TaskManager::Instance().AddTaskAndWait(pTask);
        m_pWinListManager->MaximizeWindow("dialog.dialog");
    }
    else
    {
        if (callbackType == cbOnDialogDelete)
		{
            m_pWinListManager->HideAllWindows();
			TaskManager::Instance().AddTaskAndWait(pTask);
		}
		else
        	TaskManager::Instance().AddTask(pTask);
    }
#else // (USE_TASK_MANAGER)
    if ( (m_pWaitGrid != NULL) && m_bButtonPressed_WaitGrid )
    {
        // window already closed
        m_pWaitGrid = NULL;
        // notify by return value
        return true;
    }
    if ( (m_pWaitGrid == NULL) && (nProgress >= 0) )
    {
        // create new window
        m_pWaitGrid = Safe_CreateUnique<wxDialog_WaitGrid>();
        Safe_Show<wxDialog_WaitGrid>(m_pWaitGrid);
        m_pWinListManager->MaximizeWindow("dialog.dialog");
        return false;
    }
    if ( (m_pWaitGrid != NULL) && (nProgress >= 0) )
    {
        // update
        wxDialog_WaitGrid::Data_Refresh data_refresh = { sMessage, mapChildDevices, nProgress };
        Safe_Gui_Refresh<wxDialog_WaitGrid>(m_pWaitGrid, &data_refresh);
        return false;
    }
    if ( (m_pWaitGrid != NULL) && (nProgress < 0) )
    {
        Safe_Close<wxDialog_WaitGrid>(m_pWaitGrid);
        m_pWaitGrid = NULL;
        return false;
    }
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

/////////////////////////////////////////////////////////////////
//  NON - WX dialogs
/////////////////////////////////////////////////////////////////
#if (USE_X11_LIB)
    sMessage += ": ";
    for(map<string, bool>::const_iterator it = mapChildDevices.begin(); it != mapChildDevices.end(); ++it)
        if(!it->second)
            sMessage += StringUtils::Replace(it->first, "|", "# ") + ", ";
    std::cout << "== DisplayProgress( " << sMessage << ", ChildDevices, " << nProgress << " );" << std::endl;
    if (m_pProgressWnd && m_pProgressWnd->IsCancelled())
    {
        delete m_pProgressWnd;
        m_pProgressWnd = NULL;
        return true;
    }
    if (nProgress != -1 && !m_pProgressWnd)
    {
        // Create the progress window ...
        m_pProgressWnd = new XProgressWnd();
        m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        m_pProgressWnd->Run();
        m_pWinListManager->MaximizeWindow(m_pProgressWnd->m_wndName);
        return false;
    }
    /*else*/ if (nProgress != -1)
    {
        // Update progress info
        m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        m_pProgressWnd->DrawWindow();
        return false;
    }
    /*else*/ if(m_pProgressWnd)
    {
        // We are done here ...
        m_pProgressWnd->Terminate();
        m_pProgressWnd = NULL;
        reinitGraphics();
        return false;
    }
#endif // (USE_X11_LIB)

    return false;
}

bool OrbiterLinux::DisplayProgress(string sMessage, int nProgress)
{
    std::cout << "== DisplayProgress waitlist( " << sMessage << ", " << nProgress << " );" << std::endl;

#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
	static bool bDialogRunning = false;
	PositionCallBackData *pCallBackData = new WaitUserListCallBackData(sMessage, nProgress);
    pCallBackData->m_bShowFullScreen = true;
	CallBackType callbackType = cbOnDialogRefresh;
	if(nProgress != -1)
	{
		if(!bDialogRunning)
		{
			callbackType = cbOnDialogCreate;
			bDialogRunning = true;
        }
		//else assuming it's a refresh
	}
	else
	{
		callbackType = cbOnDialogDelete;
		bDialogRunning = false;
	}
	Task *pTask = TaskManager::Instance().CreateTask(callbackType, E_Dialog_WaitList, pCallBackData);
    if (callbackType == cbOnDialogCreate)
    {
        TaskManager::Instance().AddTaskAndWait(pTask);
        m_pWinListManager->MaximizeWindow("dialog.dialog");
    }
    else
    {
        if (callbackType == cbOnDialogDelete)
		{
            m_pWinListManager->HideAllWindows();
			TaskManager::Instance().AddTaskAndWait(pTask);
		}
		else
        	TaskManager::Instance().AddTask(pTask);
    }
#else // (USE_TASK_MANAGER)
	//TODO:
    if ( (m_pWaitList != NULL) && m_bButtonPressed_WaitList )
    {
        // window already closed
        m_pWaitList = NULL;
        // notify by return value
        return true;
    }
    if ( (m_pWaitList == NULL) && (nProgress >= 0) )
    {
        // create new window
        m_pWaitList = Safe_CreateUnique<wxDialog_WaitList>();
        Safe_Show<wxDialog_WaitList>(m_pWaitList);
        m_pWinListManager->MaximizeWindow("dialog.dialog");
        return false;
    }
    if ( (m_pWaitList != NULL) && (nProgress >= 0) )
    {
        // update
        wxDialog_WaitList::Data_Refresh data_refresh = { sMessage, nProgress };
        Safe_Gui_Refresh<wxDialog_WaitList>(m_pWaitList, &data_refresh);
        return false;
    }
	if ( (m_pWaitList != NULL) && (nProgress < 0) )
    {
        Safe_Close<wxDialog_WaitList>(m_pWaitList);
        m_pWaitList = NULL;
        return false;
    }
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

#if (USE_X11_LIB)
    if (m_pProgressWnd && m_pProgressWnd->IsCancelled())
    {
        delete m_pProgressWnd;
        m_pProgressWnd = NULL;
        return true;
    }
	if (nProgress != -1 && !m_pProgressWnd)
    {
        // Create the progress window ...
        m_pProgressWnd = new XProgressWnd();
        m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        m_pProgressWnd->Run();
        m_pWinListManager->MaximizeWindow(m_pProgressWnd->m_wndName);
        return false;
    }
    /*else*/ if (nProgress != -1)
    {
        // Update progress info
        m_pProgressWnd->UpdateProgress(sMessage, nProgress);
        m_pProgressWnd->DrawWindow();
        return false;
    }
    /*else*/ if(m_pProgressWnd)
    {
        // We are done here ...
        m_pProgressWnd->Terminate();
        m_pProgressWnd = NULL;
        reinitGraphics();
        return false;
    }
#endif // (USE_X11_LIB)

    return false;
}

int OrbiterLinux::PromptUser(string sPrompt, int iTimeoutSeconds, map<int,string> *p_mapPrompts)
{
    map<int,string> mapPrompts;
    mapPrompts[PROMPT_CANCEL]    = "Ok";
    if (p_mapPrompts == NULL) {
        p_mapPrompts = &mapPrompts;
    }
    std::cout << "== PromptUser( " << sPrompt << ", " << iTimeoutSeconds << ", " << p_mapPrompts << " );" << std::endl;

#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
	PositionCallBackData *pCallBackData = new WaitUserPromptCallBackData(sPrompt, iTimeoutSeconds, *p_mapPrompts);
    pCallBackData->m_bShowFullScreen = true;
	Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, E_Dialog_WaitUser, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);
    m_pWinListManager->MaximizeWindow("dialog.dialog");
	Task *pTaskWait = TaskManager::Instance().CreateTask(cbOnDialogWaitUser, E_Dialog_WaitUser, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTaskWait);
    std::cout << "== PromptUser( " << sPrompt << ", " << iTimeoutSeconds << ", " << p_mapPrompts << " );" << std::endl;
#else // (USE_TASK_MANAGER)
    m_pWaitUser = Safe_CreateUnique<wxDialog_WaitUser>();
    m_pWinListManager->MaximizeWindow("dialog.dialog");
    int nButtonId = Safe_ShowModal<wxDialog_WaitUser>(m_pWaitUser);
    return nButtonId;
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

#if (USE_X11_LIB)
    XPromptUser promptDlg(sPrompt, iTimeoutSeconds, p_mapPrompts);
    promptDlg.SetButtonPlacement(XPromptUser::BTN_VERT);
    promptDlg.Init();
    m_pWinListManager->MaximizeWindow(promptDlg.m_wndName);
    int nUserAnswer = promptDlg.RunModal();
    promptDlg.DeInit();
    return nUserAnswer;
#endif // (USE_X11_LIB)

	return 0;
}

/*virtual*/ ScreenHandler *OrbiterLinux::CreateScreenHandler()
{
	return new OSDScreenHandler(this, &m_mapDesignObj);
}

void OrbiterLinux::X_LockDisplay()
{
    m_pX11->Lock();
}

void OrbiterLinux::X_UnlockDisplay()
{
    m_pX11->Unlock();
}

void OrbiterLinux::GrabPointer(bool bEnable)
{
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::GrabPointer(%d)", bEnable);
    if (bEnable)
    {
        m_pX11->Mouse_Grab(GetMainWindow());
    }
    else
    {
        m_pX11->Mouse_Ungrab();
    }
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::GrabPointer(%d) : done", bEnable);
}

void OrbiterLinux::GrabKeyboard(bool bEnable)
{
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::GrabKeyboard(%d)", bEnable);
    if (bEnable)
    {
        m_pX11->Keyboard_Grab(GetMainWindow());
    }
    else
    {
        m_pX11->Keyboard_Ungrab();
    }
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::GrabKeyboard(%d) : done", bEnable);
}
