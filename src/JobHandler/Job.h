/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef Job_h
#define Job_h

#include <string>
#include <list>
#include "PlutoUtils/ThreadedClass.h"

using namespace std;
using namespace nsThreadedClass;

namespace DCE
{
	class PendingTaskList;
}
using namespace DCE;

namespace nsJobHandler
{
	class Task;
	class JobHandler;
	
	class Job : public ThreadedClass
	{
	public:
		enum JobStatus
		{
			job_WaitingToStart,
			job_InProgress,
			job_Error,
			job_Aborted,
			job_Done
		};

	private:
		static int m_NextJobID;
		int m_NextTaskID;

	protected:
		friend class Task;
		friend class JobHandler;

		string m_sName;
		int m_iID;
		int m_iMaxTasks;
		JobStatus m_eJobStatus;
		class JobHandler *m_pJobHandler;
		list<Task *> m_listTask;
		time_t m_tNextRunAttempt;  // Set this to non-zero if you want to your ReadyToRun() to be called no later than this time.  Zero means don't call until something changes
		
	public:
		Job(JobHandler *pJobHandler,string sName);
		virtual ~Job();

		// Override these for better reporting of the Job's progress
		virtual int PercentComplete() { return 0; }
		virtual int SecondsRemaining() { return 0; }
		virtual string ToString() { return m_sName; }

		int m_iID_get() { return m_iID; }
		time_t m_tNextRunAttempt_get() { return m_tNextRunAttempt; }

		int PendingTasks();
		bool Abort();

		void SetMaxTasks(int MaxTasks) { m_iMaxTasks=MaxTasks; }
		virtual bool StartThread();
		virtual bool StopThread(int iTimeout);
		virtual bool ReadyToRun() { return true; }

		enum JobStatus m_eJobStatus_get() { return m_eJobStatus; }

		virtual bool CanHandleAnotherTask() { return m_iMaxTasks==0 || PendingTasks()<m_iMaxTasks; }

		virtual string GetType()=0;

		Task *GetNextTask();
		Task *FindTask(int taskID);
		void AddTask(Task *pTask);

		string m_sName_get() { return m_sName; }
		void Reset(bool bDelete = true);

		virtual void Run();

		virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList);  // override to accurately report this job
	};
};

#endif
