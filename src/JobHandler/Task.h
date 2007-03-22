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
		friend class Job;
		TaskStatus m_eTaskStatus;

	protected:
		string m_sName;
		int m_iID;
		class Job *m_pJob;
		time_t m_tStarted;
		int m_iPercent;

	public:
		string m_sResult;

		Task(Job *pJob,string sName);
		~Task() {}

		virtual bool Abort();

		int m_iID_get() { return m_iID; }
		virtual string ToString() { return m_sName; }
		virtual string Type() { return "Unknown Task"; }  // Should override
		virtual int Run()=0;  // Return 0 if the task is done, or a number of milliseconds if you want Run to be called again in that many ms
		TaskStatus m_eTaskStatus_get() { return m_eTaskStatus; }
		void m_eTaskStatus_set(TaskStatus taskStatus) 
		{
			if( m_eTaskStatus==TASK_NOT_STARTED && taskStatus==TASK_IN_PROGRESS )
				m_tStarted=time(NULL);

			m_eTaskStatus=taskStatus;
		}
		virtual int PercentComplete() { return m_iPercent; }
		virtual int SecondsRemaining()
		{ 
			if( m_iPercent==0 || m_tStarted==0 )
				return 0;

			int Duration = (int) (time(NULL) - m_tStarted);
			int TotalTime = Duration * 100 / m_iPercent;
			return TotalTime - Duration;
		}
	};
};

#endif
