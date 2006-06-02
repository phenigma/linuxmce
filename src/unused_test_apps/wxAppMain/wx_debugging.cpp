//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_debugging.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_debugging.h"
#include "wxappmain.h"
#include "wx_event_thread.h"
#include "wx_thread_wrapper.h"
#include "wx_thread_bag.h"
#include "wx_safe_dialog.h"
#include "wx_dialog_all_include.h"

static string _g_aStr[] =
{
    "str aaaa",
    "str bbbb",
    "str cccc",
    "str dddd",
};
const int _g_nStr=WXSIZEOF(_g_aStr);
int _g_nPercent = 0;
int _g_nTimeoutSeconds = 5;
static map<string, bool> _g_mapStrBool;
static map<int,string> _g_mapIntStr;
static list<int> _g_listIntSpeeds;

void _debug_global_init()
{
    for (int idx=0; idx<_g_nStr; idx++)
    {
        _g_mapIntStr[(idx+1)*10] = _g_aStr[idx];
        _g_mapStrBool[_g_aStr[idx]] = (idx % (2+idx));
    }
    _g_listIntSpeeds.push_back(-20);
    _g_listIntSpeeds.push_back(-10);
    _g_listIntSpeeds.push_back(0);
    _g_listIntSpeeds.push_back(10);
    _g_listIntSpeeds.push_back(20);
};

void _debug_init(class CallBackData *pCallBackData=NULL)
{
    static wxCriticalSection oCriticalSection;
    wxCriticalSectionLocker lock(oCriticalSection);
    // initialization
    static int iLoop=0;
    iLoop++;
    int i = iLoop % _g_nStr;
    _g_nPercent = i * 100 / _g_nStr;
    for (int idx=0; idx<_g_nStr; idx++)
    {
        _g_mapStrBool[_g_aStr[idx]] = (i % (2+idx));
    }
    _LOG_DBG("iLoop=%d, nPercent=%d, nTimeoutSeconds=%d", iLoop, _g_nPercent, _g_nTimeoutSeconds);
    // loop continue
    if (PositionCallBackData *pCallData = dynamic_cast<PositionCallBackData *>(pCallBackData))
    {
        pCallData->m_rectPosition.X = 500;
        pCallData->m_rectPosition.Y = 200;
        pCallData->m_rectPosition.Width = 300;
        pCallData->m_rectPosition.Height = 100;
        //pCallData->m_bShowFullScreen = bool( (1+iLoop) % 2 );
    }
    // loop return
    if (RoomWizardCallBackData *pCallData = dynamic_cast<RoomWizardCallBackData *>(pCallBackData))
    {
#ifdef USE_DEBUG_CODE
        if (pCallData->map_room_types.size() != 0)
            return;
        pCallData->map_room_types[101] = 1;
        pCallData->map_fn_GetRoomTypeName[101] = "Living Room";
        pCallData->map_room_types[102] = 2;
        pCallData->map_fn_GetRoomTypeName[102] = "Bathroom";
        pCallData->map_room_types[103] = 3;
        pCallData->map_fn_GetRoomTypeName[103] = "Bedroom";
#endif // USE_DEBUG_CODE
        return;
    }
    if (WaitUserGridCallBackData *pCallData = dynamic_cast<WaitUserGridCallBackData *>(pCallBackData))
    {
        pCallData->m_sMessage = _g_aStr[i];
        pCallData->m_mapChildDevices = _g_mapStrBool;
        pCallData->m_nPercent = _g_nPercent;
        //pCallData->m_bShowFullScreen = (iLoop % 2) ? true : false;
        return;
    }
    if (WaitUserListCallBackData *pCallData = dynamic_cast<WaitUserListCallBackData *>(pCallBackData))
    {
        pCallData->m_sMessage = _g_aStr[i];
        pCallData->m_nPercent = _g_nPercent;
        //pCallData->m_bShowFullScreen = (iLoop % 2) ? true : false;
        return;
    }
    if (WaitUserPromptCallBackData *pCallData = dynamic_cast<WaitUserPromptCallBackData *>(pCallBackData))
    {
        pCallData->m_sMessage = _g_aStr[i];
        pCallData->m_nTimeoutSeconds = _g_nTimeoutSeconds;
        pCallData->m_mapPrompts = _g_mapIntStr;
        //pCallData->m_bShowFullScreen = (iLoop % 2) ? true : false;
        return;
    }
    if (SpeedControlCallBackData *pCallData = dynamic_cast<SpeedControlCallBackData *>(pCallBackData))
    {
        pCallData->m_eStyle = SpeedControlCallBackData::Style( 1 + iLoop % 4 );
        pCallData->m_listSpeeds = _g_listIntSpeeds;
        pCallData->m_nSpeed = 10;
        pCallData->m_nTimeStart = 10234;
        pCallData->m_nTimeNow = 20234;
        pCallData->m_nTimeEnd = 90234;
        pCallData->m_nSeekToPos = 70234;
        return;
    }
    if (VolumeControlCallBackData *pCallData = dynamic_cast<VolumeControlCallBackData *>(pCallBackData))
    {
        pCallData->m_eStyle = VolumeControlCallBackData::Style( 1 + iLoop % 2 );
        pCallData->m_nPositions = 10;
        pCallData->m_nCrtPosition = iLoop % ( pCallData->m_nPositions + 1 );
        // new dimensions
        pCallData->m_rectPosition.Width = 1000;
        pCallData->m_rectPosition.Height = 200;
        return;
    }
    if (LightControlCallBackData *pCallData = dynamic_cast<LightControlCallBackData *>(pCallBackData))
    {
        pCallData->m_nPositions = 10;
        pCallData->m_nCrtPosition = iLoop % ( pCallData->m_nPositions + 1 );
        // new dimensions
        pCallData->m_rectPosition.Width = 200;
        pCallData->m_rectPosition.Height = 1000;
        return;
    }
};

