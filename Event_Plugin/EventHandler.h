#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

typedef vector<class Action *> ActionVector;

class EventHandler
{
public:
	int m_PK_EventHander,m_PK_Event;
	class Criteria *m_pCriteria;
	class CommandGroup *m_pCommandGroup;

	EventHandler(int PK_EventHander,int PK_Event,class CommandGroup *pCommandGroup,class Criteria *pCriteria)
	{
		m_PK_EventHander = PK_EventHander;
		m_PK_Event = PK_Event;
		m_pCriteria = pCriteria;
		m_pCommandGroup = pCommandGroup;
	}
};

#endif

