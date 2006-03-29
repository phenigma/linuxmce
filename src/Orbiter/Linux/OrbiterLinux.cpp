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
#define USE_TASK_MANAGER true //del
#include "wmtask.h"
#include "wmtaskmanager.h"
#endif // (USE_WX_LIB)

#include "DCE/Logger.h"
#include "OrbiterLinux.h"
#include "pluto_main/Define_DesignObj.h"

#include <iomanip>
#include <sstream>
#include <sys/time.h>

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include <SDL/SDL_syswm.h>

#include "pluto_main/Define_Button.h"
#include "PlutoUtils/PlutoButtonsToX.h"
#include "OSDScreenHandler.h"

#include "../wxAppMain/wx_dialog_types.h"
#include "../CallBackTypes.h"

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
          desktopInScreen(0),
          XServerDisplay(NULL),
          m_pProgressWnd(NULL),
          m_pWaitGrid(NULL),
          m_bButtonPressed_WaitGrid(false),
          m_pWaitList(NULL),
          m_bButtonPressed_WaitList(false),
          m_pWaitUser(NULL)
{
    openDisplay();

    m_pRecordHandler = new XRecordExtensionHandler(m_strDisplayName);

    m_nProgressWidth = 400;
    m_nProgressHeight = 200;

    m_sCurrentAppDesktopName = "";
}

void *HackThread(void *p)
{
    // For some reason X can fail to properly die????  TODO - HACK
    cout << "Inside Hacktrhead" << endl;
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

    delete m_pRecordHandler;
    closeDisplay();
}

void OrbiterLinux::reinitGraphics()
{
    if ( ! XServerDisplay && ! openDisplay() )
        return;

    commandRatPoison(":set winname class");
    commandRatPoison(":desktop off");

    commandRatPoison(string(":select ") + m_strWindowName);
    commandRatPoison(":desktop on");
    commandRatPoison(":keystodesktop on");
    commandRatPoison(":keybindings off");
    setDesktopVisible(false);

    OrbiterCallBack callback = (OrbiterCallBack)&OrbiterLinux::setInputFocusToMe;
    CallMaintenanceInMiliseconds( 3000, callback, NULL, pe_ALL );
}

void OrbiterLinux::setInputFocusToMe(void *)
{
    if ( ! m_bYieldInput )
        commandRatPoison(":keystodesktop on");

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

bool OrbiterLinux::openDisplay()
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

bool OrbiterLinux::closeDisplay()
{
    if ( XServerDisplay )
        XCloseDisplay(XServerDisplay);

    XServerDisplay = NULL;

    return true;
}


Display *OrbiterLinux::getDisplay()
{
    if ( ! XServerDisplay )
        openDisplay();

    return XServerDisplay;
}

Window OrbiterLinux::getWindow()
{
    return 0;
}

bool OrbiterLinux::RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, PlutoPoint point )
{
	/*
    {
        if(pObj->m_ObjectType == DESIGNOBJTYPE_wxWidgets_Applet_CONST)
            SetCurrentAppDesktopName("dialog");
        else if(pObj->m_ObjectType == DESIGNOBJTYPE_App_Desktop_CONST)
            SetCurrentAppDesktopName("pluto-xine-playback-window");
    }
	*/

    vector<int> vectButtonMaps;
    GetButtonsInObject(pObj,vectButtonMaps);

    g_pPlutoLogger->Write(LV_STATUS, "OrbiterLinux::RenderDesktop() : name='%s' ptr=%p coord=[%d,%d,%d,%d]",
                          m_sCurrentAppDesktopName.c_str(), pObj,
                          rectTotal.Left(), rectTotal.Top(), rectTotal.Right(), rectTotal.Bottom()
                          );
    resizeMoveDesktop(rectTotal.Left(), rectTotal.Top(),
                      rectTotal.Right() - rectTotal.Left(),
                      rectTotal.Bottom() - rectTotal.Top());

    desktopInScreen = true;
    return true;
}

