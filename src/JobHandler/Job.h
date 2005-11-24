#ifndef Job_h
#define Job_h

#include <string>
#include <list>
#include "PlutoUtils/MultiThreadIncludes.h"
using namespace std;

namespace nsJobHandler
{
	extern int g_ID;

	class Job
	{
		friend class Task;

		list<class Task *> m_listTask;
		string m_sName;
		int m_iID;
		int m_iMaxTasks;
		bool m_bCancel;
	    pthread_mutexattr_t m_MutexAttr;
		pthread_cond_t m_JobMutexCond; 
		
	public:
	    pluto_pthread_mutex_t m_JobMutex; // Other classes may need this

		Job(string sName);
		virtual ~Job();

		virtual int PercentComplete()=0;
		virtual int SecondsRemaining()=0;
		virtual string ToString()=0;

		void SetMaxTasks(int MaxTasks) { m_iMaxTasks=MaxTasks; }
		int m_iID_get() { return m_iID; }
		int PendingTasks();
		bool Cancel();
		virtual bool CanHandleAnotherTask() { return PendingTasks()<m_iMaxTasks; }
		Task *GetNextTask();
		void AddTask(Task *pTask);
		void ServiceTasks();
		string m_sName_get() { return m_sName; }
	};
};

#endif
