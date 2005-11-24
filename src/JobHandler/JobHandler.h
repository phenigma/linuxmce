#ifndef JobHandler_h
#define JobHandler_h

#include <string>
using namespace std;
#include "PlutoUtils/MultiThreadIncludes.h"

namespace nsJobHandler
{
	class JobHandler
	{
	    pluto_pthread_mutex_t m_JobHandlerMutex; 

		list<class Job *> m_listJob;
	public:

		JobHandler();
		bool Cancel();
		void AddJob(Job *pJob);
		void RemoveJob(Job *);
		void RemoveJob(int Job);
		string ToString();
		bool ContainsJob(string sName);
	};
};

#endif
