//
// Author : Remus C.
//
// Changed by : ...
//

// extern code copied here
// compatibility code for debug-app

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_extern_helpers.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_extern_helpers.h"

const char * _str_enum(SpeedControlCallBackData::Style value)
{
    switch (value)
    {
        CASE_const_ret_str(SpeedControlCallBackData::UNUSED);
        CASE_const_ret_str(SpeedControlCallBackData::TIME);
        CASE_const_ret_str(SpeedControlCallBackData::TIME_SEEK);
        CASE_const_ret_str(SpeedControlCallBackData::TIME_SPEED);
        CASE_const_ret_str(SpeedControlCallBackData::SPEED);
        default:
            _LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}

const char * _str_enum(VolumeControlCallBackData::Style value)
{
    switch (value)
    {
        CASE_const_ret_str(VolumeControlCallBackData::UNUSED);
        CASE_const_ret_str(VolumeControlCallBackData::RULER);
        CASE_const_ret_str(VolumeControlCallBackData::SPEED);
        default:
            _LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}

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
            _LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}
#endif // USE_DEBUG_CODE

#ifdef USE_DEBUG_CODE

const char * _str_task(Task *pTask)
{
    if (pTask == NULL)
        return wxString::Format("NULL task");
    return wxString::Format(
        "Task(TaskType='%s', DialogType='%s', pCallBackData=%p, TaskId=%d)",
        _str_enum(pTask->TaskType), _str_enum(pTask->DialogType), pTask->pCallBackData, pTask->TaskId
        );
}

Task::Task(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData, size_t TaskId)
        : TaskType(TaskType)
        , DialogType(DialogType)
        , pCallBackData(pCallBackData)
        , TaskId(TaskId)
        , b_IsWaiting(false)
        , v_oSemaphoreTaskWait(0, 1)
{
    //_LOG_DBG("New '%s'", _str_task(this));
}

TaskManagerImpl::TaskManagerImpl()
        : v_oMutex(wxMUTEX_RECURSIVE)
        , v_nNextUniqueId(0)
{
    _LOG_DBG();
}

Task * TaskManagerImpl::CreateTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData )
{
    //_LOG_DBG("LOCK CreateTask()");
    wxMutexLocker lock(v_oMutex);
    //_LOG_DBG("INTO CreateTask()");
    v_nNextUniqueId++;
    Task *pTask = new Task(TaskType, DialogType, pCallBackData, v_nNextUniqueId);
    _LOG_DBG("DONE CreateTask(%s), size=%d", _str_task(pTask), v_apTask.GetCount());
    return pTask;
}

void TaskManagerImpl::AddTask(Task *pTask)
{
    _LOG_DBG("AddTask(%s), size=%d", _str_task(pTask), v_apTask.GetCount());
    _COND_RET(pTask);
    //_LOG_DBG("LOCK AddTask()");
    wxMutexLocker lock(v_oMutex);
    //_LOG_DBG("INTO AddTask()");
    v_apTask.Add(pTask);
    _LOG_DBG("DONE AddTask(%s), size=%d", _str_task(pTask), v_apTask.GetCount());
}

void TaskManagerImpl::AddTaskAndWait(Task *pTask)
{
    _LOG_DBG("AddTaskAndWait(%s), size=%d", _str_task(pTask), v_apTask.GetCount());
    _COND_RET(pTask);
    {
        //_LOG_DBG("LOCK AddTaskAndWait()");
        wxMutexLocker lock(v_oMutex);
        //_LOG_DBG("INTO AddTaskAndWait()");
        v_apTask.Add(pTask);
        pTask->b_IsWaiting = true;
    }
    _LOG_DBG("WAIT AddTaskAndWait(%s), size=%d", _str_task(pTask), v_apTask.GetCount());
    pTask->v_oSemaphoreTaskWait.Wait();
    _LOG_DBG("DONE AddTaskAndWait(%s), size=%d", _str_task(pTask), v_apTask.GetCount());
}

Task *TaskManagerImpl::PopTask()
{
    //_LOG_DBG();
    //_LOG_DBG("LOCK PopTask()");
    wxMutexLocker lock(v_oMutex);
    //_LOG_DBG("INTO PopTask()");
    if (v_apTask.IsEmpty())
        return NULL;
    _LOG_DBG("NOT EMPTY PopTask()");
    Task *pTask = v_apTask[0];
    v_apTask.RemoveAt(0);
    _LOG_DBG("DONE PopTask(%s), size=%d", _str_task(pTask), v_apTask.GetCount());
    return pTask;
}

void TaskManagerImpl::TaskProcessed(size_t nTaskId)
{
    _LOG_DBG("TaskProcessed(%d)", nTaskId);
    _LOG_DBG("LOCK TaskProcessed()");
    wxMutexLocker lock(v_oMutex);
    _LOG_DBG("INTO TaskProcessed()");
    size_t n = v_apTask.GetCount();
    for (size_t i=0; i<n; i++)
    {
        Task *pTask = v_apTask[i];
        if (pTask->TaskId == nTaskId)
        {
            _LOG_DBG("ITEM TaskProcessed() : i=%d", i);
            _COND(i == 0);
            if (pTask->b_IsWaiting)
            {
                _LOG_DBG("WAIT TaskProcessed() : i=%d", i);
                pTask->v_oSemaphoreTaskWait.Post();
                _LOG_DBG("CONT TaskProcessed() : i=%d", i);
            }
            v_apTask.RemoveAt(i);
            _LOG_DBG("DONE TaskProcessed(%s), size=%d", _str_task(pTask), v_apTask.GetCount());
        }
    }
    //_LOG_ERR("No task found with id=%d, size=%d", nTaskId, v_apTask.GetCount());
}

TaskManagerImpl TaskManager::_g_TaskManager;

#endif // USE_DEBUG_CODE
