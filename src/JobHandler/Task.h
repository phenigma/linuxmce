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
		TASK_COMPLETED
	} TaskStatus;

	class Task
	{
	private:
		int m_iPriority;
		string m_sName;
		TaskStatus m_eTaskStatus;
		class Job *m_pJob;
		bool m_bThreadRunning;
		pthread_t m_TaskThread;

	public:
		Task(string sName,int iPriority,Job *pJob);

		TaskStatus m_eTaskStatus_get() { return m_eTaskStatus; }
		void Execute();
		bool Cancel();
		void ThreadEnded() { m_bThreadRunning=false; }

		virtual string ToString()=0;
		virtual string Type()=0;
		virtual void Run()=0;
	};
};

#endif
