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
#include "wx_win_thread.h"
#include "wxevent_dialog.h"
#include "wxdialog_base.h"
#include "wxdialog_roomwizard.h"
#include "wxdialog_waitgrid.h"
#include "wxdialog_waitlist.h"
#include "wxdialog_waituser.h"
#include "wxthread_bag.h"
#include "wx_extern_app.h"

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
EVT_TIMER(ID_Timer_Debug, wxAppMain::OnTimer_Debug)
EVT_TIMER(ID_Timer_ExternApp, wxAppMain::OnTimer_ExternApp)

END_EVENT_TABLE()
;

/*!
 * Constructor for wxAppMain
 */

wxAppMain::wxAppMain()
        : v_oTimer_Debug(this, ID_Timer_Debug)
        , v_oTimer_ExternApp(this, ID_Timer_ExternApp)
        , v_pExternApp(NULL)
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
#ifdef USE_RELEASE_CODE
    g_USE_EXTERN_APP_ON_THREAD = true;
#endif // USE_RELEASE_CODE
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
    mainWindow->Raise();
    mainWindow->Show(false);
#endif // USE_RELEASE_CODE

    if (g_USE_EXTERN_APP_ON_TIMER)
    {
        v_oTimer_ExternApp.Start(INTERVAL_EXTERNAPP_LOOP_MSEC);
    } // g_USE_EXTERN_APP_ON_TIMER
#ifdef USE_DEBUG_CODE
    v_oTimer_Debug.Start(INTERVAL_TIMER_DEBUG_MSEC);
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
    if (g_USE_EXTERN_APP_ON_THREAD)
    {
        if ( wxAppIsReady() && (v_pExternApp==NULL) )
        {
            v_pExternApp = new ExternApp(wxTheApp->argc, wxTheApp->argv);
            v_pExternApp->Start();
        }
    } // g_USE_EXTERN_APP_ON_THREAD
    if (g_USE_EXTERN_APP_ON_IDLE)
    {
        if ( wxAppIsReady() )
        {
            if (v_pExternApp==NULL)
            {
                v_pExternApp = new ExternApp(wxTheApp->argc, wxTheApp->argv);
            }
            else
            {
                if (! v_pExternApp->EventProcess())
                {
                    wxTheApp->GetTopWindow()->Destroy();
                }
                wx_sleep(0, INTERVAL_EXTERNAPP_LOOP_MSEC);
            }
            ::wxWakeUpIdle();
        }
    } // g_USE_EXTERN_APP_ON_IDLE
    wxApp::OnIdle(event);
////@begin wxEVT_IDLE event handler in wxAppMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_IDLE event handler in wxAppMain.
}

wxAppMain::~wxAppMain()
{
    _WX_LOG_NFO();
    v_oTimer_Debug.Stop();
    v_oTimer_ExternApp.Stop();
    wxDELETE(v_pExternApp);
    global_wx_log_leave();
    OnExit();
}

int wxAppMain::OnRun()
{
    _WX_LOG_NFO();
    wxApp::OnRun();
    _WX_LOG_NFO("exit code : %d", g_nExitCode);
    return g_nExitCode;
}