// public interface implementations below
bool OrbiterLinux::resizeMoveDesktop(int x, int y, int width, int height)
{
    if ( ! m_bYieldInput )
    {
        m_pRecordHandler->enableRecording(this, false);
		//this command will add a command to a hook: when a window is switched/focused, all the hooks added for this event will be executed
		//in this case, when xine or other will grap the keyboards, "keystodesktop on" command will be executed
		//and orbiter will get right away to control of the keyboard
		//
		//Command: addhook hook command
		//Add a command to hook. When the hook is run, command will be executed.
		//
		commandRatPoison(":addhook switchwin keystodesktop on");


        commandRatPoison(":keystodesktop on");
    }
    else
    {
		//this command will remove a command from the hook. see above the comments.
		commandRatPoison(":remhook switchwin keystodesktop on");
        commandRatPoison(":keystodesktop off");

        // patch the rectangle to match the actual resolution
        width = m_nDesktopWidth;
        height = m_nDesktopHeight;
        m_pRecordHandler->enableRecording(this);
    }

	/*
	 *
    if(m_sCurrentAppDesktopName != "")
        commandRatPoison(":select " + m_sCurrentAppDesktopName);
	*/

    stringstream commandLine;
    commandLine << ":set padding " << x << " " << y << " "
                << m_nDesktopWidth - x - width << " " << m_nDesktopHeight - y - height;

    commandRatPoison(commandLine.str());
    commandRatPoison(":redisplay");

    return true;
}

bool OrbiterLinux::setDesktopVisible(bool visible)
{
    if ( ! visible )
        return resizeMoveDesktop(m_nDesktopWidth, m_nDesktopHeight, 10, 10);
    else
        return resizeMoveDesktop(0, 0, m_nDesktopWidth - m_iImageWidth, m_nDesktopHeight - m_iImageHeight);
}

void OrbiterLinux::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
    OrbiterSDL::Initialize(Type,iPK_Room,iPK_EntertainArea);
    reinitGraphics();
}

void OrbiterLinux::RenderScreen()
{
    if ( XServerDisplay == NULL && ! openDisplay() )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't open the display: \"%s\"", m_strDisplayName.c_str());

        return;
    }

    desktopInScreen = false;

    OrbiterSDL::RenderScreen();

    if ( false == desktopInScreen )
        setDesktopVisible(false);

    XFlush(XServerDisplay);
}

/**
   void OrbiterLinux::RenderObject(
   DesignObj_Controller *pObj, DesignObj_Controller *pObjAttr,
   int XOffset, int YOffset)
   {
   if ( pObj->m_ObjectType == C_DESIGNOBJTYPE_APP_DESKTOP_CONST )
   {
   desktopInScreen = true;

   resizeMoveDesktop(
   pObj->m_rPosition.Left(),
   pObj->m_rPosition.Top(),
   pObj->m_rPosition.Width,
   pObj->m_rPosition.Height);

   return;
   }

   ControllerImageSDL::RenderObject(pObj, pObjAttr, XOffset, YOffset);
   }
*/

bool OrbiterLinux::PreprocessEvent(Orbiter::Event &event)
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

    switch ( keysym )
    {
        case XK_F1:     event.data.button.m_iPK_Button = BUTTON_F1_CONST; break;
        case XK_F2:     event.data.button.m_iPK_Button = BUTTON_F2_CONST; break;
        case XK_F3:     event.data.button.m_iPK_Button = BUTTON_F3_CONST; break;
        case XK_F4:     event.data.button.m_iPK_Button = BUTTON_F4_CONST; break;
        case XK_F5:     event.data.button.m_iPK_Button = BUTTON_F5_CONST; break;

        case XK_0: case XK_KP_0:    event.data.button.m_iPK_Button = BUTTON_0_CONST; break;
        case XK_1: case XK_KP_1:    event.data.button.m_iPK_Button = BUTTON_1_CONST; break;
        case XK_2: case XK_KP_2:    event.data.button.m_iPK_Button = BUTTON_2_CONST; break;
        case XK_3: case XK_KP_3:    event.data.button.m_iPK_Button = BUTTON_3_CONST; break;
        case XK_4: case XK_KP_4:    event.data.button.m_iPK_Button = BUTTON_4_CONST; break;
        case XK_5: case XK_KP_5:    event.data.button.m_iPK_Button = BUTTON_5_CONST; break;
        case XK_6: case XK_KP_6:    event.data.button.m_iPK_Button = BUTTON_6_CONST; break;
        case XK_7: case XK_KP_7:    event.data.button.m_iPK_Button = BUTTON_7_CONST; break;
        case XK_8: case XK_KP_8:    event.data.button.m_iPK_Button = BUTTON_8_CONST; break;
        case XK_9: case XK_KP_9:    event.data.button.m_iPK_Button = BUTTON_9_CONST; break;

        case XK_Up:         event.data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST; break;
        case XK_Down:       event.data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST; break;
        case XK_Left:       event.data.button.m_iPK_Button = BUTTON_Left_Arrow_CONST; break;
        case XK_Right:      event.data.button.m_iPK_Button = BUTTON_Right_Arrow_CONST; break;

        default:
            if ( XK_a <= keysym && keysym <= XK_z )
                event.data.button.m_iPK_Button = BUTTON_a_CONST + keysym - XK_a;
            else if ( XK_A <= keysym && keysym <= XK_Z )
                event.data.button.m_iPK_Button = BUTTON_A_CONST + keysym - XK_A;
            else
                event.type = Orbiter::Event::NOT_PROCESSED;
    }

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS, "The keysym was %d, the final event type %d", keysym, event.type);
#endif

	return true;
}

