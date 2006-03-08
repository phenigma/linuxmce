//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WXEVENT_DIALOG_H_
#define _WXEVENT_DIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxevent_dialog.h"
#endif

// custom event used by wxDialog_Base

enum E_DIALOG_ACTION
{
    E_Dialog_Undefined = 0,
    E_Dialog_Create,
    E_Dialog_Create_Unique,
    E_Dialog_Show,
    E_Dialog_ShowModal,
    E_Dialog_Close,
    E_Dialog_Refresh,
};

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVTC_DIALOG, wxID_HIGHEST + 10)
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

const char * _str_enum(E_DIALOG_ACTION value);
const char * _str_event_dialog(wxCommandEvent &event); // debug string

#endif
// _WXEVENT_DIALOG_H_
