#include "MouseIterator.h"
#include "MouseBehavior.h"
#include "MouseGovernor.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "pluto_main/Define_Button.h"

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
g_pPlutoLogger->Write(LV_FESTIVAL,"checking time %d last iterated %d frequency %d",dwTime,m_dwTime_Last_Iterated,m_dwFrequency_Ms);
		if( dwTime-m_dwTime_Last_Iterated>=m_dwFrequency_Ms )
		{
			// To prevent a deadlock we must always block the screen mutex before the iterator mutex
g_pPlutoLogger->Write(LV_FESTIVAL,"relocking 1");
			m.Release();
g_pPlutoLogger->Write(LV_FESTIVAL,"relocking 2");
			PLUTO_SAFETY_LOCK(sm,m_pOrbiter->m_ScreenMutex);
g_pPlutoLogger->Write(LV_FESTIVAL,"relocking 3");
			m.Relock();
g_pPlutoLogger->Write(LV_FESTIVAL,"iterating at %d",dwTime);
			DoIteration();
			m_dwTime_Last_Iterated=dwTime;
		}
		else
		{
			int TimeToWaitInMs = m_dwFrequency_Ms - ( dwTime-m_dwTime_Last_Iterated );
			int SecondsToWait = TimeToWaitInMs / 1000;
g_pPlutoLogger->Write(LV_FESTIVAL,"wawitng %d ms %d seconds",TimeToWaitInMs,SecondsToWait);
			m.TimedCondWait(SecondsToWait,(TimeToWaitInMs % 1000) * 1000000);
g_pPlutoLogger->Write(LV_FESTIVAL,"awoke");
		}
	}
	m_bThreadRunning=false;
}

void MouseIterator::SetIterator(EIteratorFunction eIteratorFunction,int dwParm,int dwFrequency_Ms)
{
	PLUTO_SAFETY_LOCK(sm,m_pOrbiter->m_ScreenMutex);
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
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Vol_Down.m_pMessage);
		}
		else if( m_dwParm>0 )
		{
			DCE::CMD_Vol_Up CMD_Vol_Up(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,m_dwParm);
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Vol_Up.m_pMessage);
		}
		break;
	case if_Light:
g_pPlutoLogger->Write(LV_FESTIVAL,"Sending light : %d",m_dwParm);
		{
			DCE::CMD_Set_Level CMD_Set_Level(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_LightingPlugIn,StringUtils::itos(m_dwParm*10));
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Level.m_pMessage);
		}
		break;
	case if_Keyboard:
		switch( m_dwParm )
		{
			case 'U':
			{
				DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
					StringUtils::itos(BUTTON_Scroll_Up_CONST),"");
				m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
			}
			break;

			case 'D':
			{
				DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
					StringUtils::itos(BUTTON_Scroll_Down_CONST),"");
				m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
			}
			break;

			case 'u':
			{
				DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
					StringUtils::itos(BUTTON_Up_Arrow_CONST),"");
				m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
			}
			break;

			case 'd':
			{
				DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
					StringUtils::itos(BUTTON_Down_Arrow_CONST),"");
				m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
			}
			break;

//TODO: remove this when we'll have these record in the database from Iasi.
#if 0 
			case 'L':
			{
				DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
					StringUtils::itos(BUTTON_Scroll_Left_CONST),"");
				m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
			}
			break;

			case 'R':
			{
				DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
					StringUtils::itos(BUTTON_Scroll_Right_CONST),"");
				m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
			}
			break;
#endif

			case 'l':
			{
				DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
					StringUtils::itos(BUTTON_Left_Arrow_CONST),"");
				m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
			}
			break;

			case 'r':
			{
				DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,
					StringUtils::itos(BUTTON_Right_Arrow_CONST),"");
				m_pOrbiter->SendCommand(CMD_Simulate_Keypress);
			}
			break;
		}
		break;
	case if_MediaTracks:
		{
			if( m_dwParm==-2 )
			{
g_pPlutoLogger->Write(LV_FESTIVAL,"page up");
				NeedToRender render( m_pOrbiter, "iterator grid" );  // Redraw anything that was changed by this command
				m_pOrbiter->CMD_Scroll_Grid("","",DIRECTION_Up_CONST);
			}
			else if( m_dwParm==-1 )
				m_pOrbiter->CMD_Move_Up();
			else if( m_dwParm==1 )
				m_pOrbiter->CMD_Move_Down();
			else if( m_dwParm==2 )
			{
g_pPlutoLogger->Write(LV_FESTIVAL,"page down");
				NeedToRender render( m_pOrbiter, "iterator grid" );  // Redraw anything that was changed by this command
				m_pOrbiter->CMD_Scroll_Grid("","",DIRECTION_Down_CONST);
			}
g_pPlutoLogger->Write(LV_FESTIVAL,"done Sending scroll");
			/*
			if( !m_pMouseBehavior->m_pMouseHandler_Vertical || !m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj || m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_ChildObjects.size()==0 )
				return; // Shouldn't happen, this should be the datagris control

			DesignObj_Orbiter *pObj_Child = (DesignObj_Orbiter *) *(m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_ChildObjects.begin());
			PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Always lock this before datagrid to prevent a deadlock
			PLUTO_SAFETY_LOCK( dng, m_pMouseBehavior->m_pOrbiter->m_DatagridMutex );
			if( pObj_Child->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
				return; // Also shouldn't happen
			DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObj_Child;
			if( !pObj_Grid->m_pDataGridTable )
				return; // Again shouldn't happen
				*/
		}

		break;
	};
}

