#ifndef TimedEvent_H
#define TimedEvent_H

#define	INTERVAL_EVENT	1
#define DAY_OF_WEEK		2
#define DAY_OF_MONTH	3
#define ABSOLUTE_TIME	4

class Row_EventHandler;

namespace DCE
{
	class CommandGroup;
}

using namespace DCE;

class TimedEvent
{
public:
	time_t m_tTime;
	int m_iTimedEventType;
	
	Row_EventHandler *m_pRow_EventHandler;
	CommandGroup *m_pCommandGroup;
	string m_sDaysOfWeek,m_sTimes,m_sDaysOfMonth;

	TimedEvent(Row_EventHandler *pRow_EventHandler);
	void CalcNextTime();

    /**
		Finds the next time in sTimes that is after tmAfter, or the first time in sTimes if tmAfter is NULL,
		and stores it in tmOutput
        */
	bool SetNextTime(tm *tmAfter,tm *tmOutput,string sTimes);
};

typedef map<int,TimedEvent *> MapTimedEvent;

#endif

