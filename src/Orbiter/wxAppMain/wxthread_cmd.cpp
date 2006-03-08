//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxthread_cmd.h"
#endif
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxthread_cmd.h"
#include "wxthread_wrapper.h"
#include "wxthread_bag.h"

wxThread_Cmd::wxThread_Cmd(wxThreadKind eKind, const char *sName, type_ptr_void_fn_void p_fn_launch/*=NULL*/)
    : v_eKind(eKind)
, v_sName(sName)
, v_p_fn_launch(p_fn_launch)
, v_pwxThread_Bag(NULL)
, v_eStatus(E_Unitialized)
, v_bShouldCancel(false)
, v_nExitCode(0)
, v_pwxThread_Wrapper(NULL)
, v_oSemaphoreRunning()
{
    _WX_LOG_NFO();
}

wxThread_Cmd::~wxThread_Cmd()
{
    _WX_LOG_NFO();
}

bool wxThread_Cmd::Start()
{
    v_pwxThread_Bag = g_pwxThread_Bag;
    if (v_pwxThread_Bag == NULL)
    {
        _WX_LOG_ERR("wxThread_Bag object not created");
        return false;
    }
    {
        wxCriticalSectionLocker lock(v_pwxThread_Bag->v_oCriticalSection);
        if ( v_pwxThread_Wrapper != NULL )
        {
            _WX_LOG_ERR("Can start only once");
            return false;
        }
        _WX_LOG_NFO("New thread object : %s", _str_thread_status(this));
        v_pwxThread_Wrapper = new wxThread_Wrapper(
            v_eKind,
            this,
            v_pwxThread_Bag->v_oCriticalSection,
            v_pwxThread_Bag->v_nRunningCount,
            v_pwxThread_Bag->v_oSemaphoreRunningAll,
            v_oSemaphoreRunning,
            v_eStatus,
            v_p_fn_launch
            );
    }
    _WX_LOG_NFO("Creating : %s", _str_thread_status(this));
    v_pwxThread_Wrapper->Create();
    if ( v_eStatus != E_Created )
    {
        _WX_LOG_ERR("Can't create thread! : %s", _str_thread_status(this));
        return false;
    }
    v_pwxThread_Bag->Add(this);
    _WX_LOG_NFO("Starting : %s", _str_thread_status(this));
    if ( v_pwxThread_Wrapper->Run() != wxTHREAD_NO_ERROR )
    {
        _WX_LOG_ERR("Can't start thread!");
        v_pwxThread_Bag->Delete(this);
        return false;
    }
    _WX_LOG_NFO("ok");
    return true;
}

bool wxThread_Cmd::Stop()
{
    _WX_LOG_NFO("Stopping : %s", _str_thread_status(this));
    _COND_RET(IsRunning(), false);
    v_bShouldCancel = true;
    bool b_wait_ok = (v_oSemaphoreRunning.WaitTimeout(WAIT_THREAD_TIMEOUT_MSEC) == wxSEMA_NO_ERROR);
    if (! b_wait_ok)
    {
        _WX_LOG_NFO("force stop");
        wxCriticalSectionLocker lock(v_pwxThread_Bag->v_oCriticalSection);
        v_pwxThread_Wrapper->Kill();
        v_pwxThread_Bag->v_nRunningCount--;
        v_pwxThread_Bag->v_oSemaphoreRunningAll.Post();
    }
    return true;
}

void wxThread_Cmd::Run()
{
    if (v_p_fn_launch == NULL)
    {
        _WX_LOG_ERR("Nothing to do in '%s'", GetName().c_str());
        return;
    }
    _WX_LOG_NFO("[%p] Before Launch", this);
    (v_p_fn_launch)();
    _WX_LOG_NFO("[%p] After Launch", this);
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
    wxCriticalSectionLocker lock(v_pwxThread_Bag->v_oCriticalSection);
    return v_nExitCode;
}

void wxThread_Cmd::SetExitCode(int nExitCode)
{
    _WX_LOG_NFO();
    wxCriticalSectionLocker lock(v_pwxThread_Bag->v_oCriticalSection);
    v_nExitCode = nExitCode;
}

bool wxThread_Cmd::IsDetached()
{
    return (v_eKind == wxTHREAD_DETACHED);
}

bool wxThread_Cmd::IsCancelled()
{
    _WX_LOG_NFO("%s", _str_thread_status(this));
    return v_bShouldCancel;
}

bool wxThread_Cmd::IsRunning()
{
    //_WX_LOG_NFO();
    return (v_eStatus == E_RunStarted);
}

bool wxThread_Cmd::IsFinished()
{
    _WX_LOG_NFO();
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
        "_str_thread_status(kind=%s, name='%s', status=%s, cancelled=%c, exit_code=%d, pCmd=%p, pWrapper=%p)",
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
            _WX_LOG_ERR("unknown value %d", value);
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
            _WX_LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}
