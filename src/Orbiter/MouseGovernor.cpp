#include "MouseGovernor.h"
#include "MouseBehavior.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"

using namespace DCE;

void *GovernorThread(void *p)
{
	MouseGovernor *pMouseGovernor = (MouseGovernor *) p;
	pMouseGovernor->Run();
	return NULL;
}

//-----------------------------------------------------------------------------------------------------
MouseGovernor::MouseGovernor(MouseBehavior *pMouseBehavior) : m_GovernorMutex("governor")
{
	m_pMouseBehavior=pMouseBehavior;

	pthread_cond_init(&m_GovernorCond, NULL);
	m_GovernorMutex.Init(NULL,&m_GovernorCond);

	m_bQuit=false;
	m_bThreadRunning=true;
	m_dwBufferMs=0;
	m_dwTime_Last_SentMessage=0;
	m_pMessage=NULL;

	pthread_t pthread_id; 
	if(pthread_create( &pthread_id, NULL, GovernorThread, (void*)this) )
	{
		m_bThreadRunning=false;
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create Governor thread" );
	}
	else
		pthread_detach(pthread_id);
}

MouseGovernor::~MouseGovernor()
{
	m_bQuit=true;
	while( m_bThreadRunning )
	{
		pthread_cond_broadcast( &m_GovernorCond );
		Sleep(5);
	}
}

void MouseGovernor::Run()
{
	PLUTO_SAFETY_LOCK(m,m_GovernorMutex);
	while( !m_bQuit )
	{
		if( m_dwBufferMs==0 && m_pMessage==NULL )
			m.CondWait(); // Nothing to do.  Just wait
		unsigned long dwTime = ProcessUtils::GetMsTime();
		if( dwTime-m_dwTime_Last_SentMessage>=m_dwBufferMs && m_pMessage )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseGovernor::Run sending %p",m_pMessage);
			DoSendMessage(m_pMessage);
			m_pMessage=NULL;
		}
		else
		{
			int TimeToWaitInMs = m_dwBufferMs - ( dwTime-m_dwTime_Last_SentMessage );
			int SecondsToWait = TimeToWaitInMs / 1000;
			m.TimedCondWait(SecondsToWait,(TimeToWaitInMs % 1000) * 1000000);
		}
	}
	m_bThreadRunning=false;
}

void MouseGovernor::SendMessage(Message *pMessage)
{
	PLUTO_SAFETY_LOCK(m,m_GovernorMutex);
	if( m_dwBufferMs==0 )
	{
		DoSendMessage(pMessage);
		return; // Not governing the messages now
	}

	unsigned long dwTime = ProcessUtils::GetMsTime();
	if( dwTime-m_dwTime_Last_SentMessage>=m_dwBufferMs )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseGovernor::Move sending %p id %d to %d",pMessage,pMessage->m_dwID,pMessage->m_dwPK_Device_To);
		DoSendMessage(pMessage);
		return; // It's been long enough governing the messages now
	}

	// It's coming in too fast.  We're going to buffer this
	if( m_pMessage )
	{
		// We're only buffering this message if it's a duplicate of the last.  If it's not, 
		// send the last
		if( m_pMessage->m_dwMessage_Type!=pMessage->m_dwMessage_Type || 
			m_pMessage->m_dwID!=pMessage->m_dwID ||
			m_pMessage->m_dwPK_Device_To!=pMessage->m_dwPK_Device_To )
				DoSendMessage(m_pMessage);
		else
			delete m_pMessage; // It's a duplicate, delete it
	}
	m_pMessage=pMessage;
	pthread_cond_broadcast( &m_GovernorCond );
}

void MouseGovernor::DoSendMessage(Message *pMessage)
{
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseGovernor::DoSendMessage ****sending %p last %d**** id %d to %d",pMessage,m_dwTime_Last_SentMessage,pMessage->m_dwID,pMessage->m_dwPK_Device_To);
	m_pMouseBehavior->m_pOrbiter->QueueMessageToRouter(pMessage);
	m_dwTime_Last_SentMessage=ProcessUtils::GetMsTime();
}

void MouseGovernor::SetBuffer(int MS)
{
	PLUTO_SAFETY_LOCK(m,m_GovernorMutex);
	g_pPlutoLogger->Write(LV_FESTIVAL,"MouseGovernor::SetBuffer %d",MS);
	m_dwBufferMs=MS;
	m_dwTime_Last_SentMessage=0;
}

void MouseGovernor::Purge()
{
	PLUTO_SAFETY_LOCK(m,m_GovernorMutex);
	delete m_pMessage;
	m_pMessage=NULL;
}