void OrbiterLinux::CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage)
{
    // not need this anymore
    // in Xine_Player : pointer_hide, pointer_show
    //bool bShow = sOnOff=="1";
    //if ( bShow )
    //    commandRatPoison(":banish off");
    //else
    //    commandRatPoison(":banish on");
}

void OrbiterLinux::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
{
    g_pPlutoLogger->Write(LV_WARNING, "CMD off not implemented on the orbiter yet");
}

void OrbiterLinux::CMD_Activate_Window(string sName,string &sCMD_Result,Message *pMessage)
{
    commandRatPoison(string(":select ") + sName);
}

void OrbiterLinux::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
{
    if( m_bYieldInput )
    {
        pair<bool,int> XKeySym = PlutoButtonsToX(atoi(sPK_Button.c_str()));
        g_pPlutoLogger->Write(LV_WARNING, "Need to forward pluto key %s to X key %d (shift %d)", sPK_Button.c_str(),XKeySym.second,XKeySym.first);

        Display *dpy = XOpenDisplay (NULL);

		if( XKeySym.first )
            XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XK_Shift_L), True, 0 );

        XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XKeySym.second), True, 0 );
        XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XKeySym.second), False, 0 );

        if( XKeySym.first )
            XTestFakeKeyEvent( dpy, XKeysymToKeycode(dpy, XK_Shift_L), False, 0 );

        XCloseDisplay(dpy);
    }
    Orbiter::CMD_Simulate_Keypress(sPK_Button,sName,sCMD_Result,pMessage);
}

void OrbiterLinux::CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
{
    Display *dpy = XOpenDisplay (NULL);
    Window rootwindow = DefaultRootWindow (dpy);
    g_pPlutoLogger->Write(LV_STATUS, "Moving mouse (relative %d,%d)",iPosition_X,iPosition_Y);

    XWarpPointer(dpy, rootwindow,0 , 0, 0, 0, 0, iPosition_X, iPosition_Y);
    XCloseDisplay(dpy);
}

void OrbiterLinux::CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage)
{
    g_pPlutoLogger->Write(LV_STATUS, "Clicking mouse %s",sType.c_str());

    XTestFakeButtonEvent(XServerDisplay, 1, true, 0);
    XTestFakeButtonEvent(XServerDisplay, 1, false, 0);
    Display *dpy = XOpenDisplay (NULL);
    XTestFakeButtonEvent(dpy, 1, true, 0);
    XTestFakeButtonEvent(dpy, 1, false, 0);
    XCloseDisplay(dpy);
}

bool OrbiterLinux::DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress)
{
/////////////////////////////////////////////////////////////////
//  WX dialogs
/////////////////////////////////////////////////////////////////
#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
	static bool bDialogRunning = false;
	CallBackData *pCallBackData = new WaitUserGridCallBackData(sMessage, mapChildDevices, nProgress);
	CallBackType callbackType = cbOnRefreshWxWidget;
	if(nProgress != -1)
	{
		if(!bDialogRunning)
		{
			callbackType = cbOnCreateWxWidget;
			bDialogRunning = true;
            {
                // little hack: also delete the other dialogs
                TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitList, NULL));
                TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitUser, NULL));
            }
		}
		//else assuming it's a refresh
	}
	else
	{
        {
            // little hack: also delete the other dialogs
            TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitList, NULL));
            TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitUser, NULL));
        }
		callbackType = cbOnDeleteWxWidget;
		bDialogRunning = false;
	}
	WMTask *pTask = TaskManager::Instance().CreateTask(callbackType, E_Dialog_WaitGrid, pCallBackData);
	//syncthis TaskManager::Instance().AddTask(pTask);
	TaskManager::Instance().AddTaskAndWait(pTask);
