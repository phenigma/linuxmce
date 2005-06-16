/* 
	MultiThreadIncludes
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"	 
#include "DCE/Logger.h" 
#include "PlutoUtils/MultiThreadIncludes.h" 
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#ifdef WINCE
	#include _STLP_NATIVE_C_HEADER(time.h)
	#include "wince.h"
#endif

// Make this a pointer, rather than an instance.  When it's an instance, sometimes when the app
// exits it destroys it before all the threads using it have died
map<int,PlutoLock *> *g_pmapLocks=NULL;
int iNextLock=1;  // A counter to keep track of locks

pluto_pthread_mutex_t *g_mapLockMutex=NULL;

using namespace DCE;

// An application can create another handler that gets called instead in the event of a deadlock
void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock)=NULL;

// This may get called before the logger is initialized.  If so, we don't want to log until if( g_pPlutoLogger ) is true

PlutoLock::PlutoLock(pluto_pthread_mutex_t *pLock)
{
	if( !g_pmapLocks )
		g_pmapLocks = new map<int,PlutoLock *>;
	m_bLogErrorsOnly=true;
	m_bReleased=false;
	m_bIgnoreDeadlock=false;
	m_pMyLock=pLock;
	m_sFileName = "*NONE*";
	m_Line = 9999999;
	m_LockNum = 0;
	m_tTime = 0;
	m_thread = pthread_self();
	pthread_mutex_lock(&m_pMyLock->mutex);
	m_pMyLock->m_NumLocks++;
}

PlutoLock::PlutoLock(pluto_pthread_mutex_t *pLock,string File,int Line,bool bLogErrorsOnly,string Message)
{
	if( !g_pmapLocks )
		g_pmapLocks = new map<int,PlutoLock *>;

	m_sMessage=Message;
	m_pMyLock=pLock;
	m_bIgnoreDeadlock=false;
	m_bLogErrorsOnly=bLogErrorsOnly;
	m_sFileName=File;
	m_Line=Line;
	m_LockNum = ++iNextLock;
	m_tTime = time(NULL);
	m_bGotLock = false;
	m_thread = pthread_self();

	if( g_mapLockMutex==NULL )
	{
		g_mapLockMutex = new pluto_pthread_mutex_t("maplock");
		g_mapLockMutex->Init(NULL);
	}

	pthread_mutex_lock(&g_mapLockMutex->mutex);
	(*g_pmapLocks)[m_LockNum] = this;
	pthread_mutex_unlock(&g_mapLockMutex->mutex);
	DoLock();
	m_pMyLock->m_NumLocks++;
}

PlutoLock::PlutoLock(pluto_pthread_mutex_t *pLock,string File,int Line,bool bLogErrorsOnly)
{
	if( !g_pmapLocks )
		g_pmapLocks = new map<int,PlutoLock *>;

	m_pMyLock=pLock;
	m_bLogErrorsOnly=bLogErrorsOnly;
	m_bIgnoreDeadlock=false;
	m_sFileName=File;
		m_Line=Line;
	m_LockNum = ++iNextLock;
	m_tTime = time(NULL);
	m_bGotLock = false;
	m_thread = pthread_self();

	if( g_mapLockMutex==NULL )
	{
		g_mapLockMutex = new pluto_pthread_mutex_t("maplock");
		g_mapLockMutex->Init(NULL);
	}

	pthread_mutex_lock(&g_mapLockMutex->mutex);
	(*g_pmapLocks)[m_LockNum] = this;
	pthread_mutex_unlock(&g_mapLockMutex->mutex);
	DoLock();
	m_pMyLock->m_NumLocks++;
}

PlutoLock::~PlutoLock()
{
	Release();
	if( m_LockNum )
	{
		pthread_mutex_lock(&g_mapLockMutex->mutex);
		map<int,PlutoLock *>::iterator itMapLock = (*g_pmapLocks).find(m_LockNum);
		if( itMapLock==(*g_pmapLocks).end() )
		{
			pthread_mutex_unlock(&g_mapLockMutex->mutex);
			
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_CRITICAL, "Cannot find self in maplock! (%p) (>%d) %s: %s:%d %s", 
					&m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
			DumpOutstandingLocks();
		}
		else
		{
int size1 = (*g_pmapLocks).size();
			(*g_pmapLocks).erase(itMapLock);	
int size2 = (*g_pmapLocks).size();
            pthread_mutex_unlock(&g_mapLockMutex->mutex);

#ifdef THREAD_LOG
if( g_pPlutoLogger && !m_bLogErrorsOnly )
g_pPlutoLogger->Write(LV_LOCKING, "removed from map (%p) #%d (>%d) %s: %s:%d %s was: %d size, now %d Rel: %s Got: %s", 
&m_pMyLock->mutex, m_pMyLock->m_NumLocks, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),
m_Line,m_sMessage.c_str(),size1,size2,(m_bReleased ? "Y" : "N"),(m_bGotLock ? "Y" : "N"));
#endif

		}
	}
}	

// Confirms there are no outstanding locks, log if there are
void PlutoLock::ConfirmNoLocks(string File,int Line)
{
	map<int,PlutoLock *>::iterator itMapLock;
	pthread_mutex_lock(&g_mapLockMutex->mutex);  // Protect the map
	if( (*g_pmapLocks).size()==0 )
	{
		pthread_mutex_unlock(&g_mapLockMutex->mutex);
		return;
	}

	string sProblems="Check requested in file: " + File + ":" + StringUtils::itos(Line) + "\n";
	int iNumProblems=0;
	for(itMapLock=(*g_pmapLocks).begin();itMapLock!=(*g_pmapLocks).end();++itMapLock)
	{
		PlutoLock *pSafetyLock = (*itMapLock).second;
		if( pSafetyLock->m_bReleased )
			continue;
		++iNumProblems;
		sProblems+=pSafetyLock->m_sFileName + ":" + StringUtils::itos(pSafetyLock->m_Line) + pSafetyLock->m_pMyLock->m_sName + "\n";
	}
	pthread_mutex_unlock(&g_mapLockMutex->mutex);
	if( !iNumProblems )
		return;

/*
#ifdef WIN32
	const char *cfn = sProblems.c_str();
	const TCHAR *tfn = TCHARNEW(cfn);
	::MessageBox(NULL, tfn, _T("LocksFound"), MB_OK);	
	TCHARDELETE(tfn);
#else
*/
	if( g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_CRITICAL,"Locks found: %s",sProblems.c_str());
