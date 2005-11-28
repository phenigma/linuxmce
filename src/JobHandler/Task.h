#ifndef Task_h
#define Task_h

#include <string>
using namespace std;
#include "PlutoUtils/MultiThreadIncludes.h"

namespace nsJobHandler
{
	typedef enum {
		TASK_NOT_STARTED = 0,
		TASK_IN_PROGRESS,
		TASK_FAILED,
		TASK_COMPLETED,
		TASK_CANCELED
	} TaskStatus;

	class Task
	{
	protected:
		int m_iPriority;
		string m_sName;
		class Job *m_pJob;
		bool m_bThreadRunning;
		pthread_t m_TaskThread;

	public:
		string m_sResult;

		TaskStatus m_eTaskStatus;
		Task(string sName,int iPriority,Job *pJob);

		//TaskStatus m_eTaskStatus_get() { return m_eTaskStatus; }
		bool m_bThreadRunning_get() { return m_bThreadRunning; }
		void Execute();
		bool Cancel();
		virtual void ThreadStarted() {}
		virtual void ThreadEnded() { m_bThreadRunning=false; }

		virtual string ToString()=0;
		virtual string Type()=0;
		virtual void Run()=0;
	};
};

#endif
