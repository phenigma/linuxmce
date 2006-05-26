#include "OrbiterRendererSDLLinux.h"

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
#include "OSDScreenHandler.h"
#include "MouseBehavior_Linux.h"

bool g_useX11LOCK = (! getenv("PLUTO_DISABLE_X11LOCK"));

OrbiterRendererSDLLinux::OrbiterRendererSDLLinux() :
          m_strWindowName("Orbiter"),
          m_strDisplayName(getenv("DISPLAY")),

          // initializations
          XServerDisplay(NULL),
          m_pProgressWnd(NULL),
          m_pWaitGrid(NULL),
          m_bButtonPressed_WaitGrid(false),
          m_pWaitList(NULL),
          m_bButtonPressed_WaitList(false),
          m_pWaitUser(NULL),
          m_WinListManager(m_strWindowName),
          m_bOrbiterReady(false),
          m_bIsExclusiveMode(true)
{
    openDisplay();
    m_pRecordHandler = new XRecordExtensionHandler(m_strDisplayName);

    m_nProgressWidth = 400;
    m_nProgressHeight = 200;

	HideOtherWindows();
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


OrbiterRendererSDLLinux::~OrbiterRendererSDLLinux()
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

    delete m_pRecordHandler;

    closeDisplay();
}

void OrbiterRendererSDLLinux::HideOtherWindows()
{
	g_pPlutoLogger->Write(LV_WARNING, "OrbiterRendererSDLLinux::HideOtherWindows: Hidding other windows...");

	list<WinInfo> listWinInfo;
	m_WinListManager.GetWindows(listWinInfo);

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
			g_pPlutoLogger->Write(LV_STATUS, "OrbiterRendererSDLLinux::HideOtherWindows, hidding %s", sClassName.c_str());
			m_WinListManager.HideWindow(sClassName);
		}
	}
}

void OrbiterRendererSDLLinux::reinitGraphics()
{
    g_pPlutoLogger->Write(LV_WARNING, "OrbiterRendererSDLLinux::reinitGraphics()");
    if ( ! XServerDisplay && ! openDisplay() )
        return;

    m_WinListManager.ShowSdlWindow(false);

    OrbiterCallBack callback = (OrbiterCallBack)&OrbiterRendererSDLLinux::setInputFocusToMe;
	m_pOrbiter->CallMaintenanceInMiliseconds( 3000, callback, NULL, pe_ALL, this );
}

void OrbiterRendererSDLLinux::setInputFocusToMe(void *)
{
	m_pOrbiter->CallMaintenanceInMiliseconds( 7000, (OrbiterCallBack)&OrbiterRendererSDLLinux::setInputFocusToMe, NULL, pe_ALL, this ); // do this every 7 seconds
}

void OrbiterRendererSDLLinux::setWindowName(string strWindowName)
{
    m_strWindowName = strWindowName;
}

void OrbiterRendererSDLLinux::setDisplayName(string strDisplayName)
{
    m_strDisplayName = strDisplayName;
}

bool OrbiterRendererSDLLinux::openDisplay()
{
    XServerDisplay = XOpenDisplay(m_strDisplayName.c_str());

    int currentScreen;

    if ( XServerDisplay == NULL )
        return false;

    XLockDisplay(XServerDisplay);
    currentScreen = XDefaultScreen(XServerDisplay);
    m_nDesktopWidth = DisplayWidth(XServerDisplay, currentScreen);
    m_nDesktopHeight = DisplayHeight(XServerDisplay, currentScreen);
    XUnlockDisplay(XServerDisplay);
    return true;
}

bool OrbiterRendererSDLLinux::closeDisplay()
{
    if ( XServerDisplay )
        XCloseDisplay(XServerDisplay);

    XServerDisplay = NULL;

    return true;
}

