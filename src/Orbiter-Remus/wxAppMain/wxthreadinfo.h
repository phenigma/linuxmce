#ifndef _WXTHREADINFO_H_
#define _WXTHREADINFO_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxthreadinfo.cpp"
#endif

//typedef void (* type_fn_thread_launch)();
typedef void (& type_fn_thread_launch)();

enum E_THREAD_STATUS
{
  E_Unitialized,  // thread object not yet created
  E_Created,      // created, not started
  // inside OnEntry
  E_RunStarted,   // started
  E_RunCancelled, // started, but cancelled
  E_RunExiting,   // started, but cancelled
  // inside OnExit
  E_RunEnded,     // finished, Exit_Code can be read
  // special case for bad thread
  E_RunDestroyed, // started, dissapeared
};

class wxThreadItem;

class wxThreadInfo
{
protected:
  // should be created by wxThreadBag
  wxThreadInfo(
    type_fn_thread_launch launch_fn,
    wxThreadKind eKind,
    const char *sName,
    wxCriticalSection &rCriticalSection,
    int &v_rnRunningCount,
    wxSemaphore &rSemaphoreRunningAll
    );

public:
  wxThreadKind GetKind();       // wxTHREAD_DETACHED or wxTHREAD_JOINABLE
  wxString GetName();           // thread name
  E_THREAD_STATUS GetStatus();  // current status
  int GetExitCode();  // valid if is_finished(), else 0

  wxThreadIdType GetIdWx(); // id - wx id, not unique in OS
  wxThreadIdType GetIdOs(); // id - unique in OS

  bool is_detached(); // kind == wxTHREAD_DETACHED
  bool is_finished(); // started and finished, object deleted
  bool check_alive(); // started and still running, can change status

protected:
  wxThreadKind v_eKind;
  type_fn_thread_launch v_launch_fn; // ptr to void fn called when the thread runs
  wxString v_sName;
  wxCriticalSection &v_rCriticalSection; // protecting: extern data
  int &v_rnRunningCount;
  wxSemaphore &v_rSemaphoreRunningAll; // used for try-wait, all threads
  E_THREAD_STATUS v_eStatus;
  bool v_bShouldCancel; // changed to signal the thread should be stopped
  int v_nExitCode;
  wxThreadItem *v_pItem;
  wxSemaphore v_oSemaphoreRunning; // used for try-wait, this thread

  bool v_bCheckStatusChanged; // used when status is corrected
  bool check_status(); // check if the thread still exists

public:
  const char * _str_thread_status(); // debug string

private:
  friend class wxThreadBag;
  friend class wxThreadItem;
};

const char * str_enum(E_THREAD_STATUS value);
const char * str_enum(wxThreadKind value);

#endif
// _WXTHREADINFO_H_
