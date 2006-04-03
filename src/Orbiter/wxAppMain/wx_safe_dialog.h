//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_SAFE_DIALOG_H_
#define _WX_SAFE_DIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_safe_dialog.h"
#endif

#include "wx_dialog_types.h"
#include "wx_event_dialog.h"

class wxDialog_Base;
struct Extern_Event_Data;

struct Data_Holder_Base
{
    Data_Holder_Base()
        : oSemaphore(0, 1)
        , bInThread(! ::wxIsMainThread())
        {
        }
    wxSemaphore oSemaphore; // wait until the action is finished
    bool bInThread; // where the object was created
};

struct Data_Holder_Dialog : Data_Holder_Base
{
    Data_Holder_Dialog()
            : e_dialog_type(E_Dialog_Undefined)
            , pWindow(NULL)
            , pExternData(NULL)
            , bShow(true)
            , bRetCode(true)
            , nRetCode(0)
        {
        }
    Data_Holder_Dialog(E_DIALOG_TYPE e_dialog_type, wxDialog_Base *pWindow, void *pExternData=NULL, bool bShow=true)
            : e_dialog_type(e_dialog_type)
            , pWindow(pWindow)
            , pExternData(pExternData)
            , bShow(bShow)
            , bRetCode(true)
            , nRetCode(0)
        {
            _COND(e_dialog_type != E_Dialog_Undefined);
        }
    E_DIALOG_TYPE e_dialog_type; // class type info
    wxDialog_Base *pWindow; // window on which the action should be performed
    void *pExternData; // extern data to be used by the window
    bool bShow;        // show/hide window
    bool bRetCode;     // return code
    int  nRetCode;     // return code
};

wxSemaError wx_semaphore_post(Data_Holder_Dialog &rData_Holder_Dialog);
wxSemaError wx_semaphore_wait(Data_Holder_Dialog &rData_Holder_Dialog);

// need public access without properties
extern wxCriticalSection g_oCriticalDialogAction; // protect critical dialog actions

const char * Get_ClassName(E_DIALOG_TYPE e_dialog_type);
const char * Get_WindowName(E_DIALOG_TYPE e_dialog_type);
const long Get_WindowID(E_DIALOG_TYPE e_dialog_type);
E_DIALOG_TYPE Get_Type(const char * sClassName);

template <class wxClassName>
E_DIALOG_TYPE Get_Type()
{
    wxClassInfo *pwxClassInfo = CLASSINFO(wxClassName);
    _COND_RET(pwxClassInfo, E_Dialog_Undefined);
    E_DIALOG_TYPE e_dialog_type = Get_Type(pwxClassInfo->GetClassName());
    return e_dialog_type;
}

template <class wxClassName>
wxClassName * ptr_wxDialogByType(E_DIALOG_TYPE e_dialog_type = E_Dialog_Undefined)
{
    if (e_dialog_type == E_Dialog_Undefined)
        e_dialog_type = Get_Type<wxClassName>();
    wxClassName *pwxWindow = NULL;
    pwxWindow = wxDynamicCast(
        wxWindow::FindWindowById(Get_WindowID(e_dialog_type)),
        wxClassName
        );
    if (pwxWindow)
        return pwxWindow;
    pwxWindow = wxDynamicCast(
        wxWindow::FindWindowByName(Get_WindowName(e_dialog_type)),
        wxClassName
        );
    return pwxWindow;
}

template <class wxClassName>
bool Safe_Close(wxClassName *pwxDialog)
{
    if (pwxDialog == NULL)
        pwxDialog = ptr_wxDialogByType<wxClassName>();
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    _COND_RET(pwxDialog != NULL, false);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        wxCriticalSectionLocker lock(g_oCriticalDialogAction);
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(Get_Type<wxClassName>(), pwxDialog);
        wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_Close, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog);
        _WX_LOG_NFO("Returned from main thread");
        return data_holder_dialog.bRetCode;
    }