// we need to use the same Display and Window with SDL
bool Get_SDL_SysWMinfo(SDL_SysWMinfo &info)
{
    SDL_VERSION(&info.version); // this is important!
    bool bResult = SDL_GetWMInfo(&info);
    if (bResult == false)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Get_SDL_SysWMinfo() : error in SDL_GetWMInfo()");
        return false;
    }
    return true;
}

Display *OrbiterRendererSDLLinux::getDisplay()
{
    if ( XServerDisplay )
        return XServerDisplay;

    SDL_SysWMinfo info;
    if (Get_SDL_SysWMinfo(info))
    {
        XServerDisplay = info.info.x11.display;
    }
    else
    {
        g_pPlutoLogger->Write(LV_WARNING, "OrbiterRendererSDLLinux::getDisplay() : error in SDL_GetWMInfo()");
    }

    if ( XServerDisplay == NULL )
        openDisplay();
    return XServerDisplay;
}

Window OrbiterRendererSDLLinux::getWindow()
{
    Window window = 0;

    SDL_SysWMinfo info;
    if (Get_SDL_SysWMinfo(info))
    {
        window = info.info.x11.window;
    }
    else
    {
        g_pPlutoLogger->Write(LV_WARNING, "OrbiterRendererSDLLinux::getDisplay() : error in SDL_GetWMInfo()");
    }

    return window;
}

void OrbiterRendererSDLLinux::X_LockDisplay()
{
    if (g_useX11LOCK)
        XLockDisplay(XServerDisplay);
}

void OrbiterRendererSDLLinux::X_UnlockDisplay()
{
    if (g_useX11LOCK)
    {
        XSync(XServerDisplay, false);
        XUnlockDisplay(XServerDisplay);
    }
}

int OrbiterRendererSDLLinux::PromptUser(string sPrompt, int iTimeoutSeconds, map<int,string> *p_mapPrompts)
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
    m_WinListManager.MaximizeWindow("dialog.dialog");
	Task *pTaskWait = TaskManager::Instance().CreateTask(cbOnDialogWaitUser, E_Dialog_WaitUser, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTaskWait);
    std::cout << "== PromptUser( " << sPrompt << ", " << iTimeoutSeconds << ", " << p_mapPrompts << " );" << std::endl;
#else // (USE_TASK_MANAGER)
    m_pWaitUser = Safe_CreateUnique<wxDialog_WaitUser>();
    m_WinListManager.MaximizeWindow("dialog.dialog");
    int nButtonId = Safe_ShowModal<wxDialog_WaitUser>(m_pWaitUser);
    return nButtonId;
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

#if (USE_X11_LIB)
    XPromptUser promptDlg(sPrompt, iTimeoutSeconds, p_mapPrompts);
    promptDlg.SetButtonPlacement(XPromptUser::BTN_VERT);
    promptDlg.Init();
    m_WinListManager.MaximizeWindow(promptDlg.m_wndName);
    int nUserAnswer = promptDlg.RunModal();
    promptDlg.DeInit();
    return nUserAnswer;
#endif // (USE_X11_LIB)

	return 0;
}

void OrbiterRendererSDLLinux::GrabPointer(bool bEnable)
{
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterRendererSDLLinux::GrabPointer(%d)", bEnable);
    Display *pDisplay = getDisplay();
    Window window = getWindow();
    XLockDisplay(pDisplay);
    int code = 0;
    if (bEnable)
    {
        code = XGrabPointer(
            pDisplay, window, false,
            ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask | PointerMotionHintMask,
            GrabModeAsync, GrabModeAsync,
            None, //win,
            None, //cursor,
            CurrentTime
            );
    }
    else
    {
        code = XUngrabPointer(pDisplay, CurrentTime);
    }
    XSync(pDisplay, false);
    {
        // for GrabModeSync
        // XAllowEvents(pDisplay, AsyncPointer, CurrentTime);
        //XSync(pDisplay, false);
    }
    XUnlockDisplay(pDisplay);
    g_pPlutoLogger->Write(LV_STATUS, "End OrbiterRendererSDLLinux::GrabPointer(%d) : %s", bEnable, X11_ErrorText(pDisplay, code).c_str());
}

