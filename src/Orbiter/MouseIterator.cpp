#include "MouseIterator.h"
#include "MouseBehavior.h"
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

	pthread_cond_init(&m_IteratorCond, NULL);
	m_IteratorMutex.Init(NULL,&m_IteratorCond);

	m_bQuit=false;
	m_bThreadRunning=true;
	m_EMenuOnScreen=mos_None;

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
		if( m_EMenuOnScreen==mos_None )
			m.CondWait(); // Nothing to do.  Just wait
		/*
		unsigned long dwTime = ProcessUtils::GetMsTime();
		if( dwTime-m_dwTime_Last_Reported_Mouse>=m_dwBufferMs )
		{
			m.Release();
			m_pMouseBehavior->Move(m_X,m_Y);
			m.Relock();
		}
		else
		{
			int TimeToWaitInMs = m_dwBufferMs - ( dwTime-m_dwTime_Last_Reported_Mouse );
			int SecondsToWait = TimeToWaitInMs / 1000;
			m.TimedCondWait(SecondsToWait,(TimeToWaitInMs % 1000) * 1000000);
		}
		*/
	}
	m_bThreadRunning=false;
}
