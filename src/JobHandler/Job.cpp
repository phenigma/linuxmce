#include "Job.h"
#include "Task.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

using namespace nsJobHandler;

int g_ID=0;

Job::Job(string sName) : m_JobMutex("job")
{ 
	m_sName=sName; 
	m_bCancel=0;
	m_iMaxTasks=1;
	m_iID=g_ID++; 

	pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	pthread_cond_init( &m_JobMutexCond, NULL );
	m_JobMutex.Init( &m_MutexAttr, &m_JobMutexCond );
}

Job::~Job()
{
    pthread_mutexattr_destroy(&m_MutexAttr);
}

bool Job::Cancel()
{
	m_bCancel=true;
	PLUTO_SAFETY_LOCK(jm,m_JobMutex);
	bool bSuccess=true;
	for(list<class Task *>::iterator it=m_listTask.begin();it!=m_listTask.end();++it)
	{
		Task *pTask = *it;
		if( !pTask->Cancel() )
			bSuccess=false;
	}
	return bSuccess;
}

Task *Job::GetNextTask()
{
	PLUTO_SAFETY_LOCK(jm,m_JobMutex);
	if( !CanHandleAnotherTask() )
		return NULL;
	for(list<class Task *>::iterator it=m_listTask.begin();it!=m_listTask.end();++it)
	{
		Task *pTask = *it;
		if( pTask->m_eTaskStatus_get()==TASK_NOT_STARTED )
			return pTask;
	}
	return NULL;
}

void Job::AddTask(Task *pTask)
{
	PLUTO_SAFETY_LOCK(jm,m_JobMutex);
	m_listTask.push_back(pTask);
}

int Job::PendingTasks()
{
	int Tasks=0;
	PLUTO_SAFETY_LOCK(jm,m_JobMutex);
	for(list<class Task *>::iterator it=m_listTask.begin();it!=m_listTask.end();++it)
	{
		Task *pTask = *it;
		if( pTask->m_eTaskStatus_get()==TASK_IN_PROGRESS )
			Tasks++;
	}
	return Tasks;
}

