//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_thread_cmd.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_thread_cmd.h"
#include "wxappmain.h"
#include "wx_thread_wrapper.h"
#include "wx_thread_bag.h"

wxThread_Cmd::wxThread_Cmd(wxThreadKind eKind, const char *sName, type_ptr_void_fn_void p_fn_launch/*=NULL*/)
        : v_eKind(eKind)
        , v_sName(sName)
        , v_p_fn_launch(p_fn_launch)
        , v_eStatus(E_Unitialized)
        , v_bShouldCancel(false)
        , v_nExitCode(0)
        , v_pwxThread_Wrapper(NULL)
        , v_oSemaphoreRunning()
{
    _LOG_NFO();
}

wxThread_Cmd::~wxThread_Cmd()
{
    _LOG_NFO();
    Stop();
}

bool wxThread_Cmd::Start()
{
    if (wxGetApp().ptr_ThreadBag() == NULL)
    {
        _LOG_ERR("wxThread_Bag object not created");
        return false;
    }
    {
        wxCriticalSectionLocker lock(wxGetApp().ptr_ThreadBag()->v_oCriticalSection);
        if ( v_pwxThread_Wrapper != NULL )
        {
            _LOG_ERR("Can start only once");
            return false;
        }
        _LOG_NFO("New thread object : %s", _str_thread_status(this));
        v_pwxThread_Wrapper = new wxThread_Wrapper(
            v_eKind,
            this,
            wxGetApp().ptr_ThreadBag()->v_oCriticalSection,
            wxGetApp().ptr_ThreadBag()->v_nRunningCount,
            wxGetApp().ptr_ThreadBag()->v_oSemaphoreRunningAll,
            v_oSemaphoreRunning,
            v_eStatus,
            v_p_fn_launch
            );
    }
    _LOG_NFO("Creating : %s", _str_thread_status(this));
    v_pwxThread_Wrapper->Create();
    if ( v_eStatus != E_Created )
    {
        _LOG_ERR("Cannot create thread : %s", _str_thread_status(this));
        return false;
    }
    wxGetApp().ptr_ThreadBag()->Add(this);
    _LOG_NFO("Starting : %s", _str_thread_status(this));
    if ( v_pwxThread_Wrapper->Run() != wxTHREAD_NO_ERROR )
    {
        _LOG_ERR("Cannot start thread!");
        wxGetApp().ptr_ThreadBag()->Delete(this);
        return false;
    }
    _LOG_NFO("ok");
    return true;
}

bool wxThread_Cmd::Stop()
{
    _LOG_NFO("Stopping : %s", _str_thread_status(this));
    if (! IsRunning())
        return false;
    v_bShouldCancel = true;
    _LOG_NFO("Waiting to stop : max %d ms", WAIT_THREAD_TIMEOUT_MSEC);
    bool b_wait_ok = (v_oSemaphoreRunning.WaitTimeout(WAIT_THREAD_TIMEOUT_MSEC) == wxSEMA_NO_ERROR);
    if (! b_wait_ok)
    {
        _LOG_NFO("force stop");
        wxCriticalSectionLocker lock(wxGetApp().ptr_ThreadBag()->v_oCriticalSection);
        v_pwxThread_Wrapper->Kill();
        wxGetApp().ptr_ThreadBag()->v_nRunningCount--;
        wxGetApp().ptr_ThreadBag()->v_oSemaphoreRunningAll.Post();
    }
    return true;
}

void wxThread_Cmd::Run()
{
    if (v_p_fn_launch == NULL)
    {
        _LOG_ERR("Nothing to do in '%s'", GetName().c_str());
        return;
    }
    _LOG_NFO("[%p] Before Launch", this);
    (v_p_fn_launch)();
    _LOG_NFO("[%p] After Launch", this);
}

wxThreadKind wxThread_Cmd::GetKind()
{
    return v_eKind;
}

wxString wxThread_Cmd::GetName()
{
    return v_sName.c_str();
}

wxThread_Cmd::E_STATUS wxThread_Cmd::GetStatus()
{
    return v_eStatus;
}

int wxThread_Cmd::GetExitCode()
{
    wxCriticalSectionLocker lock(wxGetApp().ptr_ThreadBag()->v_oCriticalSection);
    _LOG_NFO("ExitCode == %d", v_nExitCode);
    return v_nExitCode;
}

void wxThread_Cmd::SetExitCode(int nExitCode)
{
    wxCriticalSectionLocker lock(wxGetApp().ptr_ThreadBag()->v_oCriticalSection);
    _LOG_NFO("ExitCode: %d -> %d", v_nExitCode, nExitCode);
    v_nExitCode = nExitCode;
}

bool wxThread_Cmd::IsDetached()
{
    return (v_eKind == wxTHREAD_DETACHED);
}

bool wxThread_Cmd::IsCancelled()
{
    //_LOG_NFO("%s", _str_thread_status(this));
    return v_bShouldCancel;
}

bool wxThread_Cmd::IsRunning()
{
    //_LOG_NFO();
    return (v_eStatus == E_RunStarted);
}

bool wxThread_Cmd::IsFinished()
{
    _LOG_NFO();
    return (v_eStatus == E_RunEnded);
}

bool wxThread_Cmd::TestDestroy()
{
    _COND_RET(v_pwxThread_Wrapper != NULL, true);
    return v_pwxThread_Wrapper->TestDestroy();
}

const char * _str_thread_status(wxThread_Cmd *pwxThread_Cmd)
{
    return wxString::Format(
        "_str_thread_status(kind='%s', name='%s', status='%s', cancelled=%c, exit_code=%d, pCmd=%p, pWrapper=%p)",
        _str_enum(pwxThread_Cmd->v_eKind),
        pwxThread_Cmd->v_sName.c_str(),
        _str_enum(pwxThread_Cmd->v_eStatus),
        ( pwxThread_Cmd->v_bShouldCancel ? 'Y' : 'N' ),
        pwxThread_Cmd->v_nExitCode,
        pwxThread_Cmd,
        pwxThread_Cmd->v_pwxThread_Wrapper
        );
}

const char * _str_enum(wxThread_Cmd::E_STATUS value)
{
    switch (value)
    {
        CASE_const_ret_str(wxThread_Cmd::E_Unitialized);
        CASE_const_ret_str(wxThread_Cmd::E_Created);
        CASE_const_ret_str(wxThread_Cmd::E_CreateError);
        CASE_const_ret_str(wxThread_Cmd::E_RunStarted);
        CASE_const_ret_str(wxThread_Cmd::E_RunCancelled);
        CASE_const_ret_str(wxThread_Cmd::E_RunExiting);
        CASE_const_ret_str(wxThread_Cmd::E_RunEnded);
        CASE_const_ret_str(wxThread_Cmd::E_RunDestroyed);
        CASE_const_ret_str(wxThread_Cmd::E_EventIdle);
        default:
            _LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}

const char * _str_enum(wxThreadKind value)
{
    switch (value)
    {
        CASE_const_ret_str(wxTHREAD_DETACHED);
        CASE_const_ret_str(wxTHREAD_JOINABLE);
        default:
            _LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}