//#endif
}


void PlutoLock::CheckLocks()
{
	map<int,PlutoLock *>::iterator itMapLock;
	pthread_mutex_lock(&g_mapLockMutex->mutex);  // Protect the map

	PlutoLock *pSafetyLock_Problem=NULL;
	for(itMapLock=(*g_pmapLocks).begin();itMapLock!=(*g_pmapLocks).end();++itMapLock)
	{
		PlutoLock *pSafetyLock = (*itMapLock).second;
		if( time(NULL)-pSafetyLock->m_tTime>DEADLOCK_TIMEOUT_ERROR )
		{
#ifndef WINCE
			cout << "Found a safety lock problem: " << pSafetyLock << endl;
#endif
			pSafetyLock_Problem=pSafetyLock;
			break;
		}
	}
	pthread_mutex_unlock(&g_mapLockMutex->mutex);
	if( pSafetyLock_Problem )
	{
		DumpOutstandingLocks();
#ifndef WINCE
		cout << "Calling deadlock handler 2" << endl;
		cerr << "Calling deadlock handler 2" << endl;
#endif
		if( g_pDeadlockHandler )
			(*g_pDeadlockHandler)(pSafetyLock_Problem);
	}
}


void PlutoLock::DumpOutstandingLocks()
{
#ifdef WINCE
	//this is crashing under ce ?
	g_pPlutoLogger->Write(LV_STATUS,"DumpOutstandingLocks ... exiting");
	return;
#endif

	if( !g_mapLockMutex )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "g_mapLockMutex is NULL!");
		return;
	}

	if( g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_LOCKING,"Dumping locks %p mutex %p\n",&(*g_pmapLocks),&g_mapLockMutex); // This way it'll get in the log without doing any locks

	pthread_mutex_lock(&g_mapLockMutex->mutex);

	//the mutex is locked, it's ok to access the size() method of (*g_pmapLocks).
	if( g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_LOCKING,"Size of locks: %d\n",(*g_pmapLocks).size());

	// We cannot use a logger in here because the logger will need to get at the map mutex if logging is over a socket
	map<int,PlutoLock *>::iterator itMapLock;
	list<string> listMessages;
	list<pthread_t> listKilledPthreads;

	// We can't use the logger from here on because if it's a socket logger it will need the (*g_pmapLocks).  Store
	// all messages in listMessages and log after we remove the mutex lock
	listMessages.push_back(string("Dumping " + StringUtils::itos((int) (*g_pmapLocks).size()) + " locks"));
	for(itMapLock=(*g_pmapLocks).begin();itMapLock!=(*g_pmapLocks).end();)
	{
		PlutoLock *pSafetyLock = (*itMapLock).second;

		// Somehow, for some reason, pSafetyLock->m_pMyLock->m_sName occasionally throws an exception, 
		// like one of the values has been deleted.  But that should be impossible.  This map is protected
		// with a mutex and entries are deleted from the map prior to deleting the objects.  Here's a test
		// AB 4/8/2004 -- the following code is temporary.  remove it when this bug is fixed.
		// Dont' do this in CE
#ifndef UNDER_CE
		pluto_pthread_mutex_t *testLock=NULL;
		string Name="";
		bool bThrewException=false;
		int LockNum=0;
		try
		{
			LockNum=pSafetyLock->m_LockNum;
			testLock=pSafetyLock->m_pMyLock;
			Name=pSafetyLock->m_pMyLock->m_sName;
		}
		catch(...)
		{
			listMessages.push_back(string("caught an exception for exceptions"));
			bThrewException=true;
		}
		listMessages.push_back(string("finished check for exceptions"));

		if( bThrewException )
		{
			printf("^01\t****EXCEPTION with lock***** ptr: %p name: %s locknum: %d\n",testLock,Name.c_str(),LockNum);
			fflush(stdout);

			try
			{
				char Message[1024];
				sprintf(Message,"^01\t (>%d) %s l:%d time: (%d s) thread: %p Rel: %s Got: %s",
						pSafetyLock->m_LockNum,pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,(int) (time(NULL)-pSafetyLock->m_tTime),
						pSafetyLock->m_thread,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));
				listMessages.push_back(Message);

			}
			catch(...)
			{
				printf("^01\tDamn you.  Threw it again!!\n");
				fflush(stdout);
			}
			++itMapLock;
			continue;
		}
		// AB 4/8/2004 - end of temporary code
