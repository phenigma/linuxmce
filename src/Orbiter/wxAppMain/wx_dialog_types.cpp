//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_dialog_types.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_dialog_types.h"

const char * _str_enum(E_DIALOG_TYPE value)
{
    switch (value)
    {
        CASE_const_ret_str(E_Dialog_Undefined);
        CASE_const_ret_str(E_Dialog_RoomWizard);
        CASE_const_ret_str(E_Dialog_WaitGrid);
        CASE_const_ret_str(E_Dialog_WaitList);
        CASE_const_ret_str(E_Dialog_WaitUser);
        CASE_const_ret_str(E_Dialog_SpeedControl);
        CASE_const_ret_str(E_Dialog_VolumeControl);
        CASE_const_ret_str(E_Dialog_LightControl);
        default:
            _LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}

const char * _str_enum(E_ACTION_TYPE value)
{
    switch (value)
    {
        CASE_const_ret_str(E_Action_Undefined);
        CASE_const_ret_str(E_Action_Close);
        CASE_const_ret_str(E_Action_Create);
        CASE_const_ret_str(E_Action_Create_Unique);
        CASE_const_ret_str(E_Action_DataLoad);
        CASE_const_ret_str(E_Action_DataSave);
        CASE_const_ret_str(E_Action_Refresh);
        CASE_const_ret_str(E_Action_Show);
        CASE_const_ret_str(E_Action_ShowModal);
        CASE_const_ret_str(E_Action_WaitUser);
        CASE_const_ret_str(E_Action_WaitInitialized);
        default:
            _LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}