template <class wxClassName, class ExternData_CallBackData>
bool CallRefresh()
{
    wxClassName *pwxDialog = ptr_wxDialogByType<wxClassName>();
    if (pwxDialog)
    {
        _LOG_DBG("%s", Get_ClassName(Get_Type<wxClassName>()));
        ExternData_CallBackData *pCallBackData = new ExternData_CallBackData();
        _debug_init(pCallBackData);
        Safe_Gui_Refresh(pwxDialog, pCallBackData);
        wxDELETE(pCallBackData);
        return true;
    }
    return false;
}

void _debug_refresh_update()
{
#ifdef USE_DEBUG_CODE
    bool bInThread = (! ::wxIsMainThread());
    bool bOneActive = true;
    do
    {
        if ( wxIdleThreadShouldStop() || (! bOneActive) )
            break;
        bOneActive = false;
        bOneActive = CallRefresh<wxDialog_RoomWizard, RoomWizardCallBackData>() || bOneActive;
        bOneActive = CallRefresh<wxDialog_WaitGrid, WaitUserGridCallBackData>() || bOneActive;
        bOneActive = CallRefresh<wxDialog_WaitList, WaitUserListCallBackData>() || bOneActive;
        bOneActive = CallRefresh<wxDialog_WaitUser, WaitUserPromptCallBackData>() || bOneActive;
        bOneActive = CallRefresh<wxDialog_SpeedControl, SpeedControlCallBackData>() || bOneActive;
        bOneActive = CallRefresh<wxDialog_VolumeControl, VolumeControlCallBackData>() || bOneActive;
        bOneActive = CallRefresh<wxDialog_LightControl, LightControlCallBackData>() || bOneActive;
        if (bInThread)
        {
            wx_sleep(2);
        }
    } while (bInThread);
#endif // USE_DEBUG_CODE
};

template <class wxClassName>
void _debug_show_dlg_safe(CallBackData *pCallBackData=NULL)
{
#ifdef USE_DEBUG_CODE
    _LOG_DBG("Safe_START");
    wxClassName *pWin = NULL;
    _LOG_DBG("Safe_Show");
    pWin = Safe_CreateUnique<wxClassName>(pCallBackData);
    Safe_Show<wxClassName>(pWin);
    _LOG_DBG("Safe_Close");
    Safe_Close<wxClassName>(pWin);
    _LOG_DBG("Safe_Show Again");
    pWin = Safe_CreateUnique<wxClassName>(pCallBackData);
    Safe_Show<wxClassName>(pWin);
    _LOG_DBG("Safe_ShowModal");
    pWin = Safe_CreateUnique<wxClassName>(pCallBackData);
    Safe_ShowModal<wxClassName>(pWin);
    _LOG_DBG("Safe_END");
#endif // USE_DEBUG_CODE
}

