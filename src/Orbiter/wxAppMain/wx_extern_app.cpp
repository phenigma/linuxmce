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
#include "wxappmain.h"
#include "wx_safe_dialog.h"

#ifdef USE_RELEASE_CODE
#  include "../Main.h"
#  include "../SDL/StartOrbiterSDL.h"
#  include "../Linux/OrbiterLinux.h"
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
    Destroy();
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
    _WX_LOG_NFO("Event Loop Start");
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
    _WX_LOG_NFO("Event Loop End");
    return true;
}

int ExternApp::Destroy()
{
    int nExitCode = 0;
#ifdef USE_RELEASE_CODE
    if (! v_pSDL_App_Object)
        return nExitCode;
    _WX_LOG_NFO("1");//del
    nExitCode = v_pSDL_App_Object->GetExitCode();
    _WX_LOG_NFO("2");//del
    SetExitCode(nExitCode);
    _WX_LOG_NFO("3");//del
    wxDELETE(v_pSDL_App_Object);
    _WX_LOG_NFO("4");//del
#endif // USE_RELEASE_CODE
    return nExitCode;
}

#ifdef USE_RELEASE_CODE
const char * _str_enum(const CallBackType &value)
{
    switch (value)
    {
        CASE_const_ret_str(cbOnWxWidgetCreate);
        CASE_const_ret_str(cbOnWxWidgetDelete);
        CASE_const_ret_str(cbOnWxWidgetRefresh);
        CASE_const_ret_str(cbOnWxWidgetSave);
        CASE_const_ret_str(cbOnWxWidgetWaitUser);
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
    {
        _WX_LOG_NFO("No task to process, sleeping");
        wx_sleep(0, EXTERN_APP_EVENT_SLEEP_MSEC);
        return;
    }
    E_DIALOG_TYPE e_dialog_type = pTask->DialogType;
    CallBackType action = pTask->TaskType;
    void * pData = pTask->pCallBackData;
    _WX_LOG_NFO("Received command event : class='%s' action='%s'", _str_enum(e_dialog_type), _str_enum(action));
    Data_Holder_Dialog data_holder_dialog(e_dialog_type, NULL, pData);
    switch (action)
    {
        case cbOnWxWidgetCreate:
            Process_Dialog_Action(e_dialog_type, E_Action_Create_Unique, &data_holder_dialog);
            Process_Dialog_Action(e_dialog_type, E_Action_Show, &data_holder_dialog);
            break;
        case cbOnWxWidgetDelete:
            Process_Dialog_Action(e_dialog_type, E_Action_Close, &data_holder_dialog);
            break;
        case cbOnWxWidgetRefresh:
            Process_Dialog_Action(e_dialog_type, E_Action_Refresh, &data_holder_dialog);
            break;
        case cbOnWxWidgetSave:
            Process_Dialog_Action(e_dialog_type, E_Action_DataSave, &data_holder_dialog);
            break;
        case cbOnWxWidgetWaitUser:
        {
            //Extern_Event_Data *pExtern_Event_Data = new Extern_Event_Data();
            //pExtern_Event_Data->nEventId = pTask->TaskId;
            //data_holder_dialog.pExternData = pExtern_Event_Data;
            Process_Dialog_Action(e_dialog_type, E_Action_WaitUser, &data_holder_dialog);
            _WX_LOG_WRN("cbOnWxWidgetWaitUser");
            wx_sleep(5);//del
            _WX_LOG_ERR("cbOnWxWidgetWaitUser END");
            break;
        }
        default:
            _WX_LOG_ERR("bad action : %d", action);
            break;
    } // switch (action)
    _WX_LOG_NFO("End command event : class='%s' action='%s'", _str_enum(e_dialog_type), _str_enum(action));
    pTaskManager->TaskProcessed(pTask->TaskId);
#endif // USE_RELEASE_CODE
}

void Extern_Event_Response(Extern_Event_Data *pExtern_Event_Data)
{
    _WX_LOG_NFO();
    _COND_RET(pExtern_Event_Data != NULL);
    ExternApp *pExternApp = wxGetApp().ptr_ExternApp();
    if (pExternApp == NULL)
        return;
#ifdef USE_RELEASE_CODE
    if (pExternApp->v_pSDL_App_Object == NULL)
        return;
    WMTaskManager *pTaskManager = &TaskManager::Instance();
    pTaskManager->TaskProcessed(pExtern_Event_Data->nEventId);
#endif // USE_RELEASE_CODE
#ifdef USE_DEBUG_CODE
    wxUnusedVar(pExtern_Event_Data);
#endif // USE_DEBUG_CODE
}
