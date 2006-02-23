#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxthreadbag.h"
#endif
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxthreadbag.h"
#include "wxthreadevent.h"
#include "wxappmain.h"
#include "wxdialog_roomwizard.h"

wxThreadBag *g_pwxThreadBag = NULL;

BEGIN_EVENT_TABLE( wxThreadBag, wxEvtHandler )
  EVT_THREAD_COMMAND(wxID_ANY, wxThreadBag::OnThreadCommandEvent)
  END_EVENT_TABLE()
  ;

wxThreadBag::wxThreadBag()
{
  _wx_log_nfo("wxThreadBag::wxThreadBag()");
  g_pwxThreadBag = this;
  wxLog::AddTraceMask("thread");
  is_ok_array();
  _wx_log_nfo("wxThreadBag::wxThreadBag();;");
}

wxThreadBag::~wxThreadBag()
{
  _wx_log_nfo("wxThreadBag::~wxThreadBag()");
  DestroyAll();
  is_ok_array();
  g_pwxThreadBag = NULL;
  _wx_log_nfo("wxThreadBag::~wxThreadBag();;");
}

wxThreadInfo * wxThreadBag::Start(type_fn_thread_launch launch_fn, wxThreadKind eKind, const char *sName)
{
  _wx_log_nfo("wxThreadBag::Start(*)");
  wxThreadInfo *pInfo = new wxThreadInfo(launch_fn, eKind, sName, v_oCriticalSection, v_nRunningCount, v_oSemaphoreRunningAll);
  wxThreadItem *pItem = new wxThreadItem(pInfo);
  {
    wxCriticalSectionLocker lock(v_oCriticalSection);
    v_apInfo.Add(pInfo);
    v_apItem.Add(pItem);
  }
  if ( pInfo->v_eStatus != E_Created )
  {
    _wx_log_err("Can't create thread!");
    Delete(pInfo);
    return NULL;
  }
  if ( pItem->Run() != wxTHREAD_NO_ERROR )
  {
    _wx_log_err("Can't start thread!");
    Delete(pInfo);
    return NULL;
  }
  _wx_log_nfo("Started : ok");
  is_ok_array();
  return pInfo;
}

void wxThreadBag::Delete(wxThreadInfo *pInfo)
{
  _wx_log_nfo("wxThreadBag::Delete()");
  wxELSE_RET(pInfo);
  if (pInfo->check_alive())
  {
    Stop(pInfo);
  }
  {
    wxCriticalSectionLocker lock(v_oCriticalSection);
    wxThreadItem *pItem = pInfo->v_pItem;
    int idx = v_apInfo.Index(pInfo);
    wxASSERT(idx != wxNOT_FOUND);
    if (! pInfo->is_detached())
      delete pItem;
    delete pInfo;
    v_apInfo.RemoveAt(idx);
    v_apItem.RemoveAt(idx);
  }
  is_ok_array();
  _wx_log_nfo("wxThreadBag::Delete();;");
}

void wxThreadBag::Stop(wxThreadInfo *pInfo)
{
  _wx_log_nfo("wxThreadBag::Stop()");
  wxELSE_RET(pInfo);
  wxELSE_RET(pInfo->check_alive());
  _wx_log_nfo("wxThreadBag::Stop() : {{ '%s'", pInfo->_str_thread_status());
  pInfo->v_bShouldCancel = true;
  _wx_log_nfo("wxThreadBag::Stop() : }} '%s'", pInfo->_str_thread_status());
  bool b_wait_ok = (pInfo->v_oSemaphoreRunning.WaitTimeout(WAIT_THREAD_TIMEOUT_MILLISECONDS) == wxSEMA_NO_ERROR);
  if (! b_wait_ok)
  {
    _wx_log_nfo("wxThreadBag::Stop(): force stop");
    wxCriticalSectionLocker lock(v_oCriticalSection);
    wxThreadItem *pItem = pInfo->v_pItem;
    // pItem->Delete(); not working for all
    pItem->Kill();
    v_nRunningCount--;
    v_oSemaphoreRunningAll.Post();
  }
  _wx_log_nfo("wxThreadBag::Stop();;");
}

