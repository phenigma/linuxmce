//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_DIALOG_SAFE_H_
#define _WX_DIALOG_SAFE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_dialog_safe.h"
#endif

#include "wx_dialog_types.h"
#include "wx_event_dialog.h"

struct Data_Holder_Base
{
    Data_Holder_Base()
        : oSemaphore(0, 1)
        {
        }
    wxSemaphore oSemaphore; // wait until the action is finished
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
    Data_Holder_Dialog(E_DIALOG_TYPE e_dialog_type, wxWindow *pWindow, bool bShow=true)
            : e_dialog_type(e_dialog_type)
            , pWindow(pWindow)
            , pExternData(NULL)
            , bShow(bShow)
            , bRetCode(true)
            , nRetCode(0)
        {
            _COND(e_dialog_type != E_Dialog_Undefined);
            _COND(pWindow != NULL);
        }
    Data_Holder_Dialog(E_DIALOG_TYPE e_dialog_type, void *pExternData=NULL)
            : e_dialog_type(e_dialog_type)
            , pWindow(NULL)
            , pExternData(pExternData)
            , bShow(true)
            , bRetCode(true)
            , nRetCode(0)
        {
            _COND(e_dialog_type != E_Dialog_Undefined);
        }
    E_DIALOG_TYPE e_dialog_type; // class type info
    wxWindow *pWindow; // window on which the action should be performed
    void *pExternData; // extern data to be used by the window
    bool bShow;        // show/hide window
    bool bRetCode;     // return code
    bool nRetCode;     // return code
};

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
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    _COND_RET(pwxDialog != NULL, false);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(wxClassName::e_dialog_type, pwxDialog);
        wx_post_event_thread(wxTheApp, wxEVTC_DIALOG, E_Action_Close, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
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
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(wxClassName::e_dialog_type, pExternData);
        wx_post_event_thread(wxTheApp, wxEVTC_DIALOG, E_Action_Create, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
        _WX_LOG_NFO("Returned from main thread");
        return wx_static_cast(wxClassName *, data_holder_dialog.pWindow);
    }
#endif // USE_DEBUG_CODE
    wxClassName *pwxDialog = new wxClassName();
    _WX_LOG_NFO("Allocated at %p", pwxDialog);
    pwxDialog->ExternData_Load(pExternData);
    pwxDialog->Create(wxTheApp->GetTopWindow());
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
bool Safe_Refresh(wxClassName *pwxDialog, void *pExternData=NULL)
{
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    _COND_RET(pwxDialog, false);
    //--return pwxDialog->NewDataRefresh(pExternData);
    pwxDialog->Refresh();
    return true;
}

template <class wxClassName>
bool Safe_Save(wxClassName *pwxDialog, void *pExternData=NULL)
{
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    _COND_RET(pwxDialog, false);
    return pwxDialog->ExternData_Save(pExternData);
}

template <class wxClassName>
bool Safe_Show(wxClassName *pwxDialog, bool bShow=true)
{
    _WX_LOG_NFO("pWin=%p, bShow=%d", pwxDialog, bShow);
    _COND_RET(pwxDialog, false);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(wxClassName::e_dialog_type, pwxDialog, bShow);
        wx_post_event_thread(wxTheApp, wxEVTC_DIALOG, E_Action_Show, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
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
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    _COND_RET(pwxDialog, -1);
#ifdef USE_DEBUG_CODE
    if (! ::wxIsMainThread())
    {
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(wxClassName::e_dialog_type, pwxDialog);
        // create a non-modal dialog, but wait for it until dismissed
        pwxDialog->Set_Data_Holder_Dialog(&data_holder_dialog);
        wx_post_event_thread(wxTheApp, wxEVTC_DIALOG, E_Action_Show, "", &data_holder_dialog);
        _WX_LOG_NFO("Returned from main thread");
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
        _WX_LOG_NFO("Waiting the dialog");
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
        return data_holder_dialog.nRetCode;
    }
#endif // USE_DEBUG_CODE
    int retCode = pwxDialog->ShowModal();
    _WX_LOG_NFO("retCode=%d, pWin=%p", retCode, pwxDialog);
    return retCode;
}

#endif
// _WX_DIALOG_SAFE_H_
