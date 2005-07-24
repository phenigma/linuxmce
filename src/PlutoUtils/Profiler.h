#ifndef Profiler_h
#define Profiler_h

#include "DCE/Logger.h"

using namespace DCE;
#ifndef WIN32
class PlutoProfiler
{
public:
	void Start(const char *pName) {}
	void Stop(const char *pName) {}
	void DumpResults() {}
	void Error(string sID,string sMessage) {}
};
#else

typedef map<string, pair<LARGE_INTEGER,int> > MapProfiles;
class PlutoProfiler
{
public:
	MapProfiles m_MapProfiles;  // String = section name, LARGE_INTEGER = start time, int = cumulative time
	LARGE_INTEGER m_Freq;      // ticks per second

	PlutoProfiler()
	{
		QueryPerformanceFrequency(&m_Freq);
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}
	~PlutoProfiler()
	{
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}

	void Start(const char *pName)
	{
		LARGE_INTEGER tStart;
		QueryPerformanceCounter(&tStart);
		MapProfiles::iterator it = m_MapProfiles.find(pName);
		if( it==m_MapProfiles.end() )
			m_MapProfiles[pName] = make_pair<LARGE_INTEGER,int> (tStart,0);
		else if( it->second.first.QuadPart!=0 )
			Error(pName,"Already started");
		else
			it->second.first = tStart;
	}

	void Stop(const char *pName)
	{
		LARGE_INTEGER tStop;
		QueryPerformanceCounter(&tStop);

		MapProfiles::iterator it = m_MapProfiles.find(pName);
		if( it==m_MapProfiles.end() )
			Error(pName,"not started");
		else
		{
			LARGE_INTEGER diff;
			diff.QuadPart = ( (tStop.QuadPart-it->second.first.QuadPart) * 1000000 / m_Freq.QuadPart );
			it->second.second = it->second.second + (int) diff.QuadPart;
			it->second.first.QuadPart = 0;
		}
	}

	void DumpResults()
	{
		for(MapProfiles::iterator it = m_MapProfiles.begin();it != m_MapProfiles.end();++it)
			g_pPlutoLogger->Write(LV_WARNING,"ID: %s used %d nanoseconds",it->first.c_str(),it->second.second);
	}

	void Error(string sID,string sMessage)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Profiling failed: %s %s",sID.c_str(),sMessage.c_str());
	}
};

#endif

extern PlutoProfiler *g_PlutoProfiler=NULL;

#endif
