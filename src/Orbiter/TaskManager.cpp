// Window Message Task Manager

#include "TaskManager.h"
#include "../../DCE/Logger.h"
#include "CallBackData.h"
using namespace DCE;
//--------------------------------------------------------------------------------------------------------------
TaskManagerImpl::TaskManagerImpl()
{
    TaskCounter = 0;
	ListMutex = new pluto_pthread_mutex_t("TaskListManager Safety Mutex");
    pthread_cond_init(&ListMutexCond, NULL);
    ListMutex->Init(NULL, &ListMutexCond);
}
//--------------------------------------------------------------------------------------------------------------
TaskManagerImpl::~TaskManagerImpl()
{
	delete ListMutex;
}
//--------------------------------------------------------------------------------------------------------------
Task *TaskManagerImpl::CreateTask(CallBackType TaskType,
                                  E_DIALOG_TYPE DialogType,
                                  CallBackData* pCallBackData )
{
    PLUTO_SAFETY_LOCK(cm, *ListMutex);
    return new Task(TaskType, DialogType, pCallBackData, ++TaskCounter);
}
//--------------------------------------------------------------------------------------------------------------
void TaskManagerImpl::AddTask(Task *pEvent)
{
    if(NULL == pEvent)
        return;

	PLUTO_SAFETY_LOCK(cm, *ListMutex);
    g_pPlutoLogger->Write(
        LV_WARNING,
        "TaskManagerImpl::AddTask(pEvent %p, id=%d, action %d, dialog %d, data %p), size=%d",
        pEvent, pEvent->TaskId, pEvent->TaskType, pEvent->DialogType, pEvent->pCallBackData, Events.size()
        );
	Events.push(pEvent);
}
//--------------------------------------------------------------------------------------------------------------
void TaskManagerImpl::AddTaskAndWait(Task *pEvent)
{
    if(NULL == pEvent)
        return;

	PLUTO_SAFETY_LOCK(cm, *ListMutex);
    g_pPlutoLogger->Write(
        LV_WARNING,
        "TaskManagerImpl::AddTaskAndWait(pEvent %p, id=%d, action %d, dialog %d, data %p), size=%d",
        pEvent, pEvent->TaskId, pEvent->TaskType, pEvent->DialogType, pEvent->pCallBackData, Events.size()
        );
	Events.push(pEvent);
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
        {
            g_pPlutoLogger->Write(LV_WARNING, "TaskManagerImpl::AddTaskAndWait END, size=%d", Events.size());
            return;
        }
    }
}
//--------------------------------------------------------------------------------------------------------------
Task * TaskManagerImpl::PopTask()
{
	PLUTO_SAFETY_LOCK(cm, *ListMutex);
    //g_pPlutoLogger->Write(LV_WARNING, "TaskManagerImpl::PopTask, size=%d", Events.size());
	if(Events.size())
	{
        //g_pPlutoLogger->Write(LV_WARNING, "TaskManagerImpl::PopTask->Not Empty");
		Task *pEvent = Events.front();
		Events.pop();
        g_pPlutoLogger->Write(LV_WARNING, "TaskManagerImpl::PopTask(pEvent %p), size=%d", pEvent, Events.size());
        return pEvent;
	}

    return NULL;
}
//--------------------------------------------------------------------------------------------------------------
E_DIALOG_TYPE TaskManagerImpl::GetDialogTypeForTopTask()
{
	PLUTO_SAFETY_LOCK(cm, *ListMutex);
	if(Events.size())
	{
		Task *pEvent = Events.front();
		return pEvent->DialogType;
	}

	return E_Dialog_Undefined;
}	
//--------------------------------------------------------------------------------------------------------------
CallBackType TaskManagerImpl::GetTopTaskType()
{
	PLUTO_SAFETY_LOCK(cm, *ListMutex);
	if(Events.size())
	{
		Task *pEvent = Events.front();
		return pEvent->TaskType;
	}

	return cbUnused;
}
//--------------------------------------------------------------------------------------------------------------
unsigned long TaskManagerImpl::GetPopupIDForTopTask()
{
	PLUTO_SAFETY_LOCK(cm, *ListMutex);
	if(Events.size())
	{
		Task *pEvent = Events.front();
		PopupCallBackData *pPopupData = dynamic_cast<PopupCallBackData *>(pEvent->pCallBackData);
		if(NULL != pPopupData)
			return pPopupData->m_ulPopupID;
	}

	return 0;
}
//--------------------------------------------------------------------------------------------------------------
void TaskManagerImpl::WakeUp()
{
	pthread_cond_broadcast(&ListMutexCond);
}
//--------------------------------------------------------------------------------------------------------------
void TaskManagerImpl::TaskProcessed(unsigned int TaskId)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(cm, *ListMutex);
    listTasksWithConfirmation.remove(TaskId);
    g_pPlutoLogger->Write(LV_WARNING, "TaskManagerImpl::TaskProcessed(id %d), size=%d", TaskId, Events.size());
    WakeUp();
}
//--------------------------------------------------------------------------------------------------------------