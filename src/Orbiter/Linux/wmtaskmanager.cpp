#include "../wxAppMain/wx_all_include.cpp"
#include "wmtaskmanager.h"
#include "../../DCE/Logger.h"

using namespace DCE;

WMTaskManager::WMTaskManager()
{
    TaskCounter = 0;
	ListMutex = new pluto_pthread_mutex_t("WMTaskListManager Safety Mutex");
	ListMutex->Init(NULL, &ListMutexCond);
}

WMTaskManager::~WMTaskManager()
{
	delete ListMutex;
}

WMTask *WMTaskManager::CreateTask(CallBackType TaskType,
                                  E_DIALOG_TYPE DialogType,
                                  CallBackData* pCallBackData )
{
    PLUTO_SAFETY_LOCK(cm, *ListMutex);
    return new WMTask(TaskType, DialogType, pCallBackData, ++TaskCounter);
}

void WMTaskManager::AddTask(WMTask *pEvent)
{
    if(NULL == pEvent)
        return;

	PLUTO_SAFETY_LOCK(cm, *ListMutex);
    g_pPlutoLogger->Write(
        LV_WARNING,
        "WMTaskManager::AddTask(pEvent %p, action %d, dialog %d, data %p), size=%d",
        pEvent, pEvent->TaskType, pEvent->DialogType, pEvent->pCallBackData, Events.size()
        );
	Events.push(pEvent);
    ::wxWakeUpIdle();
}

void WMTaskManager::AddTaskAndWait(WMTask *pEvent)
{
    if(NULL == pEvent)
        return;

	PLUTO_SAFETY_LOCK(cm, *ListMutex);
    g_pPlutoLogger->Write(
        LV_WARNING,
        "WMTaskManager::AddTaskAndWait(pEvent %p, action %d, dialog %d, data %p), size=%d",
        pEvent, pEvent->TaskType, pEvent->DialogType, pEvent->pCallBackData, Events.size()
        );
	Events.push(pEvent);
    ::wxWakeUpIdle();
    listTasksWithConfirmation.push_back(pEvent->TaskId);

    unsigned int TaskId = pEvent->TaskId;

    while(true)
    {
        cm.CondWait();

        bool bOurTaskNotProcessed = false;
        for(std::list<unsigned int>::iterator it = listTasksWithConfirmation.begin();
            it != listTasksWithConfirmation.end(); ++it)
        {
            if(*it == TaskId)
                bOurTaskNotProcessed = true;
        }

        if(!bOurTaskNotProcessed)
            return;
    }
}

WMTask * WMTaskManager::PopTask()
{
	PLUTO_SAFETY_LOCK(cm, *ListMutex);
    //g_pPlutoLogger->Write(LV_WARNING, "WMTaskManager::PopTask, size=%d", Events.size());
	if(Events.size())
	{
        //g_pPlutoLogger->Write(LV_WARNING, "WMTaskManager::PopTask->Not Empty");
		WMTask *pEvent = Events.back();
		Events.pop();
        g_pPlutoLogger->Write(LV_WARNING, "WMTaskManager::PopTask(pEvent %p), size=%d", pEvent, Events.size());
        return pEvent;
	}

    return NULL;
}

void WMTaskManager::WakeUp()
{
	pthread_cond_broadcast(&ListMutexCond);
}

void WMTaskManager::TaskProcessed(unsigned int TaskId)
{
	PLUTO_SAFETY_LOCK(cm, *ListMutex);
    listTasksWithConfirmation.remove(TaskId);
    //WakeUp();
}
