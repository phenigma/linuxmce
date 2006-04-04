//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_DEBUGGING_H_
#define _WX_DEBUGGING_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_debugging.h"
#endif

void _debug_init();
void * _debug_RoomWizard();
void _debug_refresh_update();

void _debug_thread_block();
void _debug_thread_nonblock();

// compatibility code for debug
// wx implementation for task-manager
#ifdef USE_DEBUG_CODE
#include "wx_dialog_types.h"

enum CallBackType
{
    cbOnWxWidget_Unused = 0,
    cbOnWxWidgetCreate,   // create a unique dialog
    cbOnWxWidgetDelete,   // close dialog without saving data
    cbOnWxWidgetRefresh,  // refresh dialog with new data
    cbOnWxWidgetSave,     // save data from dialog
    cbOnWxWidgetWaitUser, // wait for an user action
};

const char * _str_enum(CallBackType value);

typedef void CallBackData;

class WMTask
{
public:
	WMTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData, size_t TaskId);

    CallBackType TaskType;
	E_DIALOG_TYPE DialogType;
	CallBackData* pCallBackData;
    size_t TaskId;

protected:
    bool b_IsWaiting;
    wxSemaphore v_oSemaphoreTaskWait;
    friend class WMTaskManager;
};
WX_DEFINE_ARRAY_PTR(WMTask *, wxArray_p_WMTask);

const char * _str_task(WMTask *pWMTask);

class WMTaskManager
{
public:
    WMTaskManager();
    WMTask * CreateTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData );
	void AddTask(WMTask *pWMTask);
	void AddTaskAndWait(WMTask *pWMTask);
	WMTask *PopTask();
    void TaskProcessed(size_t nTaskId);
protected:
    wxMutex v_oMutex;
    size_t v_nNextUniqueId;
    wxArray_p_WMTask v_apWMTask;
};

class TaskManager
{
public:
    static WMTaskManager & Instance()
        {
            return _g_WMTaskManager;
        }
protected:
    static WMTaskManager _g_WMTaskManager;
};

#endif // USE_DEBUG_CODE

#endif
// _WX_DEBUGGING_H_
