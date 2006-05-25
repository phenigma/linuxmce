// Window Message Task Manager

#ifndef WM_TASK_MANAGER_H
#define WM_TASK_MANAGER_H
//--------------------------------------------------------------------------------------------------------------
#include "Task.h"
#include <queue>
#include <list>
//--------------------------------------------------------------------------------------------------------------
#include "../PlutoUtils/MultiThreadIncludes.h"
#include "../PlutoUtils/singleton.h"
#include "CallBackTypes.h"
using namespace cpp;
using namespace cpp::Threading;
//--------------------------------------------------------------------------------------------------------------
/**
 *	Safe-thread class to treat VX queue of messages
 */
class TaskManagerImpl
{
	pluto_pthread_mutex_t* ListMutex;
	pthread_cond_t ListMutexCond;
	std::queue<Task *> Events;
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
	TaskManagerImpl();
	/**
	 *	Default destructor
	 */
	virtual ~TaskManagerImpl(void);

    Task * CreateTask(CallBackType TaskType, E_DIALOG_TYPE DialogType, CallBackData* pCallBackData );


	/**
	 *	Add one task (event) to the current queue of messages
	 */
	void AddTask(Task *Event);

	/**
	 *	Add one task (event) to the current queue of messages
	 *  and wait to wake up (by calling method WakeUp()
	 */
	void AddTaskAndWait(Task *Event);


	/**
	 *	If the queue with tasks is empty, returns NULL
	 *	Else, returns a task and removes it from the queue
	 *	It's users responsability to deallocate the memory from that task
	 */
	Task *PopTask();

	/**
	* 	Returns the type of the dialog for first task from queue.
	*/
	E_DIALOG_TYPE GetDialogTypeForTopTask();

	/**
	* 	Returns the type of the first task from queue.
	*/
	CallBackType GetTopTaskType();

	/**
	* 	Returns the id of the popup of the first task from queue if that task is for a popup; 0 if not
	*/
	unsigned long GetPopupIDForTopTask();

    /**
     *  Notifies TaskManager that a task has been processed.
     */
    void TaskProcessed(unsigned int TaskId);
};
//--------------------------------------------------------------------------------------------------------------
typedef Singleton<TaskManagerImpl, CreateUsingNew<TaskManagerImpl>, DefaultLifetime, MultiThreaded> TaskManager;
//--------------------------------------------------------------------------------------------------------------
#endif
// WM_TASK_MANAGER_H
