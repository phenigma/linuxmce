//
// Author : C Remus
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
    if (RoomWizardCallBackData *pCallData = dynamic_cast<RoomWizardCallBackData *>(pCallBackData))
    {
        pCallData->m_rectPosition.X = 500;
        pCallData->m_rectPosition.Y = 200;
        pCallData->m_rectPosition.Width = 300;
        pCallData->m_rectPosition.Height = 100;
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
        pCallData->m_nPercent = 0;
        return;
    }
    if (WaitUserListCallBackData *pCallData = dynamic_cast<WaitUserListCallBackData *>(pCallBackData))
    {
        pCallData->m_nPercent = 0;
        return;
    }
    if (WaitUserPromptCallBackData *pCallData = dynamic_cast<WaitUserPromptCallBackData *>(pCallBackData))
    {
        pCallData->m_nTimeoutSeconds = 0;
        return;
    }
    if (SpeedControlCallBackData *pCallData = dynamic_cast<SpeedControlCallBackData *>(pCallBackData))
    {
        //pCallData->m_eStyle = SpeedControlCallBackData::TIME;
        //pCallData->m_eStyle = SpeedControlCallBackData::TIME_SEEK;
        //pCallData->m_eStyle = SpeedControlCallBackData::TIME_SPEED;
        //pCallData->m_eStyle = SpeedControlCallBackData::SPEED;
        pCallData->m_eStyle = SpeedControlCallBackData::TIME_SEEK;
        pCallData->m_listSpeeds = _g_listIntSpeeds;
        pCallData->m_nSpeed = 10;
        pCallData->m_nTimeStart = 10234;
        pCallData->m_nTimeEnd = 50234;
        pCallData->m_nTimeNow = 90234;
        pCallData->m_nSeekToPos = 70234;
        pCallData->m_rectPosition.X = 500;
        pCallData->m_rectPosition.Y = 200;
        pCallData->m_rectPosition.Width = 300;
        pCallData->m_rectPosition.Height = 100;
        return;
    }
};

template <class wxClassName>
void _debug_show_dlg_safe(CallBackData *pCallBackData=NULL)
{
#ifdef USE_DEBUG_CODE
    _WX_LOG_DBG("Safe_START");
    wxClassName *pWin = NULL;
    _WX_LOG_DBG("Safe_Show");
    pWin = Safe_CreateUnique<wxClassName>(pCallBackData);
    Safe_Show<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_Close");
    Safe_Close<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_Show Again");
    pWin = Safe_CreateUnique<wxClassName>(pCallBackData);
    Safe_Show<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_ShowModal");
    pWin = Safe_CreateUnique<wxClassName>(pCallBackData);
    Safe_ShowModal<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_END");
#endif // USE_DEBUG_CODE
}

template <class wxClassName>
void _debug_show_dlg_pdac(CallBackData *pCallBackData=NULL)
{
#ifdef USE_DEBUG_CODE
    _WX_LOG_DBG("PDAC_START");
    Task *pTask = NULL;
    _WX_LOG_DBG("cbOnDialogCreate");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _WX_LOG_DBG("cbOnDialogRefresh 1");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _WX_LOG_DBG("cbOnDialogRefresh 2");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _WX_LOG_DBG("cbOnDialogDelete");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogDelete, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _WX_LOG_DBG("cbOnDialogCreate Again");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTaskAndWait(pTask);
    _WX_LOG_DBG("cbOnDialogRefresh 3");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _WX_LOG_DBG("cbOnDialogRefresh 4");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _WX_LOG_DBG("cbOnDialogWaitUser");
    pTask = TaskManager::Instance().CreateTask(cbOnDialogWaitUser, Get_Type<wxClassName>(), pCallBackData);
    TaskManager::Instance().AddTask(pTask);
    _WX_LOG_DBG("PDAC_END");
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
    RoomWizardCallBackData *pCallBackData = new RoomWizardCallBackData;
    _debug_init(pCallBackData);
    _debug_show_dlg_pdac<wxDialog_RoomWizard>(pCallBackData);
    //wxDELETE(pCallBackData);
    _debug_show_dlg_pdac<wxDialog_WaitGrid>();
    _debug_show_dlg_pdac<wxDialog_WaitList>();
    _debug_show_dlg_pdac<wxDialog_WaitUser>();
}