#endif // USE_DEBUG_CODE
    pwxDialog->Destroy();
    return true;
}

template <class wxClassName>
wxClassName * Safe_Create(void *pExternData=NULL)
{
    _WX_LOG_NFO("pData=%p", pExternData);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        wxCriticalSectionLocker lock(g_oCriticalDialogAction);
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(Get_Type<wxClassName>(), NULL, pExternData);
        wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_Create, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog);
        _WX_LOG_NFO("Returned from main thread");
        wxClassName *pwxDialog = wx_static_cast(wxClassName *, data_holder_dialog.pWindow);
        _COND_RET(pwxDialog != NULL, NULL);
        return pwxDialog;
    }
#endif // USE_DEBUG_CODE
    wxClassName *pwxDialog = new wxClassName();
    _COND_RET(pwxDialog != NULL, NULL);
    _WX_LOG_NFO("Allocated at %p", pwxDialog);
    pwxDialog->Create(wxTheApp->GetTopWindow());
    Safe_Gui_DataLoad(pwxDialog, pExternData);
    return pwxDialog;
}

template <class wxClassName>
wxClassName * Safe_CreateUnique(void *pExternData=NULL)
{
    _WX_LOG_NFO("pData=%p", pExternData);
    wxClassName *pwxDialog = ptr_wxDialogByType<wxClassName>();
    if (pwxDialog)
    {
        _WX_LOG_WRN("Dialog is already active, closing, ptr=%p", pwxDialog);
        Safe_Close<wxClassName>(pwxDialog);
    }
    pwxDialog = Safe_Create<wxClassName>(pExternData);
    return pwxDialog;
}

template <class wxClassName>
bool Safe_Gui_DataLoad(wxClassName *pwxDialog, void *pExternData=NULL)
{
    if (pwxDialog == NULL)
        pwxDialog = ptr_wxDialogByType<wxClassName>();
    _WX_LOG_NFO("pWin=%p, pData=%p", pwxDialog, pExternData);
    _COND_RET(pwxDialog != NULL, false);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        wxCriticalSectionLocker lock(g_oCriticalDialogAction);
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(Get_Type<wxClassName>(), pwxDialog, pExternData);
        wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_DataLoad, "", &data_holder_dialog);
        _WX_LOG_NFO("Returned from main thread");
        wx_semaphore_wait(data_holder_dialog);
        return data_holder_dialog.bRetCode;
    }
#endif // USE_DEBUG_CODE
    bool retCode = pwxDialog->Gui_DataLoad(pExternData);
    if (! retCode)
    {
        _WX_LOG_WRN("Data Load");
    }
    return retCode;
}

template <class wxClassName>
bool Safe_Gui_DataSave(wxClassName *pwxDialog, void *pExternData=NULL)
{
    if (pwxDialog == NULL)
        pwxDialog = ptr_wxDialogByType<wxClassName>();
    _WX_LOG_NFO("pWin=%p, pData=%p", pwxDialog, pExternData);
    _COND_RET(pwxDialog != NULL, false);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        wxCriticalSectionLocker lock(g_oCriticalDialogAction);
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(Get_Type<wxClassName>(), pwxDialog, pExternData);
        wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_DataSave, "", &data_holder_dialog);
        _WX_LOG_NFO("Returned from main thread");
        wx_semaphore_wait(data_holder_dialog);
        return data_holder_dialog.bRetCode;
    }
#endif // USE_DEBUG_CODE
    bool retCode = pwxDialog->Gui_DataSave(pExternData);
    if (! retCode)
    {
        _WX_LOG_WRN("Data Save");
    }
    return retCode;
}

