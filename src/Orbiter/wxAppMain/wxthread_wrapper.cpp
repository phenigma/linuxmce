//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxthread_wrapper.h"
#endif
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxthread_wrapper.h"
#include "wxevent_thread.h"
#include "wxthread_bag.h"
#include "wxthread_cmd.h"

wxThread_Wrapper::wxThread_Wrapper(
    wxThreadKind eKind,
    wxThread_Cmd *pwxThread_Cmd,
    wxCriticalSection &rCriticalSection,
    int &rnRunningCount,
    wxSemaphore &rSemaphoreRunningAll,
    wxSemaphore &rSemaphoreRunning,
    wxThread_Cmd::E_STATUS &reStatus,
    type_ptr_void_fn_void p_fn_launch
    )
    : wxThread(eKind)
, v_pwxThread_Cmd(pwxThread_Cmd)
, v_rCriticalSection(rCriticalSection)
, v_rnRunningCount(rnRunningCount)
, v_rSemaphoreRunningAll(rSemaphoreRunningAll)
, v_rSemaphoreRunning(rSemaphoreRunning)
, v_reStatus(reStatus)
, v_p_fn_launch(p_fn_launch)
{
    _WX_LOG_NFO("pCmd=%p, pWrapper=%p", pwxThread_Cmd, this);
}

wxThreadError wxThread_Wrapper::Create()
{
    if (v_pwxThread_Cmd == NULL)
    {
        _WX_LOG_ERR("wxThread_Cmd object not created");
        _COND_ASSIGN(v_reStatus, wxThread_Cmd::E_Unitialized, wxThread_Cmd::E_CreateError);
        return wxTHREAD_MISC_ERROR;
    }
    wxThreadError retCode = wxThread::Create();
    if (retCode == wxTHREAD_NO_ERROR)
        _COND_ASSIGN(v_reStatus, wxThread_Cmd::E_Unitialized, wxThread_Cmd::E_Created);
    else
        _COND_ASSIGN(v_reStatus, wxThread_Cmd::E_Unitialized, wxThread_Cmd::E_CreateError);
    return retCode;
}

void * wxThread_Wrapper::Entry()
{
    _WX_LOG_NFO("[%p] Starting", v_pwxThread_Cmd);
    _COND_RET(v_pwxThread_Cmd, NULL);
    {
        wxCriticalSectionLocker lock(v_rCriticalSection);
        _COND_ASSIGN(v_reStatus, wxThread_Cmd::E_Created, wxThread_Cmd::E_RunStarted);
        v_rnRunningCount++;
    }
    wx_post_event_thread(g_pwxThread_Bag, wxEVTC_THREAD, wxThread_Cmd::E_RunStarted, v_pwxThread_Cmd->GetName(), v_pwxThread_Cmd);
    _WX_LOG_NFO("[%p] Before Run", v_pwxThread_Cmd);
    v_pwxThread_Cmd->Run();
    _WX_LOG_NFO("[%p] After Run", v_pwxThread_Cmd);
    {
        wxCriticalSectionLocker lock(v_rCriticalSection);
        _COND_ASSIGN(v_reStatus, wxThread_Cmd::E_RunStarted, wxThread_Cmd::E_RunExiting);
    }
    wx_post_event_thread(g_pwxThread_Bag, wxEVTC_THREAD, wxThread_Cmd::E_RunExiting, v_pwxThread_Cmd->GetName(), v_pwxThread_Cmd);
    _WX_LOG_NFO("[%p] Exiting", v_pwxThread_Cmd);
    return NULL;
}

void wxThread_Wrapper::OnExit()
{
    _WX_LOG_NFO("[%p] Ending", v_pwxThread_Cmd);
    if (v_pwxThread_Cmd != NULL)
    {
        wxCriticalSectionLocker lock(v_rCriticalSection);
        _COND_ASSIGN(v_reStatus, wxThread_Cmd::E_RunExiting, wxThread_Cmd::E_RunEnded);
        v_rnRunningCount--;
        v_rSemaphoreRunningAll.Post();
        v_rSemaphoreRunning.Post();
        wx_post_event_thread(g_pwxThread_Bag, wxEVTC_THREAD, wxThread_Cmd::E_RunEnded, v_pwxThread_Cmd->GetName(), v_pwxThread_Cmd);
    }
    wxThread::OnExit();
}

wxThread_Wrapper * ptr_wxThread_Wrapper()
{
    if (::wxIsMainThread())
        return NULL;
    wxThread *pwxThread = wxThread::This();
    wxThread_Wrapper *pwxThread_Wrapper = wx_static_cast(wxThread_Wrapper *, pwxThread);
    return pwxThread_Wrapper;
}

wxThread_Cmd * ptr_wxThread_Cmd()
{
    wxThread_Wrapper *pwxThread_Wrapper = ptr_wxThread_Wrapper();
    if (pwxThread_Wrapper == NULL)
        return NULL;
    return pwxThread_Wrapper->v_pwxThread_Cmd;
}
