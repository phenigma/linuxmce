#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

typedef vector<class Command *> CommandVector;

class EventHandler
{
public:
	bool m_bExecuted;
	int m_PK_EventHander;
	class EventHandler_CriteriaList *m_ptrCriteria;
	class EventInstance *m_ptrEventInstance;
	class Event *m_ptrEvent;
	class CommandGroup *m_ptrCommandGroup;
	vector<class CriteriaParm *> m_vectCriteriaParm;

	EventHandler(int ID,class Event *ptrEvent,class CommandGroup *ptrCommandGroup,class EventHandler_CriteriaList *ptrCriteria)
	{
		m_PK_EventHander = ID;
		m_ptrCriteria = ptrCriteria;
		m_ptrEvent = ptrEvent;
		m_ptrCommandGroup = ptrCommandGroup;
		m_ptrEventInstance = NULL;
		m_bExecuted=false;
	}
};

#endif

