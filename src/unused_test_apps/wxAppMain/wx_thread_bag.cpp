//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_thread_bag.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_thread_bag.h"
#include "wx_event_thread.h"
#include "wx_thread_wrapper.h"
#include "wxframemain.h"

BEGIN_EVENT_TABLE( wxThread_Bag, wxEvtHandler )
    EVTC_THREAD(wxID_ANY, wxThread_Bag::OnEvent_Thread)
    END_EVENT_TABLE();

wxThread_Bag::wxThread_Bag()
{
    _LOG_NFO();
}

wxThread_Bag::~wxThread_Bag()
{
    _LOG_NFO();
    DestroyAll();
}

void wxThread_Bag::Add(wxThread_Cmd *pwxThread_Cmd)
{
    _LOG_NFO();
    {
        wxCriticalSectionLocker lock(v_oCriticalSection);
        v_apwxThread_Cmd.Add(pwxThread_Cmd);
    }
}

void wxThread_Bag::Delete(wxThread_Cmd *pwxThread_Cmd)
{
    _LOG_NFO();
    _COND_RET(pwxThread_Cmd != NULL);
    if (pwxThread_Cmd->IsRunning())
    {
        pwxThread_Cmd->Stop();
    }
    {
        wxCriticalSectionLocker lock(v_oCriticalSection);
        wxThread_Wrapper *pwxThread_Wrapper = pwxThread_Cmd->v_pwxThread_Wrapper;
        if (! pwxThread_Cmd->IsDetached())
            delete pwxThread_Wrapper;
        delete pwxThread_Cmd;
        int idx = v_apwxThread_Cmd.Index(pwxThread_Cmd);
        if (idx == wxNOT_FOUND)
        {
            _LOG_ERR("Not Found : %s", _str_thread_status(pwxThread_Cmd));
        }
        else
        {
            v_apwxThread_Cmd.RemoveAt(idx);
        }
    }
}

void wxThread_Bag::DestroyAll()
{
    _LOG_NFO();
    {
        wxCriticalSectionLocker lock(v_oCriticalSection);
        // check if already empty
        if ( v_apwxThread_Cmd.IsEmpty() )
            return;
        // send stop to each thread, in normal order
        size_t nCount = v_apwxThread_Cmd.GetCount();
        _LOG_NFO("Send stop to all : %d", nCount);
        {
            for (size_t i=0; i<nCount; i++)
            {
                wxThread_Cmd *pwxThread_Cmd = v_apwxThread_Cmd[i];
                pwxThread_Cmd->v_bShouldCancel = true;
            }
        }
    }
    _LOG_NFO("Waiting for all to stop : max %d ms", WAIT_ALL_THREADS_TIMEOUT_MSEC);
    bool b_wait_ok = (v_oSemaphoreRunningAll.WaitTimeout(WAIT_ALL_THREADS_TIMEOUT_MSEC) == wxSEMA_NO_ERROR);
    _LOG_NFO("Check again, timer expired");
    if (b_wait_ok)
        return;
    _LOG_WRN("Deleting threads that are still running : %d", v_apwxThread_Cmd.GetCount());
    // remove each thread, in reverse order
    {
        v_oCriticalSection.Enter();
        while (! v_apwxThread_Cmd.IsEmpty())
        {
            wxThread_Cmd *pwxThread_Cmd = v_apwxThread_Cmd.Last();
            v_oCriticalSection.Leave();
            Delete(pwxThread_Cmd);
            v_oCriticalSection.Enter();
        }
        v_oCriticalSection.Leave();
    }
}

size_t wxThread_Bag::GetCount()
{
    wxCriticalSectionLocker lock(v_oCriticalSection);
    return ( v_apwxThread_Cmd.GetCount() );
}

size_t wxThread_Bag::GetRunningCount()
{
    wxCriticalSectionLocker lock(v_oCriticalSection);
    size_t nRunningCount = 0;
    size_t count = v_apwxThread_Cmd.GetCount();
    for (size_t i=0; i<count; i++)
    {
        if (v_apwxThread_Cmd[i]->IsRunning())
            nRunningCount++;
    }
    return nRunningCount;
}

wxThread_Cmd * wxThread_Bag::ptr_ThreadItem(const wxString &sName)
{
    wxCriticalSectionLocker lock(v_oCriticalSection);
    size_t count = v_apwxThread_Cmd.GetCount();
    for (size_t i=0; i<count; i++)
    {
        if (sName == v_apwxThread_Cmd[i]->GetName())
            return v_apwxThread_Cmd[i];
    }
    return NULL;
}

void wxThread_Bag::OnEvent_Thread(wxCommandEvent& event)
{
    _LOG_NFO("Received event : %s", _str_event(event));
    wxThread_Cmd *pwxThread_Cmd = wx_static_cast(wxThread_Cmd *, event.GetClientData());
    _COND_RET(pwxThread_Cmd != NULL);
    int idx = v_apwxThread_Cmd.Index(pwxThread_Cmd);
    if (idx == wxNOT_FOUND)
        return;
    int id = event.GetId();
    switch (id)
    {
        case wxThread_Cmd::E_RunEnded:
            _LOG_NFO("deleting : obj(str='%s', ptr=%p)", event.GetString().c_str(), pwxThread_Cmd);
            wx_sleep(0, WAIT_THREAD_EXIT_MSEC);
            if (event.GetString() == "ExternApp")
            {
                _LOG_NFO("Send Exit Signals");
                App_SetExitCode(pwxThread_Cmd->GetExitCode());
                App_SetShouldExit(true);
            }
            Delete(pwxThread_Cmd);
            return;
            break;
    }
}
