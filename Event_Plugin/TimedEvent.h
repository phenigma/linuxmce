#ifndef TimedEvent_H
#define TimedEvent_H

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

typedef vector<TimedEvent *> VectTimedEvent;

#endif