#endif

		if( pSafetyLock->m_bIgnoreDeadlock )
		{
			++itMapLock;
			continue;
		}
		struct tm *ptm = localtime(&pSafetyLock->m_tTime);
		string sTime = (((int) time(NULL)-pSafetyLock->m_tTime)>=TRYLOCK_TIMEOUT_WARNING && pSafetyLock->m_bGotLock && !pSafetyLock->m_bReleased ? "*****DL******" : "") + 
			StringUtils::itos(ptm->tm_hour==0 ? 12 : (ptm->tm_hour>13 ? ptm->tm_hour-12 : ptm->tm_hour)) + ":" + 
			(ptm->tm_min<10 ? "0" : "") + StringUtils::itos(ptm->tm_min) + (ptm->tm_sec<10 ? ":0" : ":") + 
			StringUtils::itos(ptm->tm_sec) + (ptm->tm_hour>11 ? "p" : "a");

		char Message[400];

		sprintf(Message,"OL: (%p) (>%d) %s %s l:%d time: %s (%d s) thread: %p Rel: %s Got: %s",
			&pSafetyLock->m_pMyLock->mutex,pSafetyLock->m_LockNum,pSafetyLock->m_pMyLock->m_sName.c_str(),pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,sTime.c_str(),(int) (time(NULL)-pSafetyLock->m_tTime),
			pSafetyLock->m_thread,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));

		if( ((int) time(NULL)-pSafetyLock->m_tTime)>=KILL_THREAD_TIMEOUT && pSafetyLock->m_bGotLock && !pSafetyLock->m_bReleased )
		{
			strcat(Message,"**RELEASING**");
			listMessages.push_back(string(Message));

			pSafetyLock->m_bReleased=true;
			pSafetyLock->m_pMyLock->m_NumLocks--;
			pthread_mutex_unlock(&pSafetyLock->m_pMyLock->mutex);
/*

AB 19 Apr 05.  Disable the killing of threads.  Just release the lock as above.
			bool bKilledAlready=false;
			list<pthread_t>::iterator itKilledThreads;
			for(itKilledThreads=listKilledPthreads.begin();itKilledThreads!=listKilledPthreads.end();++itKilledThreads)
			{
				if( (*itKilledThreads) == pSafetyLock->m_thread )
				{
					bKilledAlready=true;
					break;
				}
			}
#ifdef WIN32
			sprintf(Message,"killing thread: %p (>%d), killed already: %s",pSafetyLock->m_thread,pSafetyLock->m_LockNum,bKilledAlready ? "Y" : "N");
#else
			sprintf(Message,"killing thread %ld (>%d), killed already: %s",
				pSafetyLock->m_thread,pSafetyLock->m_LockNum,bKilledAlready ? "Y" : "N");
#endif

			if( !bKilledAlready )
			{
				listKilledPthreads.push_back(pSafetyLock->m_thread);
			}

			(*g_pmapLocks).erase(itMapLock++);
*/
		}
		else
			++itMapLock;
		listMessages.push_back(string(Message));
	}

	// If we killed threads we have to go through the whole map all over again 
	// for each thread since there could have been other entries in the map
	if( listKilledPthreads.size()>0 )
	{

#ifdef UNDER_CE
#ifdef CENET
#ifndef DISABLE_REBOOTS
					KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);
