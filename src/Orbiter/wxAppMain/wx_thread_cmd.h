//
// Author : Remus C.
//
// Changed by : ...
//

#ifndef _WX_THREAD_CMD_H_
#define _WX_THREAD_CMD_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_thread_cmd.h"
#endif

#include "wx_helpers.h"

class wxThread_Wrapper;
class wxThread_Bag;

class wxThread_Cmd
{
  public:
    enum E_STATUS
        {
            E_Unitialized = 0, // thread object not yet created
            E_Unused = 100,
            E_Created,         // created, not started
            E_CreateError,     // create error
            // ( event sent by thread )
            // inside OnEntry
            E_RunStarted,      // started
            E_RunCancelled,    // started, but cancelled
            E_RunExiting,      // started, but cancelled
            // ( event sent by thread )
            // inside OnExit
            E_RunEnded,        // finished, exit code should be set by now
            // special case for bad thread
            E_RunDestroyed,    // started, destroyed
            // used only in events
            E_EventIdle,
        };

    // if p_fn_launch!=NULL, Run() is called
    wxThread_Cmd(wxThreadKind eKind, const char *sName, type_ptr_void_fn_void p_fn_launch=NULL);
    virtual ~wxThread_Cmd();

    bool Start();
    bool Stop();   // only info object valid after this

    // default implementation call p_fn_launch
    virtual void Run();

    wxThreadKind GetKind(); // wxTHREAD_DETACHED or wxTHREAD_JOINABLE
    wxString GetName();     // thread name
    E_STATUS GetStatus();   // current status

    int GetExitCode();      // valid if IsFinished(), else 0
    void SetExitCode(int nExitCode); // does not exit, can be called any time

    bool IsDetached(); // kind == wxTHREAD_DETACHED
    bool IsRunning(); // started and still running
    bool IsCancelled(); // received cancel command
    bool IsFinished(); // started and finished, object deleted

    bool TestDestroy();

  protected:
    wxThreadKind v_eKind;
    wxString v_sName;
    type_ptr_void_fn_void v_p_fn_launch; // ptr to void fn called when the thread runs

    E_STATUS v_eStatus;
    bool v_bShouldCancel; // changed to signal the thread should be stopped
    int v_nExitCode;
    wxThread_Wrapper *v_pwxThread_Wrapper;
    wxSemaphore v_oSemaphoreRunning; // used for try-wait, this thread

  private:
    // no copy ctor/assignment operator
    wxThread_Cmd(const wxThread_Cmd&);
    wxThread_Cmd& operator=(const wxThread_Cmd&);

    friend class wxThread_Bag;
    friend const char * _str_thread_status(wxThread_Cmd *pwxThread_Cmd);
};

WX_DEFINE_ARRAY_PTR(wxThread_Cmd *, wxArray_p_wxThread_Cmd);

const char * _str_enum(wxThread_Cmd::E_STATUS value);
const char * _str_enum(wxThreadKind value);

#endif
// _WX_THREAD_CMD_H_
