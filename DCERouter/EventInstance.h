#ifndef EVENTINSTANCE_H
#define EVENTINSTANCE_H

#include "Message.h"
using namespace DCE;
class EventInstance
{
public:
	class EventHandler *m_ptrEventHandler;
	Message *pMessage;
	int m_ID,m_CommandNum,m_PK_Device_OriginatedThisEvent;
	clock_t m_tTime;
//	clock_t m_ctWaitUntil;
	
	int m_TimerID;

	EventInstance(int ID,class EventHandler *ptrEventHandler)
	{
		pMessage=NULL;
		m_CommandNum=0;
		m_ptrEventHandler=ptrEventHandler;
		m_ID = ID;
		m_PK_Device_OriginatedThisEvent=0;
	}

	~EventInstance()
	{
		if( pMessage!=NULL )
			delete pMessage;
	}
};

#endif

