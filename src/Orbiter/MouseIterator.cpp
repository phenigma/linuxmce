#include "MouseIterator.h"
#include "MouseBehavior.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"

using namespace DCE;

void *IteratorThread(void *p)
{
	MouseIterator *pMouseIterator = (MouseIterator *) p;
	pMouseIterator->Run();
	return NULL;
}

//-----------------------------------------------------------------------------------------------------
MouseIterator::MouseIterator(MouseBehavior *pMouseBehavior) : m_IteratorMutex("Iterator")
{
	m_pMouseBehavior=pMouseBehavior;
	m_pOrbiter=m_pMouseBehavior->m_pOrbiter;

	pthread_cond_init(&m_IteratorCond, NULL);
	m_IteratorMutex.Init(NULL,&m_IteratorCond);

	m_bQuit=false;
	m_bThreadRunning=true;
	m_EIteratorFunction=if_None;
	m_dwTime_Last_Iterated=0;

	pthread_t pthread_id; 
	if(pthread_create( &pthread_id, NULL, IteratorThread, (void*)this) )
	{
		m_bThreadRunning=false;
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create Iterator thread" );
	}
	else
		pthread_detach(pthread_id);
}

MouseIterator::~MouseIterator()
{
	m_bQuit=true;
	while( m_bThreadRunning )
	{
		pthread_cond_broadcast( &m_IteratorCond );
		Sleep(5);
	}
}

void MouseIterator::Run()
{
	PLUTO_SAFETY_LOCK(m,m_IteratorMutex);
	while( !m_bQuit )
	{
		if( m_EIteratorFunction==if_None )
			m.CondWait(); // Nothing to do.  Just wait
		
		unsigned long dwTime = ProcessUtils::GetMsTime();
		if( dwTime-m_dwTime_Last_Iterated>=m_dwFrequency_Ms )
		{
			DoIteration();
			m_dwTime_Last_Iterated=dwTime;
		}
		else
		{
			int TimeToWaitInMs = m_dwFrequency_Ms - ( dwTime-m_dwTime_Last_Iterated );
			int SecondsToWait = TimeToWaitInMs / 1000;
			m.TimedCondWait(SecondsToWait,(TimeToWaitInMs % 1000) * 1000000);
		}
	}
	m_bThreadRunning=false;
}

void MouseIterator::SetIterator(EIteratorFunction eIteratorFunction,int dwParm,int dwFrequency_Ms)
{
	PLUTO_SAFETY_LOCK(m,m_IteratorMutex);
	m_EIteratorFunction=eIteratorFunction;
	m_dwParm=dwParm;
	m_dwFrequency_Ms=dwFrequency_Ms;
	switch(eIteratorFunction)
	{
	case if_Volume:
			break;
	};
	if( m_EIteratorFunction!=if_None )
		pthread_cond_broadcast( &m_IteratorCond );
}

void MouseIterator::DoIteration()
{
	switch(m_EIteratorFunction)
	{
	case if_Volume:
g_pPlutoLogger->Write(LV_FESTIVAL,"Sending vol : %d",m_dwParm);
		if( m_dwParm<0 )
		{
			DCE::CMD_Vol_Down CMD_Vol_Down(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,m_dwParm*-1);
			m_pOrbiter->SendCommand(CMD_Vol_Down);
		}
		else if( m_dwParm>0 )
		{
			DCE::CMD_Vol_Up CMD_Vol_Up(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,m_dwParm);
			m_pOrbiter->SendCommand(CMD_Vol_Up);
		}
		break;
	};
}

