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

/*

A JobHandler lets you add job's and tasks which need to be done asynchronously.
A Job can be something like Rip a Disc, Generate an Orbiter's GUI, etc.
Derive a class from Job for your particular type of job.  The JobHandler
will check your Job to see if it's ready to do something, and if so, make the 
Job start.  The Job will be started in it's own thread.  Several Jobs may 
run in parallel up to the maximum specified.

Within a job are a series of Tasks which much be run in order.  You do not need to
use the task mechanism, and can instead handle the job yourself directly by overriding the
ServiceTasks() function to do whatever you like.

However, you can also create classes drived from Task to do all the tasks that need to be done
in order and add them to Job, and let ServiceTasks run them in order.  Note that the Job's ServiceTasks
function can either block and not return until the Job is complete, or it can set the 
m_tNextServiceTime to the next time that the Job should be serviced.  If that is 0, and the job 
is not complete, it will be serviced whenever JobHandler::SomethingChanged is called.

Both Job::ServiceTasks and Task::Service need to keep checking m_bAbort and if it's true,
return and set the status to ABORTED.

*/

#ifndef JobHandler_h
#define JobHandler_h

#include <string>
using namespace std;
#include "PlutoUtils/ThreadedClass.h"

namespace nsJobHandler
{
	class JobHandler : public nsThreadedClass::ThreadedClass
	{
		list<class Job *> m_listJob;

	public:
		JobHandler();
		virtual ~JobHandler();

		bool AbortAllJobs();
		void PurgeCompletedJobs();
		bool WaitForJobsToFinish(bool bAbort=true,int iSeconds=5);  // By default abort and wait 5 seconds for jobs to finish

		void AddJob(Job *pJob);
		string ToString();
		bool ContainsJob(string sName);
		void StateChanged();

		bool HasJobs();

		void Run();
	};
};

#endif
