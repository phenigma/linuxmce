//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXEVENT_THREAD_H_
#define _WXEVENT_THREAD_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxevent_thread.h"
#endif

// custom event used by : wxThread_Bag, wxThread_Cmd, wxThread_Wrapper

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVTC_THREAD, wxID_HIGHEST + 10)
END_DECLARE_EVENT_TYPES()
;

#define EVTC_THREAD(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVTC_THREAD, \
        id, \
        wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

//

const char * _str_event_thread(wxCommandEvent &event); // debug string

#endif
// _WXEVENT_THREAD_H_
