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

map<int,PlutoLock *> mapLocks;
int iNextLock=1;  // A counter to keep track of locks

pluto_pthread_mutex_t *m_mapLockMutex=NULL;

using namespace DCE;

// This may get called before the logger is initialized.  If so, we don't want to log until if( g_pPlutoLogger ) is true

PlutoLock::PlutoLock(pluto_pthread_mutex_t *pLock)
{
	m_bLogErrorsOnly=true;
	m_bReleased=false;
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
	m_sMessage=Message;
	m_pMyLock=pLock;
	m_bLogErrorsOnly=bLogErrorsOnly;
	m_sFileName=File;
	m_Line=Line;
	m_LockNum = ++iNextLock;
	m_tTime = time(NULL);
	m_bGotLock = false;
	m_thread = pthread_self();

	if( m_mapLockMutex==NULL )
	{
		m_mapLockMutex = new pluto_pthread_mutex_t("maplock");
		m_mapLockMutex->Init(NULL);
	}

	pthread_mutex_lock(&m_mapLockMutex->mutex);
	mapLocks[m_LockNum] = this;
	pthread_mutex_unlock(&m_mapLockMutex->mutex);
	DoLock();
	m_pMyLock->m_NumLocks++;
}

PlutoLock::PlutoLock(pluto_pthread_mutex_t *pLock,string File,int Line,bool bLogErrorsOnly)
{
	m_pMyLock=pLock;
	m_bLogErrorsOnly=bLogErrorsOnly;
	m_sFileName=File;
		m_Line=Line;
	m_LockNum = ++iNextLock;
	m_tTime = time(NULL);
	m_bGotLock = false;
	m_thread = pthread_self();

	if( m_mapLockMutex==NULL )
	{
		m_mapLockMutex = new pluto_pthread_mutex_t("maplock");
		m_mapLockMutex->Init(NULL);
	}

	pthread_mutex_lock(&m_mapLockMutex->mutex);
	mapLocks[m_LockNum] = this;
	pthread_mutex_unlock(&m_mapLockMutex->mutex);
	DoLock();
	m_pMyLock->m_NumLocks++;
}

PlutoLock::~PlutoLock()
{
	Release();
	if( m_LockNum )
	{
		pthread_mutex_lock(&m_mapLockMutex->mutex);
		map<int,PlutoLock *>::iterator itMapLock = mapLocks.find(m_LockNum);
		if( itMapLock==mapLocks.end() )
		{
			pthread_mutex_unlock(&m_mapLockMutex->mutex);
			
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find self in maplock!");
			DumpOutstandingLocks();
		}
		else
		{
			mapLocks.erase(itMapLock);	
			pthread_mutex_unlock(&m_mapLockMutex->mutex);
		}
	}
}	

// Confirms there are no outstanding locks, log if there are
void PlutoLock::ConfirmNoLocks(string File,int Line)
{
	map<int,PlutoLock *>::iterator itMapLock;
	pthread_mutex_lock(&m_mapLockMutex->mutex);  // Protect the map
	if( mapLocks.size()==0 )
	{
		pthread_mutex_unlock(&m_mapLockMutex->mutex);
		return;
	}

	string sProblems="Check requested in file: " + File + ":" + StringUtils::itos(Line) + "\n";
	int iNumProblems=0;
	for(itMapLock=mapLocks.begin();itMapLock!=mapLocks.end();++itMapLock)
	{
		PlutoLock *pSafetyLock = (*itMapLock).second;
		if( pSafetyLock->m_bReleased )
			continue;
		++iNumProblems;
		sProblems+=pSafetyLock->m_sFileName + ":" + StringUtils::itos(pSafetyLock->m_Line) + pSafetyLock->m_pMyLock->m_sName + "\n";
	}
	pthread_mutex_unlock(&m_mapLockMutex->mutex);
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
	pthread_mutex_lock(&m_mapLockMutex->mutex);  // Protect the map

	bool bFoundProblem=false;  
	for(itMapLock=mapLocks.begin();itMapLock!=mapLocks.end();++itMapLock)
	{
		PlutoLock *pSafetyLock = (*itMapLock).second;
		if( time(NULL)-pSafetyLock->m_tTime>15 )
		{
			bFoundProblem=true;
			break;
		}
	}
	pthread_mutex_unlock(&m_mapLockMutex->mutex);
	if( bFoundProblem )
		DumpOutstandingLocks();
}


