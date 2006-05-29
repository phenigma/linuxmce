//
// Author : Remus C.
//
// Changed by : ...
//

#ifndef _WX_THREAD_WRAPPER_H_
#define _WX_THREAD_WRAPPER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_thread_wrapper.h"
#endif

#include "wx_thread_cmd.h"

class wxThread_Wrapper : public wxThread
{
  protected:
    // should be created by a friend class
    wxThread_Wrapper(
        wxThreadKind eKind,
        wxThread_Cmd *pwxThread_Cmd,
        wxCriticalSection &rCriticalSection,
        int &rnRunningCount,
        wxSemaphore &rSemaphoreRunningAll,
        wxSemaphore &rSemaphoreRunning,
        wxThread_Cmd::E_STATUS &reStatus,
        type_ptr_void_fn_void p_fn_launch
        );
  public:
    virtual wxThreadError Create(); // creates a new thread in the suspended state
    virtual void * Entry(); // thread execution starts here
    virtual void OnExit();  // called when the thread exits normally or is stopped

  protected:
    wxThread_Cmd *v_pwxThread_Cmd;

    wxCriticalSection &v_rCriticalSection; // protect relevant thread data
    int &v_rnRunningCount; // need to check current status without changing it
    wxSemaphore &v_rSemaphoreRunningAll; // used for try-wait, all threads
    wxSemaphore &v_rSemaphoreRunning; // used for try-wait, this thread
    wxThread_Cmd::E_STATUS &v_reStatus;

    type_ptr_void_fn_void v_p_fn_launch;

  private:
    friend class wxThread_Cmd;
    friend wxThread_Cmd * ptr_wxThread_Cmd();
};

#endif
// _WX_THREAD_WRAPPER_H_