void wxAppMain::OnEvent_Dialog(wxCommandEvent& event)
{
    // cannot use RTTI, because the object is not even created
    Data_Holder_Dialog *pData_Holder_Dialog = wx_static_cast(Data_Holder_Dialog *, event.GetClientData());
    _COND_RET(pData_Holder_Dialog != NULL);
    event.Skip();
    E_DIALOG_ACTION action = (E_DIALOG_ACTION)event.GetId();
    if (action == E_Dialog_Refresh)
        return;
    E_wxDialog_Class_Type class_type = pData_Holder_Dialog->e_class_type;
    _WX_LOG_NFO("Received command event : class='%s' action='%s'", _str_enum(class_type), _str_enum(action));
    switch (action)
    {
        case E_Dialog_Create:
        {
            switch (class_type)
            {
                case E_wxDialog_RoomWizard:
                    pData_Holder_Dialog->pWindow = wxDialog_Create<wxDialog_RoomWizard>(pData_Holder_Dialog->idWindow, pData_Holder_Dialog->sLabel, pData_Holder_Dialog->pExternData);
                    break;
                case E_wxDialog_WaitGrid:
                    pData_Holder_Dialog->pWindow = wxDialog_Create<wxDialog_WaitGrid>(pData_Holder_Dialog->idWindow, pData_Holder_Dialog->sLabel, pData_Holder_Dialog->pExternData);
                    break;
                case E_wxDialog_WaitList:
                    pData_Holder_Dialog->pWindow = wxDialog_Create<wxDialog_WaitList>(pData_Holder_Dialog->idWindow, pData_Holder_Dialog->sLabel, pData_Holder_Dialog->pExternData);
                    break;
                case E_wxDialog_WaitUser:
                    pData_Holder_Dialog->pWindow = wxDialog_Create<wxDialog_WaitUser>(pData_Holder_Dialog->idWindow, pData_Holder_Dialog->sLabel, pData_Holder_Dialog->pExternData);
                    break;
                default:
                    _WX_LOG_ERR("unknown class_type : %d", class_type);
                    return;
            }
            break;
        }
        case E_Dialog_Create_Unique:
        {
            switch (class_type)
            {
                case E_wxDialog_RoomWizard:
                    pData_Holder_Dialog->pWindow = wxDialog_CreateUnique<wxDialog_RoomWizard>(pData_Holder_Dialog->idWindow, pData_Holder_Dialog->sLabel, pData_Holder_Dialog->pExternData);
                    break;
                case E_wxDialog_WaitGrid:
                    pData_Holder_Dialog->pWindow = wxDialog_CreateUnique<wxDialog_WaitGrid>(pData_Holder_Dialog->idWindow, pData_Holder_Dialog->sLabel, pData_Holder_Dialog->pExternData);
                    break;
                case E_wxDialog_WaitList:
                    pData_Holder_Dialog->pWindow = wxDialog_CreateUnique<wxDialog_WaitList>(pData_Holder_Dialog->idWindow, pData_Holder_Dialog->sLabel, pData_Holder_Dialog->pExternData);
                    break;
                case E_wxDialog_WaitUser:
                    pData_Holder_Dialog->pWindow = wxDialog_CreateUnique<wxDialog_WaitUser>(pData_Holder_Dialog->idWindow, pData_Holder_Dialog->sLabel, pData_Holder_Dialog->pExternData);
                    break;
                default:
                    _WX_LOG_ERR("unknown class_type : %d", class_type);
                    return;
            }
            break;
        }
        case E_Dialog_Show:
        {
            switch (class_type)
            {
                case E_wxDialog_RoomWizard:
                    pData_Holder_Dialog->bRetCode = wxDialog_Show<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow);
                    break;
                case E_wxDialog_WaitGrid:
                    pData_Holder_Dialog->bRetCode = wxDialog_Show<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow);
                    break;
                case E_wxDialog_WaitList:
                    pData_Holder_Dialog->bRetCode = wxDialog_Show<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow);
                    break;
                case E_wxDialog_WaitUser:
                    pData_Holder_Dialog->bRetCode = wxDialog_Show<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow);
                    break;
                default:
                    _WX_LOG_ERR("unknown class_type : %d", class_type);
                    return;
            }
            break;
        }
        case E_Dialog_ShowModal:
        {
            switch (class_type)
            {
                case E_wxDialog_RoomWizard:
                    pData_Holder_Dialog->nRetCode = wxDialog_ShowModal<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow);
                    break;
                case E_wxDialog_WaitGrid:
                    pData_Holder_Dialog->nRetCode = wxDialog_ShowModal<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow);
                    break;
                case E_wxDialog_WaitList:
                    pData_Holder_Dialog->nRetCode = wxDialog_ShowModal<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow);
                    break;
                case E_wxDialog_WaitUser:
                    pData_Holder_Dialog->nRetCode = wxDialog_ShowModal<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow);
                    break;
                default:
                    _WX_LOG_ERR("unknown class_type : %d", class_type);
                    return;
            }
            break;
        }
        case E_Dialog_Close:
        {
            switch (class_type)
            {
                case E_wxDialog_RoomWizard:
                    wxDialog_Close<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow);
                    break;
                case E_wxDialog_WaitGrid:
                    wxDialog_Close<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow);
                    break;
                case E_wxDialog_WaitList:
                    wxDialog_Close<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow);
                    break;
                case E_wxDialog_WaitUser:
                    wxDialog_Close<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow);
                    break;
                default:
                    _WX_LOG_ERR("unknown class_type : %d", class_type);
                    return;
            }
            break;
        }
        default:
            _WX_LOG_ERR("unknown action : %d", action);
            break;
    } // switch (action)
    wx_semaphore_post(pData_Holder_Dialog->oSemaphore);
}

void wxAppMain::OnTimer_ExternApp(wxTimerEvent& event)
{
    //_WX_LOG_NFO();
    _COND_RET(event.GetId() == ID_Timer_ExternApp);
    event.Skip();
    if (g_USE_EXTERN_APP_ON_TIMER)
    {
        static bool isRunningNow = false;
        if (isRunningNow)
        {
            _WX_LOG_WRN("Running lazy event process");
            return;
        }
        isRunningNow = true;
        if (! v_pExternApp->EventProcess())
        {
            v_oTimer_ExternApp.Stop();
            wxTheApp->GetTopWindow()->Destroy();
        }
        isRunningNow = false;
    } // g_USE_EXTERN_APP_ON_TIMER
}

void wxAppMain::OnTimer_Debug(wxTimerEvent& event)
{
    //_WX_LOG_NFO();
    _COND_RET(event.GetId() == ID_Timer_Debug);
    event.Skip();
#ifdef DEBUG_REFRESH_ON_TIMER
    _debug_refresh_update();
#endif // DEBUG_REFRESH_ON_TIMER
}
