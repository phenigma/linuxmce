#ifndef EVENT_H
#define EVENT_H

class Event
{
public:
	int m_PK_Event;
	vector<class EventHandler *> m_vectEventHandlers;

	Event(int ID)
	{
		m_PK_Event = ID;
	}
};

#endif

