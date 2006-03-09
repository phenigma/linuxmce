//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxevent_dialog.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxevent_dialog.h"

DEFINE_EVENT_TYPE(wxEVTC_DIALOG)
;

const char * _str_enum(E_DIALOG_ACTION value)
{
    switch (value)
    {
        CASE_const_ret_str(E_Dialog_Undefined);
        CASE_const_ret_str(E_Dialog_Create);
        CASE_const_ret_str(E_Dialog_Create_Unique);
        CASE_const_ret_str(E_Dialog_Show);
        CASE_const_ret_str(E_Dialog_ShowModal);
        CASE_const_ret_str(E_Dialog_Close);
        CASE_const_ret_str(E_Dialog_Refresh);
        default:
            _WX_LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}

const char * _str_event_dialog(wxCommandEvent &event)
{
    return wxString::Format(
        "_str_event_dialog(id=%d[%s], str='%s', ptr=%p, int=%ld)",
        event.GetId(),
        _str_enum((E_DIALOG_ACTION)event.GetId()),
        event.GetString().c_str(),
        event.GetClientData(),
        event.GetInt()
        );
}