#else // (USE_TASK_MANAGER)
    if ( (m_pWaitGrid != NULL) && m_bButtonPressed_WaitGrid )
    {
        // little hack: also delete the other dialogs
        if (m_pWaitList)
        {
            Safe_Close(m_pWaitList);
            m_pWaitList = NULL;
        }
        if (m_pWaitUser)
        {
            Safe_Close(m_pWaitUser);
            m_pWaitUser = NULL;
        }
        // window already closed
        m_pWaitGrid = NULL;
        // notify by return value
        return true;
    }
    if ( (m_pWaitGrid == NULL) && (nProgress >= 0) )
    {
        // little hack: also delete the other dialogs
        if (m_pWaitList)
        {
            Safe_Close(m_pWaitList);
            m_pWaitList = NULL;
        }
        if (m_pWaitUser)
        {
            Safe_Close(m_pWaitUser);
            m_pWaitUser = NULL;
        }
        // create new window
        m_pWaitGrid = Safe_CreateUnique<wxDialog_WaitGrid>();
        Safe_Show<wxDialog_WaitGrid>(m_pWaitGrid);
        commandRatPoison(":set winname class");
        commandRatPoison(":desktop off");
        commandRatPoison(string(":select ") + "dialog");
        commandRatPoison(":desktop on");
        commandRatPoison(":keystodesktop on");
        commandRatPoison(":keybindings off");
        setDesktopVisible(false);
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
        commandRatPoison(":set winname class");
        commandRatPoison(":desktop off");
        commandRatPoison(string(":select ") + m_pProgressWnd->m_wndName);
        commandRatPoison(":desktop on");
        commandRatPoison(":keystodesktop on");
        commandRatPoison(":keybindings off");
        setDesktopVisible(false);
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
    std::cout << "== DisplayProgress( " << sMessage << ", " << nProgress << " );" << std::endl;

#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
	static bool bDialogRunning = false;
	CallBackData *pCallBackData = new WaitUserListCallBackData(sMessage, nProgress);
	CallBackType callbackType = cbOnRefreshWxWidget;
	if(nProgress != -1)
	{
		if(!bDialogRunning)
		{
			callbackType = cbOnCreateWxWidget;
			bDialogRunning = true;
            {
                // little hack: also delete the other dialogs
                TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitGrid, NULL));
                TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitUser, NULL));
            }
		}
		//else assuming it's a refresh
	}
	else
	{
		callbackType = cbOnDeleteWxWidget;
		bDialogRunning = false;
        {
            // little hack: also delete the other dialogs
            TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitGrid, NULL));
            TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitUser, NULL));
        }
	}
	WMTask *pTask = TaskManager::Instance().CreateTask(callbackType, E_Dialog_WaitList, pCallBackData);
	//syncthis TaskManager::Instance().AddTask(pTask);
	TaskManager::Instance().AddTaskAndWait(pTask);