void OrbiterRendererSDLLinux::GrabKeyboard(bool bEnable)
{
    g_pPlutoLogger->Write(LV_STATUS, "OrbiterRendererSDLLinux::GrabKeyboard(%d)", bEnable);
    Display *pDisplay = getDisplay();
    Window window = getWindow();
    XLockDisplay(pDisplay);
    int code = 0;
    if (bEnable)
    {
        code = XGrabKeyboard(
        pDisplay, window, false,
        GrabModeAsync, GrabModeAsync,
        CurrentTime
        );
    }
    else
    {
        code = XUngrabKeyboard(pDisplay, CurrentTime);
    }
    XSync(pDisplay, false);
    {
        // for GrabModeSync
        // XAllowEvents(pDisplay, AsyncKeyboard, CurrentTime);
        //XSync(pDisplay, false);
    }
    XUnlockDisplay(pDisplay);
    g_pPlutoLogger->Write(LV_STATUS, "End OrbiterRendererSDLLinux::GrabKeyboard(%d) : %s", bEnable, X11_ErrorText(pDisplay, code).c_str());
}


bool OrbiterRendererSDLLinux::RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, PlutoPoint point )
{
    m_bOrbiterReady = true;
    m_bIsExclusiveMode = false;

    //Orbiter::RenderDesktop(pObj, rectTotal, point);
    vector<int> vectButtonMaps;
	m_pOrbiter->GetButtonsInObject(pObj,vectButtonMaps);

    g_pPlutoLogger->Write(LV_STATUS, "OrbiterRendererSDLLinux::RenderDesktop() : ptr=%p coord=[%d,%d,%d,%d]",
                          pObj,
                          rectTotal.X, rectTotal.Y, rectTotal.Width, rectTotal.Height
                          );
    m_WinListManager.ShowSdlWindow(false);
    if (pObj->m_ObjectType == DESIGNOBJTYPE_App_Desktop_CONST)
    {
        //g_pPlutoLogger->Write(LV_CRITICAL,"OrbiterRendererSDLLinux::RenderDesktop rendering of %s",pObj->m_ObjectID.c_str());
        {
            // TODO : Set now playing is not sent in video wizard
            // we'll assume this is a xine for now
            if (m_WinListManager.GetExternApplicationName() == "")
                m_WinListManager.SetExternApplicationName("pluto-xine-playback-window.pluto-xine-playback-window");
        }
        m_WinListManager.SetExternApplicationPosition(rectTotal);
        ActivateExternalWindowAsync(NULL);
    }

    g_pPlutoLogger->Write(LV_WARNING, "OrbiterRendererSDLLinux::RenderDesktop() : done");
    return true;
}

/*virtual*/ void OrbiterRendererSDLLinux::ActivateExternalWindowAsync(void *)
{
    string sWindowName = m_WinListManager.GetExternApplicationName();
    PlutoRectangle rectTotal;
    m_WinListManager.GetExternApplicationPosition(rectTotal);
	g_pPlutoLogger->Write(LV_WARNING, "Is '%s' window available?", sWindowName.c_str());
    bool bIsWindowAvailable = m_WinListManager.IsWindowAvailable(sWindowName);
	g_pPlutoLogger->Write(LV_WARNING, "==> %s", bIsWindowAvailable ? "Yes, it is!" : "No, it's NOT!");
    if (bIsWindowAvailable)
    {
        if ( (rectTotal.Width == -1) && (rectTotal.Height == -1) )
        {
            g_pPlutoLogger->Write(LV_WARNING, "OrbiterRendererSDLLinux::ActivateExternalWindowAsync() : maximize sWindowName='%s'", sWindowName.c_str());
            m_WinListManager.MaximizeWindow(sWindowName);
        }
        else
        {
            g_pPlutoLogger->Write(LV_WARNING, "OrbiterRendererSDLLinux::ActivateExternalWindowAsync() : position sWindowName='%s'", sWindowName.c_str());
            // HACK: stop activating xine or wxdialog several times per second
            // TODO: do the proper activating code for windows
            m_WinListManager.PositionWindow(sWindowName, rectTotal.X, rectTotal.Y, rectTotal.Width, rectTotal.Height);
        }
    }
    else
        m_pOrbiter->CallMaintenanceInMiliseconds( bIsWindowAvailable ? 1000 : 200, (OrbiterCallBack)&OrbiterRendererSDLLinux::ActivateExternalWindowAsync, NULL, pe_ALL );
}

