//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_event_dialog.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_event_dialog.h"
#include "wx_dialog_types.h"

DEFINE_EVENT_TYPE(wxEVTC_DIALOG)
;

const char * _str_event_dialog(wxCommandEvent &event)
{
    return wxString::Format(
        "_str_event_dialog(id=%d[%s], str='%s', ptr=%p, int=%ld)",
        event.GetId(),
        _str_enum((E_ACTION_TYPE)event.GetId()),
        event.GetString().c_str(),
        event.GetClientData(),
        event.GetInt()
        );
}
