#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxthreadinfo.h"
#endif
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxthreadinfo.h"
#include "wxthreaditem.h"
#include "wx_other.h"

wxThreadInfo::wxThreadInfo(
  type_fn_thread_launch launch_fn,
  wxThreadKind eKind,
  const char *sName,
  wxCriticalSection &rCriticalSection,
  int &rnRunningCount,
  wxSemaphore &rSemaphoreRunningAll
  )
    : v_eKind(eKind)
      , v_launch_fn(launch_fn)
      , v_sName(sName)
      , v_rCriticalSection(rCriticalSection)
      , v_rnRunningCount(rnRunningCount)
      , v_rSemaphoreRunningAll(rSemaphoreRunningAll)
      , v_eStatus(E_Unitialized)
      , v_bShouldCancel(false)
      , v_nExitCode(0)
      , v_pItem(NULL)
      , v_oSemaphoreRunning()
      , v_bCheckStatusChanged(false)
{
  _wx_log_nfo("wxThreadInfo::wxThreadInfo(*)");
}

wxThreadKind wxThreadInfo::GetKind()
{
  return v_eKind;
}

wxString wxThreadInfo::GetName()
{
  return v_sName;
}

E_THREAD_STATUS wxThreadInfo::GetStatus()
{
  _wx_log_nfo("wxThreadInfo::GetStatus()");
  check_status();
  return v_eStatus;
}

int wxThreadInfo::GetExitCode()
{
  return v_nExitCode;
}

wxThreadIdType wxThreadInfo::GetIdWx()
{
  //_wx_log_nfo("wxThreadInfo::GetIdWx()");
  wxELSE_RET_0(check_status());
  return v_pItem->GetIdWx();
}

wxThreadIdType wxThreadInfo::GetIdOs()
{
  //_wx_log_nfo("wxThreadInfo::GetIdOs()");
  wxELSE_RET_0(check_status());
  return v_pItem->GetIdOs();
}

bool wxThreadInfo::is_detached()
{
  return (v_eKind == wxTHREAD_DETACHED);
}

bool wxThreadInfo::is_finished()
{
  _wx_log_nfo("wxThreadInfo::is_finished()");
  if (! check_status())
    return false;
  return (v_eStatus == E_RunEnded);
}

bool wxThreadInfo::check_alive()
{
  //_wx_log_nfo("wxThreadInfo::check_alive()");
  if (! check_status())
    return false;
  return (v_eStatus == E_RunStarted);
}

bool wxThreadInfo::check_status()
{
  wxLogNull no_logging;
  bool b_is_ok = true;
  // check ptr to object
  if (v_pItem == NULL)
  {
    // should never be here
    _wx_log_err("wxThreadInfo::check_status() : v_pItem == NULL");
    b_is_ok = false;
  }
  if ( (! b_is_ok) && (v_eStatus = E_RunStarted) )
  {
    wxCriticalSectionLocker lock(v_rCriticalSection);
    _wx_log_err("wxThreadInfo::check_status() : changing status : %s", str_enum(v_eStatus));
    v_eStatus = E_RunDestroyed;
    if (! v_bCheckStatusChanged)
    {
      v_rnRunningCount--;
      v_rSemaphoreRunningAll.Post();
      v_oSemaphoreRunning.Post();
      v_bCheckStatusChanged = true;
    }
  }
  return b_is_ok;
}

const char * wxThreadInfo::_str_thread_status()
{
  return wxString::Format(
    "_str_thread_status(kind=%s, name='%s', status=%s, cancelled=%c, exit_code=%d, IdWx=%ld, IdOs=%ld, pInfo=%p, pItem=%p, ptrThreadItem=%p)",
    str_enum(v_eKind),
    v_sName.c_str(),
    str_enum(v_eStatus),
    ( v_bShouldCancel ? 'Y' : 'N' ),
    v_nExitCode,
    GetIdWx(),
    GetIdOs(),
    this,
    v_pItem,
    ptrThreadItem()
    );
}

const char * str_enum(E_THREAD_STATUS value)
{
  switch (value)
  {
    CASE_const_ret_str(E_Unitialized);
    CASE_const_ret_str(E_Created);
    CASE_const_ret_str(E_RunStarted);
    CASE_const_ret_str(E_RunCancelled);
    CASE_const_ret_str(E_RunExiting);
    CASE_const_ret_str(E_RunEnded);
    CASE_const_ret_str(E_RunDestroyed);
    default:
      _wx_log_err("str_enum(E_THREAD_STATUS value): unknown value %d", value);
      break;
  }
  return wxString::Format("?%d?", value);
}

const char * str_enum(wxThreadKind value)
{
  switch (value)
  {
    CASE_const_ret_str(wxTHREAD_DETACHED);
    CASE_const_ret_str(wxTHREAD_JOINABLE);
    default:
      _wx_log_err("str_enum(wxThreadKind value): unknown value %d", value);
      break;
  }
  return wxString::Format("?%d?", value);
}