void PlutoLock::DumpOutstandingLocks()
{
	if( g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_LOCKING,"Dumping locks %p mutex %p\n",&mapLocks,&m_mapLockMutex); // This way it'll get in the log without doing any locks
	if( g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_LOCKING,"Size of locks: %d\n",mapLocks.size());

	if( !m_mapLockMutex )
		return;
	// We cannot use a logger in here because the logger will need to get at the map mutex if logging is over a socket
	map<int,PlutoLock *>::iterator itMapLock;
	list<string> listMessages;
	list<pthread_t> listKilledPthreads;
	pthread_mutex_lock(&m_mapLockMutex->mutex);

	if( g_pPlutoLogger )
		g_pPlutoLogger->Write(LV_LOCKING,"Dumping %d locks\n",mapLocks.size()); // This way it'll get in the log without doing any locks

	listMessages.push_back(string("Dumping " + StringUtils::itos((int) mapLocks.size()) + " locks"));
	for(itMapLock=mapLocks.begin();itMapLock!=mapLocks.end();)
	{
		PlutoLock *pSafetyLock = (*itMapLock).second;
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING,"pSafetyLock: %p check for exceptions\n",pSafetyLock);

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
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_LOCKING,"pSafetyLock: %p caught an exception for exceptions\n",pSafetyLock);
			bThrewException=true;
		}
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING,"pSafetyLock: %p finished check for exceptions: %d\n",pSafetyLock,(int) bThrewException);

		if( bThrewException )
		{
			printf("^01\t****EXCEPTION with lock***** ptr: %p name: %s locknum: %d\n",testLock,Name.c_str(),LockNum);
			fflush(stdout);

			try
			{
#ifndef WIN32
				if( g_pPlutoLogger )
					g_pPlutoLogger->Write(LV_LOCKING,"^01\t (>%d) %s l:%d time: (%d s) thread: %ld Rel: %s Got: %s",
						pSafetyLock->m_LockNum,pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,(int) (time(NULL)-pSafetyLock->m_tTime),
						pSafetyLock->m_thread,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));
#else
				if( g_pPlutoLogger )
					g_pPlutoLogger->Write(LV_LOCKING,"^01\t (>%d) %s l:%d time: (%d s) thread: %p Rel: %s Got: %s",
						pSafetyLock->m_LockNum,pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,(int) (time(NULL)-pSafetyLock->m_tTime),
						pSafetyLock->m_thread,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));
#endif
			}
			catch(...)
			{
				printf("^01\tDamn you.  Threw it again!!\n");
				fflush(stdout);
			}
			continue;
		}
		// AB 4/8/2004 - end of temporary code
#endif

		if( pSafetyLock->m_pMyLock->m_sName=="alarm" )
		{
			++itMapLock;
			continue;
		}
		struct tm *ptm = localtime(&pSafetyLock->m_tTime);
		string sTime = (((int) time(NULL)-pSafetyLock->m_tTime)>=5 && pSafetyLock->m_bGotLock && !pSafetyLock->m_bReleased ? "*****DL******" : "") + 
			StringUtils::itos(ptm->tm_hour==0 ? 12 : (ptm->tm_hour>13 ? ptm->tm_hour-12 : ptm->tm_hour)) + ":" + 
			(ptm->tm_min<10 ? "0" : "") + StringUtils::itos(ptm->tm_min) + (ptm->tm_sec<10 ? ":0" : ":") + 
			StringUtils::itos(ptm->tm_sec) + (ptm->tm_hour>11 ? "p" : "a");

		char Message[400];

#ifdef WIN32
		sprintf(Message,"OL: (%p) (>%d) %s %s l:%d time: %s (%d s) thread: %p Rel: %s Got: %s",
			pSafetyLock->m_pMyLock->mutex,pSafetyLock->m_LockNum,pSafetyLock->m_pMyLock->m_sName.c_str(),pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,sTime.c_str(),((int) time(NULL)-pSafetyLock->m_tTime),
			pSafetyLock->m_thread,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));
#else
		sprintf(Message,"OL: (%p) (>%d) %s %s l:%d time: %s (%d s) thread: %ld Rel: %s Got: %s",
			&pSafetyLock->m_pMyLock->mutex,pSafetyLock->m_LockNum,pSafetyLock->m_pMyLock->m_sName.c_str(),pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,sTime.c_str(),(int) (time(NULL)-pSafetyLock->m_tTime),
			pSafetyLock->m_thread,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));
#endif

		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING,"^01\t%s\n",Message);
		if( ((int) time(NULL)-pSafetyLock->m_tTime)>=60 && pSafetyLock->m_bGotLock && !pSafetyLock->m_bReleased )
		{
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_CRITICAL,"releasing %s\n",Message);
			strcat(Message,"**RELEASING**");
			listMessages.push_back(string(Message));

			pthread_mutex_unlock(&pSafetyLock->m_pMyLock->mutex);
			pSafetyLock->m_pMyLock->m_NumLocks--;
			pSafetyLock->m_bReleased=true;

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
#ifdef UNDER_CE
#ifdef CENET
				// Under Windows CE, just reboot
				if( g_pPlutoLogger )
					g_pPlutoLogger->Write(LV_LOCKING,"going to reboot");
#endif
#endif
			}

			mapLocks.erase(itMapLock++);
		}
		else
			++itMapLock;
		listMessages.push_back(string(Message));
