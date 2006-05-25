//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxappmain.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wxappmain.h"
#include "wx_event_dialog.h"
#include "wx_safe_dialog.h"
#include "wxdialog_base.h"
#include "wx_thread_bag.h"
#include "wx_extern_app.h"

#ifdef USE_RELEASE_CODE
#include "../TaskManager.h"
#endif // USE_RELEASE_CODE

////@begin XPM images
////@end XPM images

/*!
 * Application instance implementation
 */

#ifdef USE_MAIN_CONSOLE

IMPLEMENT_APP_NO_MAIN( wxAppMain );
IMPLEMENT_WX_THEME_SUPPORT;

#else // USE_MAIN_CONSOLE

////@begin implement app
IMPLEMENT_APP( wxAppMain )
////@end implement app
    ;

#endif // USE_MAIN_CONSOLE

/*!
 * wxAppMain type definition
 */

IMPLEMENT_CLASS( wxAppMain, wxApp )
    ;

/*!
 * wxAppMain event table definition
 */

BEGIN_EVENT_TABLE( wxAppMain, wxApp )

////@begin wxAppMain event table entries
    EVT_IDLE( wxAppMain::OnIdle )

////@end wxAppMain event table entries

    EVTC_DIALOG(wxID_ANY, wxAppMain::OnEvent_Dialog)
    EVT_TIMER(ID_Timer_WakeIdle, wxAppMain::OnTimer_WakeIdle)

    END_EVENT_TABLE()
    ;

/*!
 * Constructor for wxAppMain
 */

wxAppMain::wxAppMain()
        : v_bExiting(false)
        , v_pwxThread_Bag(NULL)
        , v_pExternApp(NULL)
        , v_oTimer_WakeIdle(this, ID_Timer_WakeIdle)
{
    global_wx_log_enter();
    _WX_LOG_NFO();
////@begin wxAppMain member initialisation
////@end wxAppMain member initialisation
}

/*!
 * Initialisation for wxAppMain
 */

bool wxAppMain::OnInit()
{
    _WX_LOG_NFO();
    _COND_RET(this == wxTheApp, false);
////@begin wxAppMain initialisation
    // Remove the comment markers above and below this block
    // to make permanent changes to the code.

#if wxUSE_XPM
    wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
    wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif
    wxFrameMain* mainWindow = new wxFrameMain;
    mainWindow->Create( NULL, ID_FRAMEMAIN );
    mainWindow->Show(true);
////@end wxAppMain initialisation

    SetTopWindow(mainWindow);
#ifdef USE_RELEASE_CODE
    // hide main window, not all window managers support this
    // not handled by the external app
    mainWindow->Raise();
    mainWindow->Show(false);
#endif // USE_RELEASE_CODE
#ifdef USE_DEBUG_CODE
    _debug_global_init();
#endif // USE_DEBUG_CODE

    v_pwxThread_Bag = new wxThread_Bag;
    v_oTimer_WakeIdle.Start(INTERVAL_TIMER_WAKEIDLE_MSEC);

#ifdef USE_DEBUG_CODE
#ifdef WX_SDL_DEMO
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        _WX_LOG_ERR("unable to init SDL: %s", SDL_GetError());
        return false;
    }
    // Setup video mode, but don't create a window
    SDL_SetVideoMode(0, 0, 0, SDL_SWSURFACE);
#endif // WX_SDL_DEMO
#endif // USE_DEBUG_CODE

    return true;
}

/*!
 * Cleanup for wxAppMain
 */
int wxAppMain::OnExit()
{
    // NO wx log call if NO DontCreateOnDemand()
    _WX_LOG_NFO();
////@begin wxAppMain cleanup
    return wxApp::OnExit();
////@end wxAppMain cleanup
}

/*!
 * wxEVT_IDLE event handler
 */

void wxAppMain::OnIdle( wxIdleEvent& event )
{
    //_WX_LOG_NFO();
#ifdef USE_RELEASE_CODE
    if (App_ShouldExit())
    {
        _WX_LOG_NFO("Received Exit Signal");
        Clean_Exit();
    }
    else if ( App_IsReady() && (v_pExternApp==NULL) )
    {
        v_pExternApp = new ExternApp(wxTheApp->argc, wxTheApp->argv);
        v_pExternApp->Start();
    }
    else
    {
        Extern_Task_Listener();
    }
#endif // USE_RELEASE_CODE
#ifdef USE_DEBUG_CODE
    {
        Extern_Task_Listener();
    }
#endif // USE_DEBUG_CODE
////@begin wxEVT_IDLE event handler in wxAppMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_IDLE event handler in wxAppMain.
    wxApp::OnIdle(event);
}

wxAppMain::~wxAppMain()
{
    _WX_LOG_NFO();
    v_oTimer_WakeIdle.Stop();
    wxDELETE(v_pwxThread_Bag);
    global_wx_log_leave();
    OnExit();
}

int wxAppMain::OnRun()
{
    _WX_LOG_NFO();
    wxApp::OnRun();
    v_oTimer_WakeIdle.Stop();
    _WX_LOG_NFO("ExitCode == %d", App_GetExitCode());
    return App_GetExitCode();
}

void wxAppMain::Clean_Exit(bool bDestroyTopWindow/*=true*/)
{
    _WX_LOG_NFO();
    if (v_bExiting)
        return;
    v_bExiting = true;
    v_oTimer_WakeIdle.Stop();
    wxGetApp().ptr_ThreadBag()->DestroyAll();
    v_pExternApp = NULL; // object already destroyed
    if (bDestroyTopWindow)
    {
        wxWindow *pTopWindow = GetTopWindow();
        if (pTopWindow)
        {
            _WX_LOG_NFO("Closing top window %s", pTopWindow->GetLabel().c_str());
            pTopWindow->Destroy();
        }
    }
#ifdef USE_DEBUG_CODE
#ifdef WX_SDL_DEMO
    // cleanup SDL
    SDL_Quit();
#endif // WX_SDL_DEMO
#endif // USE_DEBUG_CODE
}

wxThread_Bag * wxAppMain::ptr_ThreadBag() const
{
    //_WX_LOG_NFO();
    return v_pwxThread_Bag;
}

ExternApp * wxAppMain::ptr_ExternApp() const
{
    //_WX_LOG_NFO();
    if (App_ShouldExit())
        return NULL;
    return v_pExternApp;
}

void wxAppMain::OnEvent_Dialog(wxCommandEvent& event)
{
    _WX_LOG_NFO("Received event : %s", _str_event(event));
    Data_Holder_Dialog *pData_Holder_Dialog = wx_static_cast(Data_Holder_Dialog *, event.GetClientData());
    _COND_RET(pData_Holder_Dialog != NULL);
    Process_Dialog_Action(pData_Holder_Dialog->e_dialog_type, (E_ACTION_TYPE)event.GetId(), pData_Holder_Dialog);
}

void wxAppMain::OnTimer_WakeIdle(wxTimerEvent& event)
{
    //_WX_LOG_NFO();
    _COND_RET(event.GetId() == ID_Timer_WakeIdle);
    if (! App_IsReady())
        return;
    if (v_bExiting)
        return;
    ::wxWakeUpIdle();
}
