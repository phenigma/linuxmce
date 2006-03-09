//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_WIN_THREAD_H_
#define _WX_WIN_THREAD_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_win_thread.h"
#endif

#include "wxevent_dialog.h"

// instead of RTTI info
enum E_wxDialog_Class_Type
{
    E_wxDialog_Undefined = 0,
    E_wxDialog_RoomWizard,
    E_wxDialog_WaitGrid,
    E_wxDialog_WaitList,
    E_wxDialog_WaitUser,
};
const char * _str_enum(E_wxDialog_Class_Type value);

struct Data_Holder_Base : wxObject
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
            : Data_Holder_Base()
            , e_class_type(E_wxDialog_Undefined)
            , pWindow(NULL)
            , idWindow(0)
            , sLabel("")
            , pExternData(NULL)
            , bShow(true)
            , bRetCode(true)
            , nRetCode(0)
        {
        }
    Data_Holder_Dialog(E_wxDialog_Class_Type class_type, wxWindow *pWindow, bool bShow=true)
            : Data_Holder_Base()
            , e_class_type(class_type)
            , pWindow(pWindow)
            , idWindow(0)
            , sLabel("")
            , pExternData(NULL)
            , bShow(bShow)
            , bRetCode(true)
            , nRetCode(0)
        {
            _COND(e_class_type != E_wxDialog_Undefined);
            _COND(pWindow != NULL);
        }
    Data_Holder_Dialog(E_wxDialog_Class_Type class_type, int idWindow, const char *sLabel, void *pExternData=NULL)
            : Data_Holder_Base()
            , e_class_type(class_type)
            , pWindow(NULL)
            , idWindow(idWindow)
            , sLabel(sLabel)
            , pExternData(pExternData)
            , bShow(true)
            , bRetCode(true)
            , nRetCode(0)
        {
            _COND(e_class_type != E_wxDialog_Undefined);
            _COND(sLabel != NULL);
        }
    E_wxDialog_Class_Type e_class_type; // class type info
    wxWindow *pWindow; // window on which the action should be performed
    int idWindow;      // window id
    wxString sLabel;   // window label
    void *pExternData; // extern data to be used by the window
    bool bShow;        // show/hide window
    bool bRetCode;     // return code
    bool nRetCode;     // return code
};

template <class wxClassName>
wxClassName * ptr_wxWindowByName(const char *sLabel)
{
    wxClassName *pwxWindow = wxDynamicCast(wxWindow::FindWindowByName(sLabel), wxClassName);
    if (pwxWindow != NULL)
    {
        _WX_LOG_NFO("Window is allocated at %p", pwxWindow);
    }
    else
    {
        //_WX_LOG_WRN("Window is not active");
    }
    return pwxWindow;
}

template <class wxClassName>
void wxDialog_Close(wxClassName *pwxDialog)
{
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    _COND_RET(pwxDialog != NULL);
    if (! ::wxIsMainThread())
    {
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(wxClassName::e_class_type, pwxDialog);
        wx_post_event_thread(wxTheApp, wxEVTC_DIALOG, E_Dialog_Close, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
        _WX_LOG_NFO("Returned from main thread");
        return;
    }
    pwxDialog->Destroy();
}

template <class wxClassName>
wxClassName * wxDialog_Create(int idWindow, const char *sLabel, void *pExternData=NULL)
{
    _WX_LOG_NFO("id=%d, label='%s', pData=%p", idWindow, sLabel, pExternData);
    if (! ::wxIsMainThread())
    {
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(wxClassName::e_class_type, idWindow, sLabel, pExternData);
        wx_post_event_thread(wxTheApp, wxEVTC_DIALOG, E_Dialog_Create, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
        _WX_LOG_NFO("Returned from main thread");
        return wx_static_cast(wxClassName *, data_holder_dialog.pWindow);
    }
    wxClassName *pwxDialog = new wxClassName();
    _WX_LOG_NFO("Allocated at %p", pwxDialog);
    pwxDialog->ExternData_Load(pExternData);
    pwxDialog->Create(wxTheApp->GetTopWindow(), idWindow, sLabel);
    return pwxDialog;
}

template <class wxClassName>
wxClassName * wxDialog_CreateUnique(int idWindow, const char *sLabel, void *pExternData=NULL)
{
    _WX_LOG_NFO("id=%d, label='%s', pData=%p", idWindow, sLabel, pExternData);
    wxClassName *pwxDialog = ptr_wxWindowByName<wxClassName>(sLabel);
    if (pwxDialog != NULL)
    {
        _WX_LOG_WRN("Dialog is already active, closing, ptr=%p", pwxDialog);
        wxDialog_Close<wxClassName>(pwxDialog);
    }
    pwxDialog = wxDialog_Create<wxClassName>(idWindow, sLabel, pExternData);
    return pwxDialog;
}

template <class wxClassName>
bool wxDialog_Show(wxClassName *pwxDialog, bool bShow=true)
{
    _WX_LOG_NFO("pWin=%p, bShow=%d", pwxDialog, bShow);
    _COND_RET(pwxDialog != NULL, false);
    if (! ::wxIsMainThread())
    {
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(wxClassName::e_class_type, pwxDialog, bShow);
        wx_post_event_thread(wxTheApp, wxEVTC_DIALOG, E_Dialog_Show, "", &data_holder_dialog);
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
        _WX_LOG_NFO("Returned from main thread");
        return data_holder_dialog.bRetCode;
    }
    bool retCode = pwxDialog->Show(bShow);
    _WX_LOG_NFO("retCode=%d, pWin=%p", retCode, pwxDialog);
    return retCode;
}

template <class wxClassName>
int wxDialog_ShowModal(wxClassName *pwxDialog)
{
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    _COND_RET(pwxDialog != NULL, -1);
    if (! ::wxIsMainThread())
    {
        _WX_LOG_NFO("Switching to main thread");
        Data_Holder_Dialog data_holder_dialog(wxClassName::e_class_type, pwxDialog);
        // create a non-modal dialog, but wait for it until dismissed
        pwxDialog->Set_Data_Holder_Dialog(&data_holder_dialog);
        wx_post_event_thread(wxTheApp, wxEVTC_DIALOG, E_Dialog_Show, "", &data_holder_dialog);
        _WX_LOG_NFO("Waiting the dialog");
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
        _WX_LOG_NFO("Returned from main thread");
        wx_semaphore_wait(data_holder_dialog.oSemaphore);
        return data_holder_dialog.nRetCode;
    }
    int retCode = pwxDialog->ShowModal();
    _WX_LOG_NFO("retCode=%d, pWin=%p", retCode, pwxDialog);
    return retCode;
}

template <class wxClassName>
bool wxDialog_Save(wxClassName *pwxDialog, void *pExternData=NULL)
{
    _WX_LOG_NFO("pWin=%p", pwxDialog);
    _COND_RET(pwxDialog != NULL, false);
    return pwxDialog->ExternData_Save(pExternData);
}

#endif
// _WX_WIN_THREAD_H_