template <class wxClassName>
void _debug_show_dlg_pdac(CallBackData *pCallBackData=NULL)
{
#ifdef USE_DEBUG_CODE
    _LOG_DBG("PDAC_START");
    Task *pTask = NULL;
    _LOG_DBG("cbOnDialogCreate");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _LOG_DBG("cbOnDialogRefresh 1");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _LOG_DBG("cbOnDialogRefresh 2");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _LOG_DBG("cbOnDialogDelete");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogDelete, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _LOG_DBG("cbOnDialogCreate Again");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTaskAndWait(pTask);
    _LOG_DBG("cbOnDialogRefresh 3");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _LOG_DBG("cbOnDialogRefresh 4");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _LOG_DBG("cbOnDialogWaitUser");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogWaitUser, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _LOG_DBG("PDAC_END");
#endif // USE_DEBUG_CODE
}

void _debug_show_dlg_safe_all()
{
    {
        RoomWizardCallBackData *pCallBackData = new RoomWizardCallBackData;
        _debug_init(pCallBackData);
        _debug_show_dlg_safe<wxDialog_RoomWizard>(pCallBackData);
        wxDELETE(pCallBackData);
    }
    {
        WaitUserGridCallBackData *pCallBackData = new WaitUserGridCallBackData;
        _debug_init(pCallBackData);
        _debug_show_dlg_safe<wxDialog_WaitGrid>(pCallBackData);
        wxDELETE(pCallBackData);
    }
    {
        WaitUserListCallBackData *pCallBackData = new WaitUserListCallBackData;
        _debug_init(pCallBackData);
        _debug_show_dlg_safe<wxDialog_WaitList>(pCallBackData);
        wxDELETE(pCallBackData);
    }
    {
        WaitUserPromptCallBackData *pCallBackData = new WaitUserPromptCallBackData;
        _debug_init(pCallBackData);
        _debug_show_dlg_safe<wxDialog_WaitUser>(pCallBackData);
        wxDELETE(pCallBackData);
    }
}

void _debug_show_dlg_pdac_all()
{
    {
        RoomWizardCallBackData *pCallBackData = new RoomWizardCallBackData;
        _debug_init(pCallBackData);
        _debug_show_dlg_pdac<wxDialog_RoomWizard>(pCallBackData);
    }
    {
        WaitUserGridCallBackData *pCallBackData = new WaitUserGridCallBackData;
        _debug_init(pCallBackData);
        _debug_show_dlg_pdac<wxDialog_WaitGrid>(pCallBackData);
    }
    {
        WaitUserListCallBackData *pCallBackData = new WaitUserListCallBackData;
        _debug_init(pCallBackData);
        _debug_show_dlg_pdac<wxDialog_WaitList>(pCallBackData);
    }
    {
        WaitUserPromptCallBackData *pCallBackData = new WaitUserPromptCallBackData;
        _debug_init(pCallBackData);
        _debug_show_dlg_pdac<wxDialog_WaitUser>(pCallBackData);
    }
}

void _debug_show_dlg_all()
{
    //_debug_show_dlg_safe_all();
    _debug_show_dlg_pdac_all();
}

void _debug_thread_block()
{
    _LOG_DBG();
    _debug_show_dlg_all();
    _LOG_DBG("Loop");
    for ( int i = 0; i < 5; i++ )
    {
        wx_post_event(wxGetApp().ptr_ThreadBag(), wxEVTC_THREAD, wxThread_Cmd::E_EventIdle, "_debug_thread_block()", ptr_wxThread_Cmd());
        wx_sleep(1);
    }
    _LOG_DBG(";;");
}

void _debug_thread_nonblock()
{
    _LOG_DBG();
    _debug_show_dlg_all();
    _LOG_DBG("Loop");
    for ( int i = 0; i < 10; i++ )
    {
        if ( wxIdleThreadShouldStop() )
        {
            _LOG_DBG("Should Stop Now");
            return;
        }
        wx_post_event(wxGetApp().ptr_ThreadBag(), wxEVTC_THREAD, wxThread_Cmd::E_EventIdle, "_debug_thread_nonblock()", ptr_wxThread_Cmd());
        wx_sleep(0, 500);
    }
    _LOG_DBG(";;");
}
