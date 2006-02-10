#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wxthreaditem.h"
#endif
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxthreaditem.h"
#include "wxappmain.h"
#include "wxdialog_roomwizard.h"
#include "wxthreadbag.h"
#include "wxthreadevent.h"

BEGIN_EVENT_TABLE( wxThreadItem, wxEvtHandler )
  EVT_THREAD_COMMAND(wxID_ANY, wxThreadItem::OnThreadCommandEvent)
  END_EVENT_TABLE()
  ;

bool wxIdleThreadShouldStop()
{
  //_wx_log_nfo("wxIdleThreadShouldStop()");
  if (::wxIsMainThread())
  {
    //_wx_log_nfo("wxIdleThreadShouldStop() : wxIsMainThread()");
    return false;
  }
  wxThreadItem *pThis = ptrThreadItem();
  _wx_log_nfo("wxIdleThreadShouldStop(%p)", pThis);
  if (pThis == NULL)
  {
    _wx_log_nfo("wxIdleThreadShouldStop() : (pThis == NULL)");
    return false;
  }
  if (pThis->ShouldCancel())
  {
    _wx_log_nfo("wxIdleThreadShouldStop() : cancel %p", pThis);
    return true;
  }
  if (pThis->TestDestroy())
  {
    _wx_log_nfo("wxIdleThreadShouldStop() : destroy %p", pThis);
    return true;
  }
  //_wx_log_nfo("wxIdleThreadShouldStop();;");
  return false;
}

wxThreadItem * ptrThreadItem()
{
  wxThread *pThread = wxThread::This();
  wxThreadItem *pThreadItem = (wxThreadItem *) pThread;
  //_wx_log_nfo("ptrThreadItem() : %p => %p", pThread, pThreadItem);
  return pThreadItem;
}

wxThreadItem::wxThreadItem(wxThreadInfo *pThreadInfo)
    : wxThread(pThreadInfo->GetKind()), wxEvtHandler(), v_pInfo(pThreadInfo)
{
  _wx_log_nfo("wxThreadItem::wxThreadItem(*)");
  wxELSE_RET(v_pInfo);
  wxASSERT(v_pInfo->v_pItem == NULL);
  v_pInfo->v_pItem = this;
  wxASSERT(v_pInfo->v_eStatus == E_Unitialized);
  if (Create() == wxTHREAD_NO_ERROR)
    v_pInfo->v_eStatus = E_Created;
}

void * wxThreadItem::Entry()
{
  _wx_log_nfo("wxThreadItem::Entry() : Thread [%p] 0x%lx(0x%lx) started (priority = %u).", this, GetId(), GetCurrentId(), GetPriority());
  {
    wxCriticalSectionLocker lock(v_pInfo->v_rCriticalSection);
    wxELSE_RET_NULL(v_pInfo);
    wxELSE_RET_NULL(v_pInfo->v_eStatus == E_Created);
    v_pInfo->v_eStatus = E_RunStarted;
    (v_pInfo->v_rnRunningCount)++;
  }
  wxPostThreadEvent(g_pwxThreadBag, Evt_RunStarted, v_pInfo->v_sName, this);
  _wx_log_nfo("wxThreadItem::Entry() : Thread [%p] 0x%lx(0x%lx) Before Launch", this, GetId(), GetCurrentId());
  (v_pInfo->v_launch_fn)();
  _wx_log_nfo("wxThreadItem::Entry() : Thread [%p] 0x%lx(0x%lx) After Launch", this, GetId(), GetCurrentId());
  {
    wxCriticalSectionLocker lock(v_pInfo->v_rCriticalSection);
    _wx_log_nfo("Evt_RunExiting : '%s'", v_pInfo->_str_thread_status());
    wxELSE_RET_NULL(v_pInfo->v_eStatus == E_RunStarted);
    v_pInfo->v_eStatus = E_RunExiting;
  }
  wxPostThreadEvent(g_pwxThreadBag, Evt_RunExiting, v_pInfo->v_sName, this);
  _wx_log_nfo("wxThreadItem::Entry() : Thread [%p] 0x%lx(0x%lx) Exiting", this, GetId(), GetCurrentId());
  return NULL;
}

void wxThreadItem::OnExit()
{
  _wx_log_nfo("wxThreadItem::OnExit() : [%p]", this);
  do
  {
    wxCriticalSectionLocker lock(v_pInfo->v_rCriticalSection);
    if (v_pInfo == NULL)
      break;
    _wx_log_nfo("wxThreadItem::OnExit() : '%s'", v_pInfo->_str_thread_status());
    wxASSERT(v_pInfo->v_eStatus == E_RunExiting);
    v_pInfo->v_eStatus = E_RunEnded;
    (v_pInfo->v_rnRunningCount)--;
    v_pInfo->v_rSemaphoreRunningAll.Post();
    v_pInfo->v_oSemaphoreRunning.Post();
  } while (0);
  wxPostThreadEvent(g_pwxThreadBag, Evt_RunEnded, v_pInfo->v_sName, this);
  wxThread::OnExit();
  _wx_log_nfo("wxThreadItem::OnExit();;");
}

void wxThreadItem::Exit(ExitCode exitcode/*=0*/)
{
  _wx_log_nfo("wxThreadItem::Exit(*)");
  wxThread::Exit(exitcode);
}

void wxThreadItem::SetExitCode(int nExitCode)
{
  wxELSE_RET(v_pInfo);
  wxCriticalSectionLocker lock(v_pInfo->v_rCriticalSection);
  v_pInfo->v_nExitCode = nExitCode;
}

bool wxThreadItem::ShouldCancel()
{
  _wx_log_nfo("wxThreadItem::ShouldCancel() : '%s'", v_pInfo->_str_thread_status());
  wxCriticalSectionLocker lock(v_pInfo->v_rCriticalSection);
  return v_pInfo->v_bShouldCancel;
}

void wxThreadItem::OnThreadCommandEvent(wxCommandEvent& event)
{
  _wx_log_nfo("wxThreadItem::OnThreadCommandEvent(*) : %s", _str_thread_event(event));
  int id = event.GetId();
  switch (id)
  {
    case Evt_StopPlease:
      ::wxWakeUpIdle();
      this->Exit();
      return;
  }
}

wxThreadIdType wxThreadItem::GetIdWx()
{
  return GetId();
}

wxThreadIdType wxThreadItem::GetIdOs()
{
  return GetCurrentId();
}
