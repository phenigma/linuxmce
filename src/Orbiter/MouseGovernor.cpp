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
	m_dwTime_Last_Reported_Mouse=0;

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
		if( m_dwBufferMs==0 )
			m.CondWait(); // Nothing to do.  Just wait
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
	}
	m_bThreadRunning=false;
}

bool MouseGovernor::Move(int X, int Y)
{
	if( m_dwBufferMs==0 )
		return true; // Not governing the mouse now

	unsigned long dwTime = ProcessUtils::GetMsTime();
	if( dwTime-m_dwTime_Last_Reported_Mouse>=m_dwBufferMs )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"MouseGovernor::Move reporting %d,%d",X,Y);
		m_dwTime_Last_Reported_Mouse = dwTime;
		return true; // Go ahead and report it
	}

	// It's coming in too fast.  We're going to buffer this
	m_X=X;
	m_Y=Y;
	pthread_cond_broadcast( &m_GovernorCond );
	return false;  // Don't process it now
}

void MouseGovernor::SetBuffer(int MS)
{
	g_pPlutoLogger->Write(LV_FESTIVAL,"MouseGovernor::SetBuffer %d",MS);
	m_dwBufferMs=MS;
	m_dwTime_Last_Reported_Mouse=0;
}