#endif
#endif
#endif
		listMessages.push_back("Killed thread: " + StringUtils::itos((int) listKilledPthreads.size()));
		list<pthread_t>::iterator itKilledThreads;
		for(itKilledThreads=listKilledPthreads.begin();itKilledThreads!=listKilledPthreads.end();++itKilledThreads)
		{
			for(itMapLock=(*g_pmapLocks).begin();itMapLock!=(*g_pmapLocks).end();)
			{
				PlutoLock *pSafetyLock = (*itMapLock).second;
				if( pSafetyLock->m_thread == (*itKilledThreads) )
				{
					listMessages.push_back("already killed (>" + StringUtils::itos(pSafetyLock->m_LockNum) + ")");
					(*g_pmapLocks).erase(itMapLock++);
				}
				else
					++itMapLock;
			}
		}


		for(itKilledThreads=listKilledPthreads.begin();itKilledThreads!=listKilledPthreads.end();++itKilledThreads)
		{
			pthread_t thread = (*itKilledThreads);
#ifndef UNDER_CE
			pthread_cancel(thread);
#endif
		}

	}
	pthread_mutex_unlock(&g_mapLockMutex->mutex);

#ifndef WINCE
	printf("ready to dump locks using logger: %p\n",g_pPlutoLogger);
#endif

	list<string>::iterator itMessages;
	for(itMessages=listMessages.begin();itMessages!=listMessages.end();++itMessages)
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING,"logging message %s\n",(*itMessages).c_str());
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_WARNING,(*itMessages).c_str());
	}
}

