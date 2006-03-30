//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_safe_dialog.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_safe_dialog.h"
#include "wxdialog_roomwizard.h"
#include "wxdialog_waitgrid.h"
#include "wxdialog_waitlist.h"
#include "wxdialog_waituser.h"

wxCriticalSection g_oCriticalDialogAction;

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

bool Process_Dialog_Action(E_DIALOG_TYPE e_dialog_type, E_ACTION_TYPE action, Data_Holder_Dialog *pData_Holder_Dialog)
{
    _WX_LOG_NFO("class='%s' action='%s'", _str_enum(e_dialog_type), _str_enum(action));
    _COND_RET(pData_Holder_Dialog != NULL, false);
    switch (action)
    {
        case E_Action_Close:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Close<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Close<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Close<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Close<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return false;
            }
            break;
        }
        case E_Action_Create:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->pWindow = Safe_Create<wxDialog_RoomWizard>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->pWindow = Safe_Create<wxDialog_WaitGrid>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->pWindow = Safe_Create<wxDialog_WaitList>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->pWindow = Safe_Create<wxDialog_WaitUser>(pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return false;
            }
            break;
        }
        case E_Action_Create_Unique:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->pWindow = Safe_CreateUnique<wxDialog_RoomWizard>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->pWindow = Safe_CreateUnique<wxDialog_WaitGrid>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->pWindow = Safe_CreateUnique<wxDialog_WaitList>(pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->pWindow = Safe_CreateUnique<wxDialog_WaitUser>(pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return false;
            }
            break;
        }
        case E_Action_DataLoad:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Gui_DataLoad<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Gui_DataLoad<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Gui_DataLoad<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Gui_DataLoad<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return false;
            }
            break;
        }
        case E_Action_DataSave:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Gui_DataSave<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Gui_DataSave<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Gui_DataSave<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Gui_DataSave<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return false;
            }
            break;
        }
        case E_Action_Refresh:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Gui_Refresh<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Gui_Refresh<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Gui_Refresh<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Gui_Refresh<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->pExternData); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return false;
            }
            break;
        }
        case E_Action_Show:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->bRetCode = Safe_Show<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->bRetCode = Safe_Show<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->bRetCode = Safe_Show<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->bRetCode = Safe_Show<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow, pData_Holder_Dialog->bShow); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return false;
            }
            break;
        }
        case E_Action_ShowModal:
        {
            switch (e_dialog_type)
            {
                case E_Dialog_RoomWizard: pData_Holder_Dialog->nRetCode = Safe_ShowModal<wxDialog_RoomWizard>((wxDialog_RoomWizard *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitGrid: pData_Holder_Dialog->nRetCode = Safe_ShowModal<wxDialog_WaitGrid>((wxDialog_WaitGrid *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitList: pData_Holder_Dialog->nRetCode = Safe_ShowModal<wxDialog_WaitList>((wxDialog_WaitList *)pData_Holder_Dialog->pWindow); break;
                case E_Dialog_WaitUser: pData_Holder_Dialog->nRetCode = Safe_ShowModal<wxDialog_WaitUser>((wxDialog_WaitUser *)pData_Holder_Dialog->pWindow); break;
                default: _WX_LOG_ERR("bad dialog type : %d", e_dialog_type); return false;
            }
            break;
        }
        default:
            _WX_LOG_ERR("bad action : %d", action);
            break;
    } // switch (action)
    wx_semaphore_post(pData_Holder_Dialog->oSemaphore, pData_Holder_Dialog->bInThread);
    _WX_LOG_NFO("DONE WITH class='%s' action='%s'", _str_enum(e_dialog_type), _str_enum(action));
    return true;
}
