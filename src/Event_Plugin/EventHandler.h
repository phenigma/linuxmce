#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

namespace DCE
{
	class CommandGroup;
}

using namespace DCE;

class EventHandler
{
public:
	unsigned long m_PK_EventHander,m_PK_Event;
	class Criteria *m_pCriteria;
	CommandGroup *m_pCommandGroup;

	EventHandler(unsigned long PK_EventHander,unsigned long PK_Event,CommandGroup *pCommandGroup,class Criteria *pCriteria)
	{
		m_PK_EventHander = PK_EventHander;
		m_PK_Event = PK_Event;
		m_pCriteria = pCriteria;
		m_pCommandGroup = pCommandGroup;
	}
};

typedef list<EventHandler *> ListEventHandler;

#endif

