#include "Task.h"
#include "Job.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

using namespace nsJobHandler;
using namespace DCE;

void * StartTaskThread(void * Arg)
{
	Task * pTask = (Task *) Arg;
	pTask->ThreadStarted();
	pTask->Run();
	pTask->ThreadEnded();
	return NULL;
}

Task::Task(string sName,int iPriority,Job *pJob)
{
	m_sName = sName;
	m_iPriority=iPriority;
	m_pJob=pJob;
	m_eTaskStatus=TASK_NOT_STARTED;
	m_bThreadRunning=false;
	m_TaskThread=0;
}

void Task::Execute()
{
	m_bThreadRunning=true;
	m_eTaskStatus = TASK_IN_PROGRESS;
	if (pthread_create(&m_TaskThread, NULL, StartTaskThread, (void *) this))
	{
		m_bThreadRunning=false;
		g_pPlutoLogger->Write(LV_CRITICAL,"Task::Execute - cannot start helper thread");
	}
	else
	{
		pthread_detach(m_TaskThread);
	}
}

bool Task::Cancel()
{
	time_t tTimeout = time(NULL) + 10;
	while(m_bThreadRunning && tTimeout>time(NULL))
	{
		pthread_cond_broadcast(&m_pJob->m_JobMutexCond);
		Sleep(100);
	}
	return m_bThreadRunning==false;
}
