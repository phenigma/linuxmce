//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_DIALOG_TYPES_H_
#define _WX_DIALOG_TYPES_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_dialog_types.h"
#endif

enum E_DIALOG_TYPE
{
    E_Dialog_Undefined = 0, // should not be used
    E_Dialog_RoomWizard,
    E_Dialog_WaitGrid,
    E_Dialog_WaitList,
    E_Dialog_WaitUser,
    E_Dialog_None,
};
const char * _str_enum(E_DIALOG_TYPE value);

enum E_ACTION_TYPE
{
    E_Action_Undefined = 0,
    E_Action_Close,
    E_Action_Create,
    E_Action_Create_Unique,
    E_Action_DataLoad,
    E_Action_DataSave,
    E_Action_Refresh,
    E_Action_Show,
    E_Action_ShowModal,
    E_Action_WaitUser,
};
const char * _str_enum(E_ACTION_TYPE value);

#endif
// _WX_DIALOG_TYPES_H_
