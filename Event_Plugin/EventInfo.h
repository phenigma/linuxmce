#ifndef EVENTINFO_H
#define EVENTINFO_H

#include "EventHandler.h"

class EventInfo
{
public:
	int m_iPKID_EventList;
	class EventList *pEventList;
	class PlutoEvents *m_pPlutoEvents;
	class OCMessage *pMessage;
	class OCLogger *pLogger;
	class EventHandler *pEventHandler;
	class Device *pDevice;
	int PKID_C_Mode_Room,PKID_C_Mode_House;
	vector<class EventHandler *> m_vectEventHandlers;

	EventInfo(class PlutoEvents *plutoEvents,class EventList *eventlist,class OCMessage *message,class OCLogger *logger,class Device *device,int C_Mode_Room,int C_Mode_House) :
		pEventList(eventlist), m_pPlutoEvents(plutoEvents), pMessage(message), pLogger(logger), 
		pEventHandler(NULL), pDevice(device),PKID_C_Mode_Room(C_Mode_Room),PKID_C_Mode_House(C_Mode_House) 
	{};
	~EventInfo()
	{
		for(int i=0;i<(int) m_vectEventHandlers.size();++i)
		{
			delete m_vectEventHandlers[i];
		}
	}
};

#endif