void PlutoLock::Relock()
{
	if( !m_bReleased )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "failure relocking(%p): (>%d) %s:%d %s",&m_pMyLock->mutex, m_LockNum, m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 

		return; // We don't have a lock
	}

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING, "relocking (%p) (>%d) %s", &m_pMyLock->mutex,m_LockNum,m_sMessage.c_str());

		DoLock();
	}
	else
	{
		m_bReleased=false;
		pthread_mutex_lock(&m_pMyLock->mutex);
	}
#else
		m_bReleased=false;
		pthread_mutex_lock(&m_pMyLock->mutex);
#endif
	m_pMyLock->m_NumLocks++;
}


int PlutoLock::CondWait()
{
	if( !m_pMyLock->m_pthread_cond_t )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "failure cond wait -- no condition (%p): %s:%d %s",&m_pMyLock->mutex, m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 
		return EINVAL;
	}

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly && g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_LOCKING, "start cond wait (%p) %s", &m_pMyLock->mutex,m_sMessage.c_str());
#endif
	m_bReleased=true;
	int iResult = pthread_cond_wait(m_pMyLock->m_pthread_cond_t,&m_pMyLock->mutex); // This will unlock the mutex and lock it on awakening
	m_bReleased=false;
	m_bGotLock=true;

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly && g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_LOCKING, "end cond wait (%p) %s", &m_pMyLock->mutex,m_sMessage.c_str());
#endif
	return iResult;
}

int PlutoLock::TimedCondWait(timespec &ts)
{
	if( !m_pMyLock->m_pthread_cond_t )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "failure cond timed wait -- no condition (%p): %s:%d %s",&m_pMyLock->mutex, m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 
		return EINVAL;
	}

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly && g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_LOCKING, "start cond timed wait (%p) %s", &m_pMyLock->mutex,m_sMessage.c_str());
#endif
	m_bReleased=true;
	int iResult = pthread_cond_timedwait(m_pMyLock->m_pthread_cond_t,&m_pMyLock->mutex,&ts); // This will unlock the mutex and lock it on awakening
	m_bReleased=false;
	m_bGotLock=true;

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly && g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_LOCKING, "end cond timed wait (%p) %s", &m_pMyLock->mutex,m_sMessage.c_str());
#endif

	return iResult;
}

int PlutoLock::TimedCondWait(int Seconds,int Nanoseconds)
{
	timespec ts;
	ts.tv_sec=Seconds;
	ts.tv_nsec=Nanoseconds;
	return TimedCondWait(ts);
}

void PlutoLock::DoLock()
{
	m_bGotLock=false;
	m_bReleased=false;
	if( !m_pMyLock->m_bInitialized )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "attempting to use un-initialized lock (%p) (>%d) %s: %s:%d %s",
				&m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
		return;  // We just won't get the lock this first time
	}
#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING, "lock(%p) (>%d) %s: %s:%d %s",&m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
	}
#endif
	time_t t = time(NULL);

	// Try lock doesn't work under CE for some reason
#ifdef UNDER_CE
	pthread_mutex_lock(&m_pMyLock->mutex);
	m_bGotLock=true;
#else
	while( t+TRYLOCK_TIMEOUT_WARNING>time(NULL) && !m_bGotLock)
	{
		 int iResult = pthread_mutex_trylock(&m_pMyLock->mutex);
		 if( iResult==0 )
			 m_bGotLock=true;
		 else if( iResult!= EBUSY )
		 {
			 // Some other error condition
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_CRITICAL, "pthread_mutex_trylock returned %d  (%p) (>%d) %s: %s:%d %s", iResult,
					&m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
			Sleep(1000);  // Something went terribly wrong.  Pause a second so we don't log these errors a zillion times in 5 seconds
		 }

		if( m_bGotLock )
			break;
		Sleep(10);
	}
#endif
	if( !m_bGotLock )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "Failed to get lock(%p) %s: %s:%d last used %s:%d thread: %d (>%d) %s",
				&m_pMyLock->mutex, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_pMyLock->m_sFileName.c_str(),m_pMyLock->m_Line,m_pMyLock->m_thread,m_pMyLock->m_LockNum,m_sMessage.c_str()); 
		DumpOutstandingLocks();