#else // (USE_TASK_MANAGER)
	//TODO:
    if ( (m_pWaitList != NULL) && m_bButtonPressed_WaitList )
    {
        // little hack: also delete the other dialogs
        if (m_pWaitGrid)
        {
            Safe_Close(m_pWaitGrid);
            m_pWaitGrid = NULL;
        }
        if (m_pWaitUser)
        {
            Safe_Close(m_pWaitUser);
            m_pWaitUser = NULL;
        }
        // window already closed
        m_pWaitList = NULL;
        // notify by return value
        return true;
    }
    if ( (m_pWaitList == NULL) && (nProgress >= 0) )
    {
        // little hack: also delete the other dialogs
        if (m_pWaitGrid)
        {
            Safe_Close(m_pWaitGrid);
            m_pWaitGrid = NULL;
        }
        if (m_pWaitUser)
        {
            Safe_Close(m_pWaitUser);
            m_pWaitUser = NULL;
        }
        // create new window
        m_pWaitList = Safe_CreateUnique<wxDialog_WaitList>();
        Safe_Show<wxDialog_WaitList>(m_pWaitList);
        commandRatPoison(":set winname class");
        commandRatPoison(":desktop off");
        commandRatPoison(string(":select ") + "dialog");
        commandRatPoison(":desktop on");
        commandRatPoison(":keystodesktop on");
        commandRatPoison(":keybindings off");
        setDesktopVisible(false);
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
        commandRatPoison(":set winname class");
        commandRatPoison(":desktop off");
        commandRatPoison(string(":select ") + m_pProgressWnd->m_wndName);
        commandRatPoison(":desktop on");
        commandRatPoison(":keystodesktop on");
        commandRatPoison(":keybindings off");
        setDesktopVisible(false);
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
    //return 0;
    //return PROMPT_CANCEL;
    map<int,string> mapPrompts;
    mapPrompts[PROMPT_CANCEL]    = "Ok";
    if (p_mapPrompts == NULL) {
        p_mapPrompts = &mapPrompts;
    }
    std::cout << "== PromptUser( " << sPrompt << ", " << iTimeoutSeconds << ", " << p_mapPrompts << " );" << std::endl;

#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
    {
        // little hack: also delete the other dialogs
        TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitGrid, NULL));
        TaskManager::Instance().AddTaskAndWait(TaskManager::Instance().CreateTask(cbOnDeleteWxWidget, E_Dialog_WaitList, NULL));
    }
	CallBackData *pCallBackData = new WaitUserPromptCallBackData(sPrompt, iTimeoutSeconds, *p_mapPrompts);
	CallBackType callbackType = cbOnCreateWxWidget;
	WMTask *pTask = TaskManager::Instance().CreateTask(callbackType, E_Dialog_WaitUser, pCallBackData);
	TaskManager::Instance().AddTaskAndWait(pTask);
#else // (USE_TASK_MANAGER)
    // little hack: also delete the other dialogs
    if (m_pWaitGrid)
    {
        Safe_Close(m_pWaitGrid);
        m_pWaitGrid = NULL;
    }
    if (m_pWaitList)
    {
        Safe_Close(m_pWaitList);
        m_pWaitList = NULL;
    }
    m_pWaitUser = Safe_CreateUnique<wxDialog_WaitUser>();
    commandRatPoison(":set winname class");
    commandRatPoison(":desktop off");
    commandRatPoison(string(":select ") + "dialog");
    commandRatPoison(":desktop on");
    commandRatPoison(":keystodesktop on");
    commandRatPoison(":keybindings off");
    setDesktopVisible(false);
    int nButtonId = Safe_ShowModal<wxDialog_WaitUser>(m_pWaitUser);
    return nButtonId;
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

#if (USE_X11_LIB)
    XPromptUser promptDlg(sPrompt, iTimeoutSeconds, p_mapPrompts);
    promptDlg.SetButtonPlacement(XPromptUser::BTN_VERT);
    promptDlg.Init();
    commandRatPoison(":set winname class");
    commandRatPoison(":desktop off");
    commandRatPoison(string(":select ") + promptDlg.m_wndName);
    commandRatPoison(":desktop on");
    commandRatPoison(":keystodesktop on");
    commandRatPoison(":keybindings off");
    setDesktopVisible(false);
    int nUserAnswer = promptDlg.RunModal();
    promptDlg.DeInit();
    return nUserAnswer;
#endif // (USE_X11_LIB)
}

ScreenHandler *OrbiterLinux::CreateScreenHandler()
{
	return new OSDScreenHandler(this, &m_mapDesignObj);
}

void OrbiterLinux::SetCurrentAppDesktopName(string sName)
{
    g_pPlutoLogger->Write( LV_WARNING, "OrbiterLinux::SetCurrentAppDesktopName(%s)", sName.c_str());
	m_sCurrentAppDesktopName = sName;
}

//void OrbiterLinux::OnReload()
//{
//	WMTask *pTask = TaskManager::Instance().CreateTask(cbReloadOrbiter, E_Dialog_None, NULL);
//	TaskManager::Instance().AddTask(pTask);
//}

//void OrbiterLinux::OnQuit()
//{
//	WMTask *pTask = TaskManager::Instance().CreateTask(cbReloadOrbiter, E_Dialog_None, NULL);
//	TaskManager::Instance().AddTask(pTask);
//}
