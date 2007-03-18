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
		class Job *m_pJob;

	public:
		string m_sResult;

		Task(Job *pJob,string sName);

		bool Abort();

		virtual string ToString() { return m_sName; }
		virtual string Type() { return "Unknown Task"; }  // Should override
		virtual int Run()=0;  // Return 0 if the task is done, or a number of milliseconds if you want Run to be called again in that many ms
		TaskStatus m_eTaskStatus_get() { return m_eTaskStatus; }
		void m_eTaskStatus_set(TaskStatus taskStatus) 
		{ 
			m_eTaskStatus=taskStatus;
		}
	};
};

#endif
