//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_extern_app.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_extern_app.h"
#include "wx_safe_dialog.h"

#ifdef USE_RELEASE_CODE
bool g_USE_EXTERN_APP_ON_THREAD = true;
#else
bool g_USE_EXTERN_APP_ON_THREAD = false;
#endif // USE_RELEASE_CODE

#ifdef USE_RELEASE_CODE
#  include "../Main.h"
#  include "../SDL/StartOrbiterSDL.h"
#  include "../Linux/wmtaskmanager.h"
#endif // USE_RELEASE_CODE

ExternApp::ExternApp(int argc, char *argv[])
        : wxThread_Cmd(wxTHREAD_JOINABLE, "ExternApp")
        , argc(argc)
        , argv(argv)
{
    _WX_LOG_NFO();
#ifdef USE_RELEASE_CODE
    v_pSDL_App_Object = NULL;
#endif // USE_RELEASE_CODE
}

ExternApp::~ExternApp()
{
    _WX_LOG_NFO();
#ifdef USE_RELEASE_CODE
    wxDELETE(v_pSDL_App_Object);
#endif // USE_RELEASE_CODE
}

void ExternApp::Run()
{
    _WX_LOG_NFO();
    if (! Create())
        return;
    EventLoop();
    Destroy();
}

bool ExternApp::Create()
{
    _WX_LOG_NFO();
#ifdef USE_RELEASE_CODE
    v_pSDL_App_Object = new SDL_App_Object(argc, argv);
    if (! v_pSDL_App_Object->LoadConfig())
    {
        _WX_LOG_ERR("error returned by : LoadConfig()");
        wxDELETE(v_pSDL_App_Object);
        return false;
    }
    if (! v_pSDL_App_Object->Create())
    {
        _WX_LOG_ERR("error returned by : Create()");
        wxDELETE(v_pSDL_App_Object);
        return false;
    }
#endif // USE_RELEASE_CODE
    return true;
}

bool ExternApp::EventLoop()
{
    _WX_LOG_NFO("Event Loop");
#ifdef USE_RELEASE_CODE
    while (v_pSDL_App_Object->EventProcess())
    {
        if (wxIdleThreadShouldStop())
        {
            _WX_LOG_WRN("Event process loop stopped");
            return false;
        }
    }
#endif // USE_RELEASE_CODE
    return true;
}

int ExternApp::Destroy()
{
    int nExitCode = 0;
#ifdef USE_RELEASE_CODE
    v_pSDL_App_Object->Destroy();
    wxDELETE(v_pSDL_App_Object);
#endif // USE_RELEASE_CODE
    return nExitCode;
}

#ifdef USE_RELEASE_CODE
const char * _str_enum(const CallBackType &value)
{
    switch (value)
    {
        CASE_const_ret_str(cbOnCreateWxWidget);
        CASE_const_ret_str(cbOnDeleteWxWidget);
        CASE_const_ret_str(cbOnRefreshWxWidget);
        default:
            _WX_LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}
#endif // USE_RELEASE_CODE

void Extern_Event_Listener(ExternApp *pExternApp)
{
    //_WX_LOG_NFO();
    if (pExternApp == NULL)
        return;
#ifdef USE_RELEASE_CODE
    if (pExternApp->v_pSDL_App_Object == NULL)
        return;
    WMTaskManager *pTaskManager = &TaskManager::Instance();
    WMTask *pTask = pTaskManager->PopTask();
    if (pTask == NULL)
        return;
    E_DIALOG_TYPE e_dialog_type = pTask->DialogType;
    CallBackType action = pTask->TaskType;
    void * pData = pTask->pCallBackData;
    _WX_LOG_NFO("Received command event : class='%s' action='%s'", _str_enum(e_dialog_type), _str_enum(action));
    switch (action)
    {
        case cbOnCreateWxWidget:
        {
            Data_Holder_Dialog data_holder_dialog(e_dialog_type, NULL, pData);
            wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_Create_Unique, "", &data_holder_dialog);
            switch (e_dialog_type)
            {
                case E_Dialog_WaitUser:
                    wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_ShowModal, "", &data_holder_dialog);
                    break;
                default:
                    wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_Show, "", &data_holder_dialog);
                    break;
            }
            break;
        }
        case cbOnDeleteWxWidget:
        {
            Data_Holder_Dialog data_holder_dialog(e_dialog_type, NULL, pData);
            wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_Close, "", &data_holder_dialog);
            break;
        }
        case cbOnRefreshWxWidget:
        {
            Data_Holder_Dialog data_holder_dialog(e_dialog_type, NULL, pData);
            wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_Refresh, "", &data_holder_dialog);
            break;
        }
        default:
            _WX_LOG_ERR("bad action : %d", action);
            break;
    } // switch (action)
    pTaskManager->TaskProcessed(pTask->TaskId);
#endif // USE_RELEASE_CODE
}
