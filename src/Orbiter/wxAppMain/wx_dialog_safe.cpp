//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_dialog_safe.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_dialog_safe.h"
#include "wxdialog_roomwizard.h"
#include "wxdialog_waitgrid.h"
#include "wxdialog_waitlist.h"
#include "wxdialog_waituser.h"

const char * Get_ClassName(E_DIALOG_TYPE e_dialog_type)
{
    switch (e_dialog_type)
    {
        case E_Dialog_RoomWizard: return "wxDialog_RoomWizard";
        case E_Dialog_WaitGrid:   return "wxDialog_WaitGrid";
        case E_Dialog_WaitList:   return "wxDialog_WaitList";
        case E_Dialog_WaitUser:   return "wxDialog_WaitUser";
        default: break;
    } // switch (e_dialog_type)
    _WX_LOG_ERR("unknown value '%s'", _str_enum(e_dialog_type));
    return "";
}

const char * Get_WindowName(E_DIALOG_TYPE e_dialog_type)
{
    switch (e_dialog_type)
    {
        case E_Dialog_RoomWizard: return SYMBOL_WXDIALOG_ROOMWIZARD_TITLE;
        case E_Dialog_WaitGrid:   return SYMBOL_WXDIALOG_WAITGRID_TITLE;
        case E_Dialog_WaitList:   return SYMBOL_WXDIALOG_WAITLIST_TITLE;
        case E_Dialog_WaitUser:   return SYMBOL_WXDIALOG_WAITUSER_TITLE;
        default: break;
    } // switch (e_dialog_type)
    _WX_LOG_ERR("unknown value '%s'", _str_enum(e_dialog_type));
    return "";
}

const long Get_WindowID(E_DIALOG_TYPE e_dialog_type)
{
    switch (e_dialog_type)
    {
        case E_Dialog_RoomWizard: return SYMBOL_WXDIALOG_ROOMWIZARD_IDNAME;
        case E_Dialog_WaitGrid:   return SYMBOL_WXDIALOG_WAITGRID_IDNAME;
        case E_Dialog_WaitList:   return SYMBOL_WXDIALOG_WAITLIST_IDNAME;
        case E_Dialog_WaitUser:   return SYMBOL_WXDIALOG_WAITUSER_IDNAME;
        default: break;
    } // switch (e_dialog_type)
    _WX_LOG_ERR("unknown value '%s'", _str_enum(e_dialog_type));
    return 0;
}

E_DIALOG_TYPE Get_Type(const char * sClassName)
{
    wxString str = sClassName;
    if (str == "wxDialog_RoomWizard") return E_Dialog_RoomWizard;
    if (str == "wxDialog_WaitGrid")   return E_Dialog_WaitGrid;
    if (str == "wxDialog_WaitList")   return E_Dialog_WaitList;
    if (str == "wxDialog_WaitUser")   return E_Dialog_WaitUser;
    _WX_LOG_ERR("unknown value '%s'", sClassName);
    return E_Dialog_Undefined;
}
