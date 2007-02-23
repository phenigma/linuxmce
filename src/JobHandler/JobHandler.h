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
