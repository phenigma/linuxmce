//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_EVENT_DIALOG_H_
#define _WX_EVENT_DIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_event_dialog.h"
#endif

// custom event used for wxDialog_Base and derived classes

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVTC_DIALOG, ID_EVTC_DIALOG)
END_DECLARE_EVENT_TYPES()
;

#define EVTC_DIALOG(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVTC_DIALOG, \
        id, \
        wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

const char * _str_event_dialog(wxCommandEvent &event);

#endif
// _WX_EVENT_DIALOG_H_
