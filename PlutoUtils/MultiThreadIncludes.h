#ifndef MultiThreadIncludes_h
#define MultiThreadIncludes_h

#include <pthread.h>
#include "PlutoUtils/CommonIncludes.h"	

#ifdef WIN32
	#define PTW32_TIMESPEC_TO_FILETIME_OFFSET   ( ((LONGLONG) 27111902 << 32) + (LONGLONG) 3577643008 )

	void filetime_to_timespec(const FILETIME *ft, struct timespec *ts);
	bool operator <(timespec &t1,timespec &t2);
	bool operator <=(timespec &t1,timespec &t2);
	void operator +=(timespec &t1,timespec &t2);
	void timespec_to_timeval(timespec *ts_source,timeval *tv_dest);
	void timeval_to_timespec(timeval *tv_source,timespec *ts_dest);
	int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

// Four levels of thread debugging with combinations of Debug and Thread_LOG
// Maximum debugging, DEBUG && THREAD_LOG means all locks and releases are logged

// x=the name of the instance, y=the mutex, z=optional information to log if lock fails
#ifdef DEBUG
	#ifdef THREAD_LOG
		#define PLUTO_SAFETY_LOCK_ERRORSONLY(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK(x,y) PlutoLock x(&y,__FILE__,__LINE__,false);
		#define PLUTO_SAFETY_LOCK_MESSAGE(x,y,z) PlutoLock x(&y,__FILE__,__LINE__,false,z);
	#else
		#define PLUTO_SAFETY_LOCK_ERRORSONLY(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK_MESSAGE(x,y,z) PlutoLock x(&y,__FILE__,__LINE__,true,z);
	#endif
#else
	#ifdef THREAD_LOG
		#define PLUTO_SAFETY_LOCK_ERRORSONLY(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK_MESSAGE(x,y,z) PlutoLock x(&y,__FILE__,__LINE__,true,z);
	#else
		#define PLUTO_SAFETY_LOCK(x,y) PlutoLock x(&y);
		#define PLUTO_SAFETY_LOCK_MESSAGE(x,y,z) PlutoLock x(&y);
		#define PLUTO_SAFETY_LOCK_ERRORSONLY(x,y) PlutoLock x(&y);
	#endif
#endif

// A Wrapper for pthread mutex
class pluto_pthread_mutex_t 
{
public:
    bool m_bInitialized;
    int m_Line,m_LockNum;
	pthread_mutex_t mutex;
	string m_sFileName;
	pthread_t m_thread;
	string m_sName;
	int m_NumLocks;  // For tracking recursive
	pluto_pthread_mutex_t(string Name) 
		: m_bInitialized(false), m_Line(0), m_LockNum(0), m_sFileName("NONE"), m_thread(pthread_self()), m_sName(Name) 
	{
		m_NumLocks=0;
	}
	void Init(pthread_mutexattr_t *type) 
	{ 
		pthread_mutex_init(&mutex, type); m_bInitialized=true; 
	}
};

class PlutoLock
{
public:
	string m_sFileName;
	int m_Line,m_LockNum;
	bool m_bReleased,m_bLog,m_bLogErrorsOnly,m_bGotLock;
	
	string m_sMessage;
	pluto_pthread_mutex_t *m_pMyLock;
	time_t m_tTime;
	pthread_t m_thread;

public:
	PlutoLock(pluto_pthread_mutex_t *pLock);
	PlutoLock(pluto_pthread_mutex_t *pLock,string File,int Line,bool bLogErrorsOnly);
	PlutoLock(pluto_pthread_mutex_t *pLock,string File,int Line,bool bLogErrorsOnly,string Message);

	~PlutoLock();
	void Relock();
	void DoLock();
	void Release();
	static void DumpOutstandingLocks();
	static void CheckLocks();
	static void ConfirmNoLocks(string File,int Line);
};

#endif