// public interface implementations below

void OrbiterRendererSDLLinux::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
    //we know here the ui version!
    g_pPlutoLogger->Write(LV_WARNING, "Orbiter UI Version is %d", m_pOrbiter->m_iUiVersion);

    if(m_pOrbiter->UsesUIVersion2())
	{
		m_pOrbiter->m_pMouseBehavior = new MouseBehavior_Linux();
	}
    m_pRecordHandler->enableRecording(m_pOrbiter->, true);

    reinitGraphics();
    g_pPlutoLogger->Write(LV_WARNING, "status of new calls to X(Un)LockDisplay : %d, env-variable PLUTO_DISABLE_X11LOCK %s", g_useX11LOCK, (! g_useX11LOCK) ? "exported" : "unset" );
    GrabPointer(true);
    GrabKeyboard(true);
}

void OrbiterRendererSDLLinux::RenderScreen( bool bRenderGraphicsOnly )
{
	if( bRenderGraphicsOnly )
	{
		OrbiterSDL::RenderScreen( bRenderGraphicsOnly );
		return;
	}

    if ( XServerDisplay == NULL && ! openDisplay() )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't open the display: \"%s\"", m_strDisplayName.c_str());

        return;
    }

    m_WinListManager.HideAllWindows();

    m_bIsExclusiveMode = true;
    OrbiterSDL::RenderScreen(bRenderGraphicsOnly);

    if(m_bOrbiterReady)
        m_WinListManager.ShowSdlWindow(m_bIsExclusiveMode);

    XFlush(XServerDisplay);
}

bool OrbiterRendererSDLLinux::PreprocessEvent(Orbiter::Event &event)
{
    if ( event.type != Orbiter::Event::BUTTON_DOWN && event.type != Orbiter::Event::BUTTON_UP )
        return false;

    XKeyEvent  kevent;
    KeySym   keysym;
    char   buf[1];

    kevent.type = KeyPress;
    kevent.display = getDisplay();
    kevent.state = 0;
    kevent.keycode = event.data.button.m_iPK_Button;;
    XLookupString(&kevent, buf, sizeof(buf), &keysym, 0);

#ifdef ENABLE_MOUSE_BEHAVIOR
	if(m_pOrbiter->UsesUIVersion2())
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
bool OrbiterRendererSDLLinux::DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress)
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
        m_WinListManager.MaximizeWindow("dialog.dialog");
    }
    else
    {
        if (callbackType == cbOnDialogDelete)
		{
            m_WinListManager.HideAllWindows();
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
        m_WinListManager.MaximizeWindow("dialog.dialog");
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
        m_WinListManager.MaximizeWindow(m_pProgressWnd->m_wndName);
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

bool OrbiterRendererSDLLinux::DisplayProgress(string sMessage, int nProgress)
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
        m_WinListManager.MaximizeWindow("dialog.dialog");
    }
    else
    {
        if (callbackType == cbOnDialogDelete)
		{
            m_WinListManager.HideAllWindows();
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
        m_WinListManager.MaximizeWindow("dialog.dialog");
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
        m_WinListManager.MaximizeWindow(m_pProgressWnd->m_wndName);
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
