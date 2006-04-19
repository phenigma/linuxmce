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
#include "wxdialog_roomwizard.h"
#include "wxdialog_waitgrid.h"
#include "wxdialog_waitlist.h"
#include "wxdialog_waituser.h"
#include "wx_safe_dialog.h"

wxArray_RoomItems _g_aRoomItems;

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

void _debug_init()
{
    for (int idx=0; idx<_g_nStr; idx++)
    {
        _g_mapIntStr[(idx+1)*10] = _g_aStr[idx];
        _g_mapStrBool[_g_aStr[idx]] = (idx % (2+idx));
    }
    _g_aRoomItems.Add( RoomItem("Living Room", 1, 0) );
    _g_aRoomItems.Add( RoomItem("Bathroom", 2, 0) );
    _g_aRoomItems.Add( RoomItem("Bedroom", 4, 0) );
};

void * _debug_RoomWizard()
{
    return &_g_aRoomItems;
};

template <class wxClassName>
void _debug_show_dlg_safe(void *pData=NULL)
{
#ifdef USE_DEBUG_CODE
    _WX_LOG_DBG("Safe_START");
    wxClassName *pWin = NULL;
    _WX_LOG_DBG("Safe_Show");
    pWin = Safe_CreateUnique<wxClassName>(pData);
    Safe_Show<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_Close");
    Safe_Close<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_Show Again");
    pWin = Safe_CreateUnique<wxClassName>(pData);
    Safe_Show<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_ShowModal");
    pWin = Safe_CreateUnique<wxClassName>(pData);
    Safe_ShowModal<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_END");
#endif // USE_DEBUG_CODE
}

template <class wxClassName>
void _debug_show_dlg_pdac(void *pCallBackData=NULL)
{
#ifdef USE_DEBUG_CODE
    _WX_LOG_DBG("PDAC_START");
    WMTask *pTask = NULL;
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
    _debug_show_dlg_safe<wxDialog_RoomWizard>(_debug_RoomWizard());
    _debug_show_dlg_safe<wxDialog_WaitGrid>();
    _debug_show_dlg_safe<wxDialog_WaitList>();
    _debug_show_dlg_safe<wxDialog_WaitUser>();
}

void _debug_show_dlg_pdac_all()
{
    _debug_show_dlg_pdac<wxDialog_RoomWizard>(_debug_RoomWizard());
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
                wxDialog_WaitGrid::Data_Refresh data_refresh = { _g_aStr[i], _g_mapStrBool, _g_nPercent };
                Safe_Gui_Refresh(pwxDialog, &data_refresh);
            }
        }
        {
            wxDialog_WaitList *pwxDialog = ptr_wxDialogByType<wxDialog_WaitList>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitList");
                bOneActive = true;
                wxDialog_WaitList::Data_Refresh data_refresh = { _g_aStr[i], _g_nPercent };
                Safe_Gui_Refresh(pwxDialog, &data_refresh);
            }
        }
        {
            wxDialog_WaitUser *pwxDialog = ptr_wxDialogByType<wxDialog_WaitUser>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitUser");
                bOneActive = true;
                wxDialog_WaitUser::Data_Refresh data_refresh = { _g_aStr[i], _g_nTimeoutSeconds, _g_mapIntStr };
                Safe_Gui_Refresh(pwxDialog, &data_refresh);
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

// compatibility code for debug
// wx implementation for task-manager
#ifdef USE_DEBUG_CODE

const char * _str_enum(CallBackType value)
{
    switch (value)
    {
        CASE_const_ret_str(cbOnDialog_Unused);
        CASE_const_ret_str(cbOnDialogCreate);
        CASE_const_ret_str(cbOnDialogDelete);
        CASE_const_ret_str(cbOnDialogRefresh);
        CASE_const_ret_str(cbOnDialogSave);
        CASE_const_ret_str(cbOnDialogWaitUser);
        default:
            _WX_LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}

const char * _str_task(WMTask *pWMTask)
{
    if (pWMTask == NULL)
        return wxString::Format("NULL task");
    return wxString::Format(
        "WMTask(TaskType=%s, DialogType=%s, pCallBackData=%p, TaskId=%d)",
        _str_enum(pWMTask->TaskType), _str_enum(pWMTask->DialogType), pWMTask->pCallBackData, pWMTask->TaskId
        );
}

WMTask::WMTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData, size_t TaskId)
        : TaskType(TaskType)
        , DialogType(DialogType)
        , pCallBackData(pCallBackData)
        , TaskId(TaskId)
        , b_IsWaiting(false)
        , v_oSemaphoreTaskWait(0, 1)
{
    //_WX_LOG_DBG("New %s", _str_task(this));
}

WMTaskManager::WMTaskManager()
        : v_oMutex(wxMUTEX_RECURSIVE)
        , v_nNextUniqueId(0)
{
    _WX_LOG_DBG();
}

WMTask * WMTaskManager::CreateTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData )
{
    //_WX_LOG_DBG("LOCK CreateTask()");
    wxMutexLocker lock(v_oMutex);
    //_WX_LOG_DBG("INTO CreateTask()");
    v_nNextUniqueId++;
    WMTask *pWMTask = new WMTask(TaskType, DialogType, pCallBackData, v_nNextUniqueId);
    _WX_LOG_DBG("DONE CreateTask(%s), size=%d", _str_task(pWMTask), v_apWMTask.GetCount());
    return pWMTask;
}

