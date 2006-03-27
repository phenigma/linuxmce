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
#include "wxdialog_roomwizard.h"
#include "wxdialog_waitgrid.h"
#include "wxdialog_waitlist.h"
#include "wxdialog_waituser.h"
#include "wx_thread_bag.h"
#include "wx_extern_app.h"

#ifdef USE_RELEASE_CODE
#include "../Linux/wmtaskmanager.h"
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

    END_EVENT_TABLE()
    ;

/*!
 * Constructor for wxAppMain
 */

wxAppMain::wxAppMain()
        : v_pExternApp(NULL)
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
    mainWindow->Raise();
    mainWindow->Show(false);
#endif // USE_RELEASE_CODE
#ifdef USE_DEBUG_CODE
    _debug_init();
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
        else
        {
            Extern_Event_Listener(v_pExternApp);
        }
    } // g_USE_EXTERN_APP_ON_THREAD
////@begin wxEVT_IDLE event handler in wxAppMain.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_IDLE event handler in wxAppMain.
    wxApp::OnIdle(event);
}

wxAppMain::~wxAppMain()
{
    _WX_LOG_NFO();
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
    _WX_LOG_NFO("Received event : %s", _str_event_dialog(event));
#ifdef USE_DEBUG_CODE
    Data_Holder_Dialog *pData_Holder_Dialog = wx_static_cast(Data_Holder_Dialog *, event.GetClientData());
    event.Skip();
    _COND_RET(pData_Holder_Dialog != NULL);
    event.Skip(false);
    E_ACTION_TYPE action = (E_ACTION_TYPE)event.GetId();
    E_DIALOG_TYPE e_dialog_type = pData_Holder_Dialog->e_dialog_type;
    _WX_LOG_NFO("Received command event : class='%s' action='%s'", _str_enum(e_dialog_type), _str_enum(action));
    switch (action)
    {
        case E_Action_Close:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Close<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Close<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Close<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Close<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return;
            }
            break;
        }
        case E_Action_Create:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->pWindow = Safe_Create<wxDialog_RoomWizard>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->pWindow = Safe_Create<wxDialog_WaitGrid>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->pWindow = Safe_Create<wxDialog_WaitList>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->pWindow = Safe_Create<wxDialog_WaitUser>(pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return;
            }
            break;
        }
        case E_Action_Create_Unique:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->pWindow = Safe_CreateUnique<wxDialog_RoomWizard>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->pWindow = Safe_CreateUnique<wxDialog_WaitGrid>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->pWindow = Safe_CreateUnique<wxDialog_WaitList>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->pWindow = Safe_CreateUnique<wxDialog_WaitUser>(pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return;
            }
            break;
        }
        case E_Action_DataLoad:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Gui_DataLoad<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Gui_DataLoad<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Gui_DataLoad<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Gui_DataLoad<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return;
            }
            break;
        }
        case E_Action_DataSave:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Gui_DataSave<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Gui_DataSave<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Gui_DataSave<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Gui_DataSave<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return;
            }
            break;
        }
        case E_Action_Refresh:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Gui_Refresh<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Gui_Refresh<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Gui_Refresh<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Gui_Refresh<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return;
            }
            break;
        }
        case E_Action_Show:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Show<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Show<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Show<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Show<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return;
            }
            break;
        }
        case E_Action_ShowModal:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->nRetCode = Safe_ShowModal<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->nRetCode = Safe_ShowModal<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->nRetCode = Safe_ShowModal<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->nRetCode = Safe_ShowModal<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return;
            }
            break;
        }
        default:
            _WX_LOG_ERR("bad action : %d", action);
            break;
    } // switch (action)
    wx_semaphore_post(pData_Holder_Dialog->oSemaphore);
#endif // USE_DEBUG_CODE
}
