#ifndef _WXTHREADBAG_H_
#define _WXTHREADBAG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxthreadbag.h"
#endif

#include "wxthreadinfo.h"
#include "wxthreaditem.h"

WX_DEFINE_ARRAY_PTR(wxThreadInfo *, wxArray_p_wxThreadInfo);
WX_DEFINE_ARRAY_PTR(wxThreadItem *, wxArray_p_wxThreadItem);

// all the threads except main should be monitored

class wxThreadBag : public wxEvtHandler
{
public:
  wxThreadBag();
  ~wxThreadBag();

  wxThreadInfo * Start(type_fn_thread_launch launch_fn, wxThreadKind eKind, const char *sName);
  void Stop(wxThreadInfo *pInfo);    // only info object valid after this
  void Delete(wxThreadInfo *pInfo); // stop if running

  void DestroyAll();

  size_t GetCount();
  size_t GetRunningCount();

protected:
  wxCriticalSection v_oCriticalSection; // protect relevant thread data
  wxArray_p_wxThreadInfo v_apInfo; // array of ptr to thread extern info
  wxArray_p_wxThreadItem v_apItem; // array of ptr to thread
  int v_nRunningCount; // need to check current status without changing it
  wxSemaphore v_oSemaphoreRunningAll; // used for try-wait, all threads

  bool is_ok_array(); // self check
  bool is_ok_ptr(wxThreadInfo *pInfo);

  // event callbacks
  void OnThreadCommandEvent(wxCommandEvent& event);

private:
  DECLARE_EVENT_TABLE()
    ;
};

const char * str_enum(wxSemaError value);
void _wx_log_status_threads();

extern wxThreadBag *g_pwxThreadBag;

#endif
// _WXTHREADBAG_H_
