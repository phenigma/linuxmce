#ifndef _WXTHREADEVENT_H_
#define _WXTHREADEVENT_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxthreadevent.cpp"
#endif

enum E_THREAD_COMMAND_TYPE
{
  // sent by thread, inside OnEntry
  Evt_RunStarted = 1,
  Evt_RunIdle,
  Evt_RunCancelled,
  Evt_RunExiting,
  // sent by thread, inside OnExit
  Evt_RunEnded,
  // received by thread
  Evt_StopPlease,
};

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(wxEVT_THREAD_COMMAND, wxID_HIGHEST + 10)
  END_DECLARE_EVENT_TYPES()
  ;

#define EVT_THREAD_COMMAND(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_THREAD_COMMAND, \
        id, \
        wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

void wxPostThreadEvent(wxEvtHandler *pDestEvtHandler, E_THREAD_COMMAND_TYPE event_type, const char *str="", void *pData=NULL, int nInt=0);

const char * str_enum(E_THREAD_COMMAND_TYPE value);
const char * _str_thread_event(wxCommandEvent &event); // debug string

#endif
// _WXTHREADEVENT_H_
