#ifndef EVENTINFO_H
#define EVENTINFO_H

#include "EventHandler.h"

namespace DCE
{
	class DeviceData_Router;
	class Message;
}

using namespace DCE;

class EventInfo
{
public:
	int m_iPK_Event;
	Message *m_pMessage;
	class EventHandler *m_pEventHandler;
	DeviceData_Router *m_pDevice;
	int m_PK_HouseMode;
	vector<class EventHandler *> m_vectEventHandlers;

	EventInfo(int PK_Event,Message *pMessage,DeviceData_Router *pDevice,int PK_HouseMode) :
		m_iPK_Event(PK_Event), m_pMessage(pMessage), m_pEventHandler(NULL), m_pDevice(pDevice),
		m_PK_HouseMode(PK_HouseMode)
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