#ifndef WIN32
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"--%s\n",Message);
#endif
	}

	// If we killed threads we have to go through the whole map all over again 
	// for each thread since there could have been other entries in the map
	if( listKilledPthreads.size()>0 )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING,"killed %d threads",(int) listKilledPthreads.size());

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
			for(itMapLock=mapLocks.begin();itMapLock!=mapLocks.end();)
			{
				PlutoLock *pSafetyLock = (*itMapLock).second;
				if( pSafetyLock->m_thread == (*itKilledThreads) )
				{
					if( g_pPlutoLogger )
						g_pPlutoLogger->Write(LV_LOCKING,"already killed (>%d) threads",pSafetyLock->m_LockNum);
					listMessages.push_back("already killed (>" + StringUtils::itos(pSafetyLock->m_LockNum) + ")");
					mapLocks.erase(itMapLock++);
				}
				else
					++itMapLock;
			}
		}


		for(itKilledThreads=listKilledPthreads.begin();itKilledThreads!=listKilledPthreads.end();++itKilledThreads)
		{
			pthread_t thread = (*itKilledThreads);
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_LOCKING,"doing the kill: %ld",thread);
#ifndef UNDER_CE
			pthread_cancel(thread);
#endif
		}

	}
	pthread_mutex_unlock(&m_mapLockMutex->mutex);

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
#ifdef WIN32
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "failure relocking(%p): %s:%d %s",m_pMyLock->mutex, m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 
#else
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "failure relocking(%p): %s:%d %s",&m_pMyLock->mutex, m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 
#endif
		return; // We don't have a lock
	}

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
	{
	#ifdef WIN32
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING, "relocking (%p) %s", m_pMyLock->mutex,m_sMessage.c_str());
	#else
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING, "relocking (%p) %s", &m_pMyLock->mutex,m_sMessage.c_str());
	#endif
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


void PlutoLock::DoLock()
{
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
	#ifdef WIN32
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING, "lock(%p) (>%d) %s: %s:%d %s",m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 
	#else
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_LOCKING, "lock(%p) (>%d) %s: %s:%d %s",&m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
	#endif
	}
#endif
	time_t t = time(NULL);

	// Try lock doesn't work under CE for some reason
#ifdef UNDER_CE
	pthread_mutex_lock(&m_pMyLock->mutex);
	m_bGotLock=true;
#else
	while( t+5>time(NULL) && !m_bGotLock)
	{
		m_bGotLock = (pthread_mutex_trylock(&m_pMyLock->mutex)!=EBUSY);
		if( m_bGotLock )
			break;
		Sleep(10);
	}
#endif
	if( !m_bGotLock )
	{
		if( g_pPlutoLogger )
#ifdef WIN32
			g_pPlutoLogger->Write(LV_CRITICAL, "Failed to get lock(%p) %s: %s:%d last used %s:%d thread: %p (>%d) %s",
				m_pMyLock->mutex, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_pMyLock->m_sFileName.c_str(),m_pMyLock->m_Line,m_pMyLock->m_thread,m_pMyLock->m_LockNum,m_sMessage.c_str()); 
#else
			g_pPlutoLogger->Write(LV_CRITICAL, "Failed to get lock(%p) %s: %s:%d last used %s:%d thread: %d (>%d) %s",
				&m_pMyLock->mutex, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_pMyLock->m_sFileName.c_str(),m_pMyLock->m_Line,m_pMyLock->m_thread,m_pMyLock->m_LockNum,m_sMessage.c_str()); 
#endif
		DumpOutstandingLocks();
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
	#ifdef WIN32
			g_pPlutoLogger->Write(LV_LOCKING, "acquired(%p) #%d (>%d) %s %s:%d %s", 
				m_pMyLock->mutex,m_pMyLock->m_NumLocks,m_LockNum,m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
	#else
			g_pPlutoLogger->Write(LV_LOCKING, "acquired(%p) #%d (>%d) %s %s:%d %s", 
				&m_pMyLock->mutex,m_pMyLock->m_NumLocks,m_LockNum,m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
	#endif
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
	#ifdef WIN32
				g_pPlutoLogger->Write(LV_LOCKING, "unlock(%p) #%d (>%d) %s: %s:%d %s", m_pMyLock->mutex, m_pMyLock->m_NumLocks, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
	#else
				g_pPlutoLogger->Write(LV_LOCKING, "unlock(%p) #%d (>%d) %s: %s:%d %s", &m_pMyLock->mutex, m_pMyLock->m_NumLocks, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
	#endif
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
	
bool operator < (const timespec & t1, const timespec & t2)
{
	if( t1.tv_sec!=t2.tv_sec )
		return t1.tv_sec<t2.tv_sec;
	return t1.tv_nsec<t2.tv_nsec;
}

bool operator <= (const timespec & t1, const timespec & t2)
{
	if( t1.tv_sec!=t2.tv_sec )
		return t1.tv_sec<t2.tv_sec;
	return t1.tv_nsec<=t2.tv_nsec;
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

void timespec_to_timeval(timespec *ts_source,timeval *tv_dest)
{
	tv_dest->tv_sec = ts_source->tv_sec;
	tv_dest->tv_usec = ts_source->tv_nsec / 1000;
}

void timeval_to_timespec(timeval *tv_source,timespec *ts_dest)
{
	ts_dest->tv_sec = tv_source->tv_sec;
	ts_dest->tv_nsec = tv_source->tv_usec * 1000;
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

timespec operator - (const timespec & t1, const timespec & t2)
{
	timespec result = { 0, 0 };

	result += t1;
	result -= t2;

	return result;
}

