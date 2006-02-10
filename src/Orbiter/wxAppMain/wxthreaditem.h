#ifndef _WXTHREADITEM_H_
#define _WXTHREADITEM_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxthreaditem.cpp"
#endif

#include "wxthreadinfo.h"

class wxThreadItem : public wxThread, public wxEvtHandler
{
protected:
  // should be created by wxThreadBag
  wxThreadItem(wxThreadInfo *pThreadInfo);
public:
  virtual void * Entry(); // thread execution starts here
  virtual void OnExit();  // called when the thread exits normally or is stopped
  virtual void Exit(ExitCode exitcode=0); // call this to stop the thread

  void SetExitCode(int nExitCode); // does not exit

  bool ShouldCancel();

  wxThreadIdType GetIdWx();     // id - wx id, not unique in OS
  wxThreadIdType GetIdOs();     // id - unique in OS

protected:
  void OnThreadCommandEvent(wxCommandEvent& event);
  wxThreadInfo *v_pInfo;

private:
  friend class wxThreadBag;
  friend class wxThreadInfo;
  DECLARE_EVENT_TABLE()
    ;
};

// should be called from the thread
// return true if the thread should be stopped
bool wxIdleThreadShouldStop();

wxThreadItem * ptrThreadItem();

#endif
// _WXTHREADITEM_H_