void _debug_show_dlg_all()
{
    //_debug_show_dlg_safe_all();
    _debug_show_dlg_pdac_all();
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
        // initialization
        static int iLoop=0;
        int i = iLoop % _g_nStr;
        _g_nPercent = i * 100 / _g_nStr;
        for (int idx=0; idx<_g_nStr; idx++)
        {
            _g_mapStrBool[_g_aStr[idx]] = (i % (2+idx));
        }
        _WX_LOG_DBG("iLoop=%d, nPercent=%d, nTimeoutSeconds=%d", iLoop, _g_nPercent, _g_nTimeoutSeconds);
        // refresh update
        {
            wxDialog_WaitGrid *pwxDialog = ptr_wxDialogByType<wxDialog_WaitGrid>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitGrid");
                bOneActive = true;
                WaitUserGridCallBackData *pCallBackData = new WaitUserGridCallBackData();
                pCallBackData->m_sMessage = _g_aStr[i];
                pCallBackData->m_mapChildDevices = _g_mapStrBool;
                pCallBackData->m_nPercent = _g_nPercent;
                Safe_Gui_Refresh(pwxDialog, pCallBackData);
                wxDELETE(pCallBackData);
            }
        }
        {
            wxDialog_WaitList *pwxDialog = ptr_wxDialogByType<wxDialog_WaitList>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitList");
                bOneActive = true;
                WaitUserListCallBackData *pCallBackData = new WaitUserListCallBackData();
                pCallBackData->m_sMessage = _g_aStr[i];
                pCallBackData->m_nPercent = _g_nPercent;
                Safe_Gui_Refresh(pwxDialog, pCallBackData);
                wxDELETE(pCallBackData);
            }
        }
        {
            wxDialog_WaitUser *pwxDialog = ptr_wxDialogByType<wxDialog_WaitUser>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitUser");
                bOneActive = true;
                WaitUserPromptCallBackData *pCallBackData = new WaitUserPromptCallBackData();
                pCallBackData->m_sMessage = _g_aStr[i];
                pCallBackData->m_nTimeoutSeconds = _g_nTimeoutSeconds;
                pCallBackData->m_mapPrompts = _g_mapIntStr;
                Safe_Gui_Refresh(pwxDialog, pCallBackData);
                wxDELETE(pCallBackData);
            }
        }
        {
            wxDialog_SpeedControl *pwxDialog = ptr_wxDialogByType<wxDialog_SpeedControl>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_SpeedControl");
                bOneActive = true;
                SpeedControlCallBackData *pCallBackData = new SpeedControlCallBackData();
                _debug_init(pCallBackData);
                pCallBackData->m_eStyle = SpeedControlCallBackData::Style( 1 + iLoop % 4 );
                Safe_Gui_Refresh(pwxDialog, pCallBackData);
                wxDELETE(pCallBackData);
            }
        }
        iLoop++;
        if (bInThread)
        {
            wx_sleep(2);
        }
    } while (bInThread);
#endif // USE_DEBUG_CODE
};

void _debug_thread_block()
{
    _WX_LOG_DBG();
    _debug_show_dlg_all();
    _WX_LOG_DBG("Loop");
    for ( int i = 0; i < 5; i++ )
    {
        wx_post_event(wxGetApp().ptr_ThreadBag(), wxEVTC_THREAD, wxThread_Cmd::E_EventIdle, "_debug_thread_block()", ptr_wxThread_Cmd());
        wx_sleep(1);
    }
    _WX_LOG_DBG(";;");
}

void _debug_thread_nonblock()
{
    _WX_LOG_DBG();
    _debug_show_dlg_all();
    _WX_LOG_DBG("Loop");
    for ( int i = 0; i < 10; i++ )
    {
        if ( wxIdleThreadShouldStop() )
        {
            _WX_LOG_DBG("Should Stop Now");
            return;
        }
        wx_post_event(wxGetApp().ptr_ThreadBag(), wxEVTC_THREAD, wxThread_Cmd::E_EventIdle, "_debug_thread_nonblock()", ptr_wxThread_Cmd());
        wx_sleep(0, 500);
    }
    _WX_LOG_DBG(";;");
}
