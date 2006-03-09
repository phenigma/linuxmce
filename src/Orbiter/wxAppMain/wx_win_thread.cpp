//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_win_thread.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_win_thread.h"

const char * _str_enum(E_wxDialog_Class_Type value)
{
    switch (value)
    {
        CASE_const_ret_str(E_wxDialog_Undefined);
        CASE_const_ret_str(E_wxDialog_RoomWizard);
        CASE_const_ret_str(E_wxDialog_WaitGrid);
        CASE_const_ret_str(E_wxDialog_WaitList);
        CASE_const_ret_str(E_wxDialog_WaitUser);
        default:
            _WX_LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}