template <class wxClassName>
bool Safe_Gui_Refresh(wxClassName *pwxDialog, void *pExternData=NULL)
{
    if (pwxDialog == NULL)
        pwxDialog = ptr_wxDialogByType<wxClassName>();
    _WX_LOG_NFO("pWin=%p, pData=%p", pwxDialog, pExternData);
    _COND_RET(pwxDialog != NULL, false);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        wxCriticalSectionLocker lock(g_oCriticalDialogAction);
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(Get_Type<wxClassName>(), pwxDialog, pExternData);
        wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_Refresh, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog);
        _WX_LOG_NFO("Returned from main thread");
        return data_holder_dialog.bRetCode;
    }
#endif // USE_DEBUG_CODE
    if (! pwxDialog->IsInitialized())
    {
        _WX_LOG_ERR("Dialog is not initialized");
    }
    bool retCode = pwxDialog->Gui_Refresh(pExternData);
    pwxDialog->Refresh(false);
    pwxDialog->Update();
    return retCode;
}

template <class wxClassName>
bool Safe_Show(wxClassName *pwxDialog, bool bShow=true)
{
    if (pwxDialog == NULL)
        pwxDialog = ptr_wxDialogByType<wxClassName>();
    _WX_LOG_NFO("pWin=%p, bShow=%d", pwxDialog, bShow);
    _COND_RET(pwxDialog != NULL, false);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        wxCriticalSectionLocker lock(g_oCriticalDialogAction);
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(Get_Type<wxClassName>(), pwxDialog, NULL, bShow);
        wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_Show, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog);
        _WX_LOG_NFO("Returned from main thread");
        return data_holder_dialog.bRetCode;
    }
#endif // USE_DEBUG_CODE
    bool retCode = pwxDialog->Show(bShow);
    _WX_LOG_NFO("retCode=%d, pWin=%p", retCode, pwxDialog);
    return retCode;
}

template <class wxClassName>
int Safe_ShowModal(wxClassName *pwxDialog)
{
    if (pwxDialog == NULL)
        pwxDialog = ptr_wxDialogByType<wxClassName>();
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    _COND_RET(pwxDialog != NULL, 0);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        // create a non-modal dialog
        // but wait for it until dismissed by the user
        wxCriticalSectionLocker lock(g_oCriticalDialogAction);
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(Get_Type<wxClassName>(), pwxDialog);
        pwxDialog->Set_Data_Holder_Dialog(&data_holder_dialog);
        wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_Show, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog);
        _WX_LOG_NFO("Returned from main thread");
        _WX_LOG_NFO("Waiting the user");
        wx_semaphore_wait(data_holder_dialog);
        _WX_LOG_NFO("Acknowledge from the user");
        return data_holder_dialog.nRetCode;
    }
#endif // USE_DEBUG_CODE
    int retCode = pwxDialog->ShowModal();
    _WX_LOG_NFO("retCode=%d, pWin=%p", retCode, pwxDialog);
    return retCode;
}

template <class wxClassName>
bool Safe_WaitUser(wxClassName *pwxDialog, void *pExternData)
{
    if (pwxDialog == NULL)
        pwxDialog = ptr_wxDialogByType<wxClassName>();
    _WX_LOG_NFO("pWin=%p, pData=%p", pwxDialog, pExternData);
    _COND_RET(pwxDialog != NULL, false);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        wxCriticalSectionLocker lock(g_oCriticalDialogAction);
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(Get_Type<wxClassName>(), pwxDialog, pExternData);
        pwxDialog->Set_Data_Holder_Dialog(&data_holder_dialog);
        wx_post_event(wxTheApp, wxEVTC_DIALOG, E_Action_WaitUser, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog);
        _WX_LOG_NFO("Returned from main thread");
        return data_holder_dialog.bRetCode;
    }
#endif // USE_DEBUG_CODE
    pwxDialog->Set_WaitUser(wx_static_cast(Extern_Event_Data *, pExternData));
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    return true;
}

bool Process_Dialog_Action(E_DIALOG_TYPE e_dialog_type, E_ACTION_TYPE action, Data_Holder_Dialog *pData_Holder_Dialog);

#endif
// _WX_SAFE_DIALOG_H_
