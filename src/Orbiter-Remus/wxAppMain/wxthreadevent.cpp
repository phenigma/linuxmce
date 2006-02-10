#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wxthreadevent.h"
#endif
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxthreadevent.h"
#include "wx_other.h"

DEFINE_EVENT_TYPE(wxEVT_THREAD_COMMAND)
  ;

//wxEvent * wxEVT_THREAD_COMMAND::Clone(void) const
//{
//  return new wxEVT_THREAD_COMMAND(*this);
//}

void wxPostThreadEvent(wxEvtHandler *pDestEvtHandler, E_THREAD_COMMAND_TYPE event_type, const char *str/*=""*/, void *pData/*=NULL*/, int nInt/*=0*/ )
{
  wxCommandEvent event( wxEVT_THREAD_COMMAND, event_type );
  event.SetString(str);
  event.SetClientData(pData);
  event.SetInt(nInt);
  _wx_log_nfo("::wxPostThreadEvent() : %s", _str_thread_event(event));
  ::wxPostEvent( pDestEvtHandler, event );
}

const char * str_enum(E_THREAD_COMMAND_TYPE value)
{
  switch (value)
  {
    CASE_const_ret_str(Evt_RunStarted);
    CASE_const_ret_str(Evt_RunIdle);
    CASE_const_ret_str(Evt_RunCancelled);
    CASE_const_ret_str(Evt_RunExiting);
    CASE_const_ret_str(Evt_RunEnded);
    CASE_const_ret_str(Evt_StopPlease);
    default:
      _wx_log_err("str_enum(E_THREAD_COMMAND_TYPE): unknown value %d", value);
      break;
  }
  return wxString::Format("?%d?", value);
}

const char * _str_thread_event(wxCommandEvent &event)
{
  return wxString::Format(
    "_str_thread_event(id=%d[%s], str='%s', ptr=%p, int=%ld)",
    event.GetId(),
    str_enum((E_THREAD_COMMAND_TYPE)event.GetId()),
    event.GetString().c_str(),
    event.GetClientData(),
    event.GetInt()
    );
}