void WMTaskManager::AddTask(WMTask *pWMTask)
{
    _WX_LOG_DBG("AddTask(%s), size=%d", _str_task(pWMTask), v_apWMTask.GetCount());
    _COND_RET(pWMTask);
    //_WX_LOG_DBG("LOCK AddTask()");
    wxMutexLocker lock(v_oMutex);
    //_WX_LOG_DBG("INTO AddTask()");
    v_apWMTask.Add(pWMTask);
    _WX_LOG_DBG("DONE AddTask(%s), size=%d", _str_task(pWMTask), v_apWMTask.GetCount());
}

void WMTaskManager::AddTaskAndWait(WMTask *pWMTask)
{
    _WX_LOG_DBG("AddTaskAndWait(%s), size=%d", _str_task(pWMTask), v_apWMTask.GetCount());
    _COND_RET(pWMTask);
    {
        //_WX_LOG_DBG("LOCK AddTaskAndWait()");
        wxMutexLocker lock(v_oMutex);
        //_WX_LOG_DBG("INTO AddTaskAndWait()");
        v_apWMTask.Add(pWMTask);
        pWMTask->b_IsWaiting = true;
    }
    _WX_LOG_DBG("WAIT AddTaskAndWait(%s), size=%d", _str_task(pWMTask), v_apWMTask.GetCount());
    pWMTask->v_oSemaphoreTaskWait.Wait();
    _WX_LOG_DBG("DONE AddTaskAndWait(%s), size=%d", _str_task(pWMTask), v_apWMTask.GetCount());
}

WMTask *WMTaskManager::PopTask()
{
    //_WX_LOG_DBG();
    //_WX_LOG_DBG("LOCK PopTask()");
    wxMutexLocker lock(v_oMutex);
    //_WX_LOG_DBG("INTO PopTask()");
    if (v_apWMTask.IsEmpty())
        return NULL;
    _WX_LOG_DBG("NOT EMPTY PopTask()");
    WMTask *pWMTask = v_apWMTask[0];
    v_apWMTask.RemoveAt(0);
    _WX_LOG_DBG("DONE PopTask(%s), size=%d", _str_task(pWMTask), v_apWMTask.GetCount());
    return pWMTask;
}

void WMTaskManager::TaskProcessed(size_t nTaskId)
{
    _WX_LOG_DBG("TaskProcessed(%d)", nTaskId);
    _WX_LOG_DBG("LOCK TaskProcessed()");
    wxMutexLocker lock(v_oMutex);
    _WX_LOG_DBG("INTO TaskProcessed()");
    size_t n = v_apWMTask.GetCount();
    for (size_t i=0; i<n; i++)
    {
        WMTask *pWMTask = v_apWMTask[i];
        if (pWMTask->TaskId == nTaskId)
        {
            _WX_LOG_DBG("ITEM TaskProcessed() : i=%d", i);
            _COND(i == 0);
            if (pWMTask->b_IsWaiting)
            {
                _WX_LOG_DBG("WAIT TaskProcessed() : i=%d", i);
                pWMTask->v_oSemaphoreTaskWait.Post();
                _WX_LOG_DBG("CONT TaskProcessed() : i=%d", i);
            }
            v_apWMTask.RemoveAt(i);
            _WX_LOG_DBG("Done TaskProcessed(%s), size=%d", _str_task(pWMTask), v_apWMTask.GetCount());
        }
    }
    //--_WX_LOG_ERR("No task found with id=%d, size=%d", nTaskId, v_apWMTask.GetCount());
}

WMTaskManager TaskManager::_g_WMTaskManager;

#endif // USE_DEBUG_CODE
