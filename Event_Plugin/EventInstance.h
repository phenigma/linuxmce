#ifndef EVENTINSTANCE_H
#define EVENTINSTANCE_H

class EventInstance
{
public:
	class EventHandler *m_ptrEventHandler;
	class OCMessage *pMessage;
	int m_ID,m_ActionNum,m_PKID_Device_OriginatedThisEvent;
	clock_t m_tTime;
//	clock_t m_ctWaitUntil;
	
	int m_TimerID;

	EventInstance(int ID,class EventHandler *ptrEventHandler)
	{
		pMessage=NULL;
		m_ActionNum=0;
		m_ptrEventHandler=ptrEventHandler;
		m_ID = ID;
		m_PKID_Device_OriginatedThisEvent=0;
	}

	~EventInstance()
	{
		if( pMessage!=NULL )
			delete pMessage;
	}
};

#endif

