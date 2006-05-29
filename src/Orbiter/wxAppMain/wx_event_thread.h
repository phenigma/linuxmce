//
// Author : Remus C.
//
// Changed by : ...
//

#ifndef _WX_EVENT_THREAD_H_
#define _WX_EVENT_THREAD_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_event_thread.h"
#endif

// custom event used by : wxThread_Bag, wxThread_Cmd, wxThread_Wrapper

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVTC_THREAD, ID_EVTC_THREAD)
    END_DECLARE_EVENT_TYPES();

#define EVTC_THREAD(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVTC_THREAD, \
        id, \
        wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#endif
// _WX_EVENT_THREAD_H_