#ifndef WINCE
		cout << "Calling deadlock handler 1" << endl;
		cerr << "Calling deadlock handler 1" << endl;
#endif
		if( g_pDeadlockHandler )
			(*g_pDeadlockHandler)(this);
		pthread_mutex_lock(&m_pMyLock->mutex);
	}
	m_pMyLock->m_Line=m_Line;
	m_pMyLock->m_sFileName=m_sFileName;
	m_pMyLock->m_thread=pthread_self();
	m_pMyLock->m_LockNum=m_LockNum;
#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING, "acquired(%p) #%d (>%d) %s %s:%d %s", 
				&m_pMyLock->mutex,m_pMyLock->m_NumLocks,m_LockNum,m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
	}
#endif
}

void PlutoLock::Release()
{
	if( !m_bReleased )
	{
		pthread_mutex_unlock(&m_pMyLock->mutex);
		m_pMyLock->m_NumLocks--;
#ifdef THREAD_LOG
		if( !m_bLogErrorsOnly )
		{
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_LOCKING, "unlock(%p) #%d (>%d) %s: %s:%d %s", &m_pMyLock->mutex, m_pMyLock->m_NumLocks, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
		}
#endif
	}
	m_bReleased=true;
}

#ifdef WIN32
	void filetime_to_timespec(const FILETIME *ft, struct timespec *ts)
		/*
		* -------------------------------------------------------------------
		* converts FILETIME (as set by GetSystemTimeAsFileTime), where the time is
		* expressed in 100 nanoseconds from Jan 1, 1601,
		* into struct timespec
		* where the time is expressed in seconds and nanoseconds from Jan 1, 1970.
		* -------------------------------------------------------------------
		*/
	{
		ts->tv_sec = (int)((*(LONGLONG *)ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET) / 10000000);
		ts->tv_nsec = (int)((*(LONGLONG *)ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET - ((LONGLONG)ts->tv_sec * (LONGLONG)10000000)) * 100);
	}

	int gettimeofday(struct timeval *tv, struct timezone *tz)
	 {
		FILETIME ft;
		SYSTEMTIME st;
        /* Get the current system time */
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);
		timespec ts;
		filetime_to_timespec(&ft,&ts);
		timespec_to_timeval(&ts,tv);
		return 0;
	 }
#endif

int gettimeofday(struct timespec *ts, struct timezone *tz)
{
	timeval tv_current;
	int i=gettimeofday(&tv_current,NULL);
	timeval_to_timespec(&tv_current,ts);
	return i;
}

// timespec operators
bool operator < (const timespec & t1, const timespec & t2)
{
	if (t1.tv_sec != t2.tv_sec)
		return t1.tv_sec < t2.tv_sec;
	return t1.tv_nsec < t2.tv_nsec;
}

bool operator == (const timespec & t1, const timespec & t2)
{
	return t1.tv_sec == t2.tv_sec && t1.tv_nsec == t2.tv_nsec;
}

bool operator <= (const timespec & t1, const timespec & t2)
{
	return t1 == t2 || t1 < t2;
}

bool operator > (const timespec & t1, const timespec & t2)
{
	return ! (t1 <= t2);
}

bool operator >= (const timespec & t1, const timespec & t2)
{
	return t1 == t2 || t1 > t2;
}

bool operator != (const timespec & t1, const timespec & t2)
{
	return ! (t1 == t2);
}

timespec & operator += (timespec & t1, const timespec & t2)
{
	t1.tv_sec += t2.tv_sec;
	t1.tv_nsec += t2.tv_nsec;

	t1.tv_sec += t1.tv_nsec / 1000000000;
	t1.tv_nsec = t1.tv_nsec % 1000000000;

	return t1;
}

