//
// Author : C Remus
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

wxThread_Bag *g_pwxThread_Bag = NULL;

BEGIN_EVENT_TABLE( wxThread_Bag, wxEvtHandler )
EVTC_THREAD(wxID_ANY, wxThread_Bag::OnEvent_Thread)
END_EVENT_TABLE()
;

wxThread_Bag::wxThread_Bag()
{
    _WX_LOG_NFO();
    g_pwxThread_Bag = this;
    wxLog::AddTraceMask("thread");
}

wxThread_Bag::~wxThread_Bag()
{
    _WX_LOG_NFO();
    DestroyAll();
    g_pwxThread_Bag = NULL;
}

void wxThread_Bag::Add(wxThread_Cmd *pwxThread_Cmd)
{
    _WX_LOG_NFO();
    {
        wxCriticalSectionLocker lock(v_oCriticalSection);
        v_apwxThread_Cmd.Add(pwxThread_Cmd);
    }
}

void wxThread_Bag::Delete(wxThread_Cmd *pwxThread_Cmd)
{
    _WX_LOG_NFO();
    _COND_RET(pwxThread_Cmd);
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
            _WX_LOG_WRN("Not Found : %s", _str_thread_status(pwxThread_Cmd));
        }
        else
        {
            v_apwxThread_Cmd.RemoveAt(idx);
        }
    }
}

void wxThread_Bag::DestroyAll()
{
    _WX_LOG_NFO();
    // check if already empty
    {
        if ( v_apwxThread_Cmd.IsEmpty() )
            return;
    }
    // send stop to each thread, in normal order
    size_t nCount = v_apwxThread_Cmd.GetCount();
    _WX_LOG_NFO("Send stop to all : %d", nCount);
    {
        for (size_t i=0; i<nCount; i++)
        {
            wxThread_Cmd *pwxThread_Cmd = v_apwxThread_Cmd[i];
            pwxThread_Cmd->v_bShouldCancel = true;
        }
    }
    _WX_LOG_NFO("Waiting for all to stop : max %d ms", WAIT_THREAD_TIMEOUT_MSEC);
    bool b_wait_ok = (v_oSemaphoreRunningAll.WaitTimeout(WAIT_THREAD_TIMEOUT_MSEC) == wxSEMA_NO_ERROR);
    _WX_LOG_NFO("Check again, timer expired");
    if (b_wait_ok)
        return;
    _WX_LOG_WRN("Deleting threads that are still running : %d", v_apwxThread_Cmd.GetCount());
    // delete each thread, in reverse order
    {
        v_oCriticalSection.Enter();
        while ( ! v_apwxThread_Cmd.IsEmpty() )
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
    return ( v_apwxThread_Cmd.GetCount() );
}

size_t wxThread_Bag::GetRunningCount()
{
    size_t nRunningCount = 0;
    size_t count = v_apwxThread_Cmd.GetCount();
    for (size_t i=0; i<count; i++)
    {
        if (v_apwxThread_Cmd[i]->IsRunning())
            nRunningCount++;
    }
    return nRunningCount;
}

void wxThread_Bag::OnEvent_Thread(wxCommandEvent& event)
{
    _WX_LOG_NFO("Received event : %s", _str_event_thread(event));
    wxThread_Cmd *pwxThread_Cmd = wx_static_cast(wxThread_Cmd *, event.GetClientData());
    event.Skip();
    _COND_RET(pwxThread_Cmd != NULL);
    event.Skip(false);
    int idx = v_apwxThread_Cmd.Index(pwxThread_Cmd);
    if (idx == wxNOT_FOUND)
        return;
    int id = event.GetId();
    switch (id)
    {
        case wxThread_Cmd::E_RunEnded:
            _WX_LOG_NFO("deleting : obj(str='%s', ptr=%p)", event.GetString().c_str(), pwxThread_Cmd);
            Delete(pwxThread_Cmd);
            ::wxWakeUpIdle();
            if (event.GetString() == "ExternApp")
                wxTheApp->GetTopWindow()->Destroy();
            return;
            break;
    }
}
