//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXTHREAD_BAG_H_
#define _WXTHREAD_BAG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxthread_bag.h"
#endif

#include "wxthread_cmd.h"

// all the threads except main should be monitored

class wxThread_Bag : public wxEvtHandler
{
  public:
    wxThread_Bag();
    ~wxThread_Bag();

    void Add(wxThread_Cmd *pwxThread_Cmd);
    void Delete(wxThread_Cmd *pwxThread_Cmd); // stop if running
    void DestroyAll();

    size_t GetCount();
    size_t GetRunningCount();

  protected:
    // event callbacks
    void OnEvent_Thread(wxCommandEvent& event);

    wxCriticalSection v_oCriticalSection; // protect relevant thread data
    int v_nRunningCount; // need to check current status without changing it
    wxSemaphore v_oSemaphoreRunningAll; // used for try-wait, all threads
    wxArray_p_wxThread_Cmd v_apwxThread_Cmd; // array of ptr to thread command class

  private:
    friend class wxThread_Cmd;
    DECLARE_EVENT_TABLE()
        ;
};

void _wx_log_status_threads();

extern wxThread_Bag *g_pwxThread_Bag;

#endif
// _WXTHREAD_BAG_H_