timespec & operator -= (timespec & t1, const timespec & t2)
{
	t1.tv_sec -= t2.tv_sec;
	t1.tv_nsec -= t2.tv_nsec;

	if( t2.tv_nsec<0 )
	{
		t1.tv_sec--;
		t1.tv_nsec += 1000000000;
	}

	return t1;
}

timespec operator + (const timespec & t1, const timespec & t2)
{
	timespec result = { 0, 0 };

	result += t1;
	result += t2;

	return result;
}

timespec ms_to_timespec(unsigned long ts)
{
	timespec result;

	result.tv_sec = ts / 1000;
	result.tv_nsec = ts % 1000 * 1000000;

	return result;
}

timespec & operator += (timespec & t1, long milliseconds)
{
	timespec t2 = { milliseconds / 1000, (milliseconds % 1000) * 1000000 };
	return t1 += t2;
}

timespec & operator -= (timespec & t1, long milliseconds)
{
	timespec t2 = { milliseconds / 1000, (milliseconds % 1000) * 1000000 };
	return t1 -= t2;
}

timespec operator - (const timespec & t1, const timespec & t2)
{
	timespec result = { 0, 0 };

	result += t1;
	result -= t2;

	return result;
}

// timeval operators
bool operator < (const timeval & t1, const timeval & t2)
{
	if (t1.tv_sec != t2.tv_sec)
		return t1.tv_sec < t2.tv_sec;
	return t1.tv_usec < t2.tv_usec;
}

bool operator == (const timeval & t1, const timeval & t2)
{
	return t1.tv_sec == t2.tv_sec && t1.tv_usec == t2.tv_usec;
}

bool operator <= (const timeval & t1, const timeval & t2)
{
	return t1 == t2 || t1 < t2;
}

bool operator > (const timeval & t1, const timeval & t2)
{
	return ! (t1 <= t2);
}

bool operator >= (const timeval & t1, const timeval & t2)
{
	return t1 == t2 || t1 > t2;
}

bool operator != (const timeval & t1, const timeval & t2)
{
	return ! (t1 == t2);
}

timeval & operator += (timeval & t1, const timeval & t2)
{
	t1.tv_sec += t2.tv_sec;
	t1.tv_usec += t2.tv_usec;

	t1.tv_sec += t1.tv_usec / 1000000;
	t1.tv_usec = t1.tv_usec % 1000000;

	return t1;
}

timeval & operator -= (timeval & t1, const timeval & t2)
{
	t1.tv_sec -= t2.tv_sec;
	t1.tv_usec -= t2.tv_usec;

	if( t2.tv_usec<0 )
	{
		t1.tv_sec--;
		t1.tv_usec += 1000000;
	}

	return t1;
}

timeval operator + (const timeval & t1, const timeval & t2)
{
	timeval result = { 0, 0 };

	result += t1;
	result += t2;

	return result;
}

timeval ms_to_timeval(unsigned long ts)
{
	timeval result;

	result.tv_sec = ts / 1000;
	result.tv_usec = ts % 1000 * 1000;

	return result;
}

timeval & operator += (timeval & t1, long milliseconds)
{
	timeval t2 = { milliseconds / 1000, (milliseconds % 1000) * 1000 };
	return t1 += t2;
}

timeval & operator -= (timeval & t1, long milliseconds)
{
	timeval t2 = { milliseconds / 1000, (milliseconds % 1000) * 1000 };
	return t1 -= t2;
}

timeval operator - (const timeval & t1, const timeval & t2)
{
	timeval result = { 0, 0 };

	result += t1;
	result -= t2;

	return result;
}

// timespec <-> timeval
void timespec_to_timeval(const timespec *ts_source,timeval *tv_dest)
{
	tv_dest->tv_sec = ts_source->tv_sec;
	tv_dest->tv_usec = ts_source->tv_nsec / 1000;
}

void timeval_to_timespec(const timeval *tv_source,timespec *ts_dest)
{
	ts_dest->tv_sec = tv_source->tv_sec;
	ts_dest->tv_nsec = tv_source->tv_usec * 1000;
}
