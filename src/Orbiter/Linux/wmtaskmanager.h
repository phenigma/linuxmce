#ifndef WM_TASK_MANAGER_H
#define WM_TASK_MANAGER_H

// Window Message Task Manager

#include "wmtask.h"

#include <queue>
#include <list>

#include "../PlutoUtils/MultiThreadIncludes.h"
#include "../PlutoUtils/singleton.h"
#include "../CallBackTypes.h"
#include "wmtask.h"
using namespace cpp;
using namespace cpp::Threading;

/**
 *	Safe-thread class to treat VX queue of messages
 */
class WMTaskManager
{
	pluto_pthread_mutex_t* ListMutex;
	pthread_cond_t ListMutexCond;
	std::queue<WMTask *> Events;
    std::list<unsigned int> listTasksWithConfirmation;

    unsigned int TaskCounter;

	/**
     *	Wake up the thread which was blocked by AddTaskAndWait()
     */
	void WakeUp();
public:
	/**
	 *	Default constructor
	 */
	WMTaskManager();
	/**
	 *	Default destructor
	 */
	virtual ~WMTaskManager(void);

    WMTask * CreateTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData );


	/**
	 *	Add one task (event) to the current queue of messages
	 */
	void AddTask(WMTask *Event);

	/**
	 *	Add one task (event) to the current queue of messages
	 *  and wait to wake up (by calling method WakeUp()
	 */
	void AddTaskAndWait(WMTask *Event);


	/**
	 *	If the queue with tasks is empty, returns NULL
	 *	Else, returns a task and removes it from the queue
	 *	It's users responsability to deallocate the memory from that task
	 */
	WMTask *PopTask();

    /**
     *  ?
     */
    void TaskProcessed(unsigned int TaskId);
};

typedef Singleton<WMTaskManager, CreateUsingNew<WMTaskManager>, DefaultLifetime, MultiThreaded> TaskManager;

#endif
// WM_TASK_MANAGER_H
