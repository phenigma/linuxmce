//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxevent_thread.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxevent_thread.h"
#include "wxthread_cmd.h"

DEFINE_EVENT_TYPE(wxEVTC_THREAD)
;

const char * _str_event_thread(wxCommandEvent &event)
{
    return wxString::Format(
        "_str_event_thread(id=%d[%s], str='%s', ptr=%p, int=%ld)",
        event.GetId(),
        _str_enum((wxThread_Cmd::E_STATUS)event.GetId()),
        event.GetString().c_str(),
        event.GetClientData(),
        event.GetInt()
        );
}
