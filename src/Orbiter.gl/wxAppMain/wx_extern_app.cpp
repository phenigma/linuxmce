//
// Author : Remus C.
//
// Changed by : ...
//

// support code for external app, launched in a separate thread

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
#  include "../TaskManager.h"
#endif // USE_RELEASE_CODE

ExternApp::ExternApp(int argc, char *argv[])
        : wxThread_Cmd(wxTHREAD_JOINABLE, "ExternApp")
        , argc(argc)
        , argv(argv)
{
    _LOG_NFO();
#ifdef USE_RELEASE_CODE
    v_pSDL_App_Object = NULL;
#endif // USE_RELEASE_CODE
}

ExternApp::~ExternApp()
{
    _LOG_NFO();
    Destroy();
}

void ExternApp::Run()
{
    _LOG_NFO();
    if (! Create())
        return;
    EventLoop();
    Destroy();
}

bool ExternApp::Create()
{
    _LOG_NFO();
#ifdef USE_RELEASE_CODE
    v_pSDL_App_Object = new SDL_App_Object(argc, argv);
    if (! v_pSDL_App_Object->LoadConfig())
    {
        _LOG_ERR("error returned by : LoadConfig()");
        wxDELETE(v_pSDL_App_Object);
        return false;
    }
    if (! v_pSDL_App_Object->Create())
    {
        _LOG_ERR("error returned by : Create()");
        wxDELETE(v_pSDL_App_Object);
        return false;
    }
#endif // USE_RELEASE_CODE
    return true;
}

bool ExternApp::EventLoop()
{
    _LOG_NFO("Event Loop Start");
#ifdef USE_RELEASE_CODE
    while (v_pSDL_App_Object->EventProcess())
    {
        if (wxIdleThreadShouldStop())
        {
            _LOG_WRN("Event process loop stopped");
            return false;
        }
    }
#endif // USE_RELEASE_CODE
    _LOG_NFO("Event Loop End");
    return true;
}

void ExternApp::Destroy()
{
    _LOG_NFO();
#ifdef USE_RELEASE_CODE
    if (! v_pSDL_App_Object)
        return;
    SetExitCode(v_pSDL_App_Object->GetExitCode());
    wxDELETE(v_pSDL_App_Object);
#endif // USE_RELEASE_CODE
}

#ifdef USE_RELEASE_CODE
const char * _str_enum(const CallBackType &value)
{
    switch (value)
    {
        CASE_const_ret_str(cbOnDialogCreate);
        CASE_const_ret_str(cbOnDialogDelete);
        CASE_const_ret_str(cbOnDialogRefresh);
        CASE_const_ret_str(cbOnDialogSave);
        CASE_const_ret_str(cbOnDialogWaitUser);
        default:
            _LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}
#endif // USE_RELEASE_CODE

void Extern_Task_Listener()
{
#ifdef USE_RELEASE_CODE
    ExternApp *pExternApp = wxGetApp().ptr_ExternApp();
    if (pExternApp == NULL)
        return;
    if (! pExternApp->IsRunning())
        return;
#endif // USE_RELEASE_CODE
    //_LOG_NFO();
    Task *pTask = TaskManager::Instance().PopTask();
    if (pTask == NULL)
    {
        //_LOG_NFO("No task to process, sleeping");
        return;
    }
    E_DIALOG_TYPE e_dialog_type = pTask->DialogType;
    CallBackType action = pTask->TaskType;
    _LOG_NFO("Received command event : class='%s' action='%s'", _str_enum(e_dialog_type), _str_enum(action));
    Data_Holder_Dialog data_holder_dialog(e_dialog_type, NULL, pTask->pCallBackData);
    switch (action)
    {
        case cbOnDialogCreate:
        {
            Process_Dialog_Action(e_dialog_type, E_Action_Create_Unique, &data_holder_dialog);
            Process_Dialog_Action(e_dialog_type, E_Action_Show, &data_holder_dialog);
            Extern_Task_Data *pExtern_Task_Data = new Extern_Task_Data();
            pExtern_Task_Data->nEventId = pTask->TaskId;
            data_holder_dialog.pCallBackData = pExtern_Task_Data;
            Process_Dialog_Action(e_dialog_type, E_Action_WaitInitialized, &data_holder_dialog);
            break;
        }
        case cbOnDialogDelete:
        {
            Process_Dialog_Action(e_dialog_type, E_Action_Close, &data_holder_dialog);
            break;
        }
        case cbOnDialogRefresh:
        {
            Process_Dialog_Action(e_dialog_type, E_Action_Refresh, &data_holder_dialog);
            break;
        }
        case cbOnDialogSave:
        {
            Process_Dialog_Action(e_dialog_type, E_Action_DataSave, &data_holder_dialog);
            break;
        }
        case cbOnDialogWaitUser:
        {
            Extern_Task_Data *pExtern_Task_Data = new Extern_Task_Data();
            pExtern_Task_Data->nEventId = pTask->TaskId;
            data_holder_dialog.pCallBackData = pExtern_Task_Data;
            Process_Dialog_Action(e_dialog_type, E_Action_WaitUser, &data_holder_dialog);
            return;
        }
        default:
        {
            _LOG_ERR("bad action : %d", action);
            break;
        }
    } // switch (action)
    _LOG_NFO("Processed command event : class='%s' action='%s'", _str_enum(e_dialog_type), _str_enum(action));
    wxDELETE(pTask->pCallBackData); // this is the request
    TaskManager::Instance().TaskProcessed(pTask->TaskId);
#ifdef USE_RELEASE_CODE
#endif // USE_RELEASE_CODE
}

void Extern_Task_Response(Extern_Task_Data *pExtern_Task_Data)
{
    _LOG_NFO("pExtern_Task_Data=%p", pExtern_Task_Data);
    _COND_RET(pExtern_Task_Data != NULL);
#ifdef USE_RELEASE_CODE
    ExternApp *pExternApp = wxGetApp().ptr_ExternApp();
    if (pExternApp == NULL)
        return;
    if (! pExternApp->IsRunning())
        return;
#endif // USE_RELEASE_CODE
    TaskManager::Instance().TaskProcessed(pExtern_Task_Data->nEventId);
    // pExtern_Task_Data will be deleted
}