void wxThreadBag::DestroyAll()
{
  _wx_log_nfo("wxThreadBag::DestroyAll()");
  // check if already empty
  {
    if ( v_apInfo.IsEmpty() )
      return;
  }
  size_t nCount = v_apInfo.GetCount();
  _wx_log_nfo("wxThreadBag::DestroyAll() : send stop to all : %d", nCount);
  {
    for (size_t i=0; i<nCount; i++)
    {
      wxThreadInfo *pInfo = v_apInfo[i];
      _wx_log_nfo("wxThreadBag::DestroyAll() : {{ '%s'", pInfo->_str_thread_status());
      pInfo->v_bShouldCancel = true;
      _wx_log_nfo("wxThreadBag::DestroyAll() : }} '%s'", pInfo->_str_thread_status());
    }
  }
  _wx_log_nfo("wxThreadBag::DestroyAll() : waiting all : %d", nCount);
  bool b_wait_ok = (v_oSemaphoreRunningAll.WaitTimeout(WAIT_THREAD_TIMEOUT_MILLISECONDS) == wxSEMA_NO_ERROR);
  if (! b_wait_ok)
  {
    _wx_log_wrn("wxThreadBag::DestroyAll() : not all threads were stopped");
  }
  // deleting each thread
  {
    v_oCriticalSection.Enter();
    while ( ! v_apInfo.IsEmpty() )
    {
      wxThreadInfo *pInfo = v_apInfo.Last();
      v_oCriticalSection.Leave();
      Delete(pInfo);
      v_oCriticalSection.Enter();
    }
    v_oCriticalSection.Leave();
  }
  is_ok_array();
  _wx_log_nfo("wxThreadBag::DestroyAll();;");
}

size_t wxThreadBag::GetCount()
{
  is_ok_array();
  return ( v_apInfo.GetCount() );
}

size_t wxThreadBag::GetRunningCount()
{
  is_ok_array();
  size_t nRunningCount = 0;
  size_t count = v_apInfo.GetCount();
  for (size_t i=0; i<count; i++)
  {
    if (v_apInfo[i]->check_alive())
      nRunningCount++;
  }
  return nRunningCount;
}

bool wxThreadBag::is_ok_array()
{
  size_t n = v_apInfo.GetCount();
  bool b_is_ok = (n == v_apInfo.GetCount());
  wxELSE_RET_false(b_is_ok);
  return b_is_ok;
}

void wxThreadBag::OnThreadCommandEvent(wxCommandEvent& event)
{
  _wx_log_nfo("wxThreadBag::OnThreadCommandEvent(*) : %s", _str_thread_event(event));
  {
    wxThreadItem *pItem = (wxThreadItem *)event.GetClientData();
    if (pItem == NULL)
      return;
    is_ok_array();
    int idx = v_apItem.Index(pItem);
    wxELSE_RET(idx != wxNOT_FOUND);
    wxThreadInfo *pInfo = v_apInfo[idx];
    int id = event.GetId();
    switch (id)
    {
      case Evt_RunEnded:
        Delete(pInfo);
        ::wxWakeUpIdle();
        return;
        break;
    }
  }
}

const char * str_enum(wxSemaError value)
{
  switch (value)
  {
    CASE_const_ret_str(wxSEMA_NO_ERROR);
    CASE_const_ret_str(wxSEMA_INVALID);
    CASE_const_ret_str(wxSEMA_BUSY);
    CASE_const_ret_str(wxSEMA_TIMEOUT);
    CASE_const_ret_str(wxSEMA_OVERFLOW);
    CASE_const_ret_str(wxSEMA_MISC_ERROR);
    default:
      _wx_log_err("str_enum(wxSemaError): unknown value %d", value);
      break;
  }
  return wxString::Format("?%d?", value);
}

void _wx_log_status_threads()
{
  if (! g_pwxThreadBag)
    return;
  g_pwxFrameMain->SetStatusText(
    wxString::Format(
      "All[%d] Running[%d]",
      g_pwxThreadBag->GetCount(),
      g_pwxThreadBag->GetRunningCount()
      )
    );
}
