#include "SpeedMouseHandler.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"

#ifndef WIN32
#	include "Linux/wmtask.h"
#	include "Linux/wmtaskmanager.h"
#	include "CallBackData.h"
#endif

using namespace DCE;

const int SpeedMouseHandler::m_iSpeeds[] = {0,250,500,1000,2000,3000,4000,6000,8000,10000,15000,20000,30000,50000,100000,200000,400000};

SpeedMouseHandler::SpeedMouseHandler(DesignObj_Orbiter *pObj,MouseBehavior *pMouseBehavior) 
	: MouseHandler(pObj,pMouseBehavior)
{
g_pPlutoLogger->Write(LV_CRITICAL,"Speed Control widget on screen!");	

	m_bTapAndRelease=false; // We'll set it to true later
	string sResponse;
	DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,0);
	m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed,&sResponse);

	string sText,sMediaPosition;
	DCE::CMD_Report_Playback_Position CMD_Report_Playback_Position(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,&sText,&sMediaPosition);
	m_bHasTimeline = m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Report_Playback_Position) && ParsePosition(sMediaPosition);
	m_iCancelLevel = m_CurrentMedia_Pos;
m_bHasTimeline=true;
m_CurrentMedia_Start=0;m_CurrentMedia_Stop=7200;m_CurrentMedia_Pos=3600;
pObj = m_pMouseBehavior->m_pOrbiter->FindObject(DESIGNOBJ_popSpeedControl_CONST);  // See if the speed control applet is on screen
g_pPlutoLogger->Write(LV_CRITICAL,"Speed Control widget %s on screen",(pObj && pObj->m_bOnScreen) ? "IS" : "IS NOT");

	m_iLastNotch=0;
	m_pMouseBehavior->m_iTime_Last_Mouse_Down=ProcessUtils::GetMsTime();  // The above may have taken too much time already
	Update();
}

void SpeedMouseHandler::Start()
{
	m_pMouseBehavior->m_pMouseGovernor->SetBuffer(2000);
	if( !m_bHasTimeline || m_pMouseBehavior->m_iTime_Last_Mouse_Up )
		m_bTapAndRelease=true;
	else
		m_bTapAndRelease=false;  // Can only do this if we have the timeline, and the user is holding down

	if( m_bHasTimeline && m_bTapAndRelease==false )
	{
		m_iLastNotch=m_CurrentMedia_Pos;
		float Percentage = m_CurrentMedia_Pos==0 ? 0 : ((float) m_CurrentMedia_Pos)/(m_CurrentMedia_Stop-m_CurrentMedia_Start);
		int X = (m_pObj->m_rPosition.Width * Percentage) + m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X;
		int Y = m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y + (m_pObj->m_rPosition.Height/2);
		m_pMouseBehavior->SetMousePosition(X,Y);
	}
	else
	{
		m_pMouseBehavior->SetMousePosition(m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
	}
	Update();
}

void SpeedMouseHandler::Stop()
{
	Update();
}

bool SpeedMouseHandler::ButtonDown(int PK_Button)
{
	g_pPlutoLogger->Write(LV_FESTIVAL,"SpeedMouseHandler::ButtonDown %d",PK_Button);
	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_6_CONST || PK_Button==BUTTON_Mouse_2_CONST )
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"SpeedMouseHandler::ButtonDown cancelling");
		m_pMouseBehavior->m_pMouseGovernor->Purge();
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
		if( PK_Button==BUTTON_Mouse_2_CONST && m_bHasTimeline )
		{
			DCE::CMD_Set_Media_Position CMD_Set_Media_Position(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,"POS:" + StringUtils::itos(m_iCancelLevel*1000));
			m_pMouseBehavior->m_pOrbiter->SendMessage(CMD_Set_Media_Position.m_pMessage);
		}
		m_pMouseBehavior->Clear(); // this will be deleted
		return false; // Keep processing
	}
	return false; // Keep processing
}

bool SpeedMouseHandler::ButtonUp(int PK_Button)
{
	g_pPlutoLogger->Write(LV_FESTIVAL,"SpeedMouseHandler::ButtonUp %d",PK_Button);
	if( PK_Button==BUTTON_Mouse_6_CONST && !m_bTapAndRelease )  // The user was in press and hold mode, or tapped again after the menu appeared on screen
	{
		g_pPlutoLogger->Write(LV_FESTIVAL,"SpeedMouseHandler::ButtonUp cancelling");
		m_pMouseBehavior->m_pMouseGovernor->Purge();
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
		m_pMouseBehavior->Clear();
		return false; // this is now invalid
	}
	return false; // Keep processing
}

void SpeedMouseHandler::Move(int X,int Y)
{
	int XStart = m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2;
	if( m_bTapAndRelease || !m_bHasTimeline )  // holding button down, change speed, or this stream doesn't support absolute positioning anyway
	{
		int NotchWidth = m_pObj->m_rPosition.Width/2/MAX_SPEEDS;
		int Notch = abs(X-XStart) / NotchWidth;
		if( Notch>MAX_SPEEDS-1 )
			Notch = MAX_SPEEDS-1;
		int Speed = m_iSpeeds[Notch];
		if( X<XStart )
			Speed *= -1;
		if( Speed!=m_iLastNotch )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"SpeedMouseHandler::Move speed %d",Speed);
			DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,Speed);
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Change_Playback_Speed.m_pMessage);
			m_iLastNotch=Speed;
			Update();
		}
	}
	else  // holding down, go to position
	{
		int Offset = X - m_pObj->m_rPosition.X-m_pObj->m_pPopupPoint.X;
		double Perc = (double) Offset / m_pObj->m_rPosition.Width;
		m_iLastNotch = (m_CurrentMedia_Stop-m_CurrentMedia_Start) * Perc + m_CurrentMedia_Start;
g_pPlutoLogger->Write(LV_FESTIVAL,"SpeedMouseHandler::Move seek %d",m_iLastNotch);
		DCE::CMD_Set_Media_Position CMD_Set_Media_Position(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,"POS:" + StringUtils::itos(m_iLastNotch*1000));
		m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Media_Position.m_pMessage);
		Update();
	}
}

bool SpeedMouseHandler::ParsePosition(string sMediaPosition)
{
	m_CurrentMedia_Start=0;
	string::size_type pos=sMediaPosition.find("POS:");
	if( pos==string::npos )
		return false;
	m_CurrentMedia_Pos = atoi( sMediaPosition.substr(pos+4).c_str() ) / 1000;
	
	pos=sMediaPosition.find("TOTAL:");
	if( pos==string::npos )
		return false;
	m_CurrentMedia_Stop = atoi( sMediaPosition.substr(pos+6).c_str() ) / 1000;
	
	return m_CurrentMedia_Stop>0;
}

void SpeedMouseHandler::Update()
{
	int Style=0; // Option #1, the style
	char *SpeedOptions = "FULL";  // Option #2, the number of available speeds
	int Speed = -9999;  // Option #3, the current speed, -9999 = don't show any speed
	int SeekPosition = -1;  // Option #5, the current seek to position.  -1 = none

	if( m_bHasTimeline )
	{
		if( !m_bIsActive )
			Style = SPEED_STYLE_TIMELINE_IDLE;
		else if( m_bTapAndRelease )
			Style = SPEED_STYLE_TIMELINE_SPEED;
		else
			Style = SPEED_STYLE_TIMELINE_SEEK;
	}
	else
		Style = SPEED_STYLE_SPEED_ONLY;

	// We only set the current speed for these
	if( (Style==SPEED_STYLE_SPEED_ONLY || Style==SPEED_STYLE_TIMELINE_SPEED) && m_bIsActive )
		Speed = m_iLastNotch;

	if( Style==SPEED_STYLE_TIMELINE_SEEK && m_bIsActive )
		SeekPosition = m_iLastNotch;

	static unsigned long m_ulLastTimeControlUpdated = 0;
	unsigned long ulCurrentTimeControlUpdated = ProcessUtils::GetMsTime();
	
	if(ulCurrentTimeControlUpdated - m_ulLastTimeControlUpdated < 50)
	{
		//we don't want to fill the queue with very lots tasks, because the speed control
		//might not be able to process all of them
		//only 20 per seconds should be enough
		g_pPlutoLogger->Write(LV_WARNING,"SpeedMouseHandler::Update ignored. Only %ul ms passed since "
			"the speed control was updated", ulCurrentTimeControlUpdated - m_ulLastTimeControlUpdated);
		return;
	}
	
	//update the time - will update the control right now
	m_ulLastTimeControlUpdated = ulCurrentTimeControlUpdated;
		
	g_pPlutoLogger->Write(LV_WARNING,"SpeedMouseHandler::Update parm 1: %d 2: %s 3: %d 4: %d/%d/%d 5: %d",
		Style,SpeedOptions,Speed,m_CurrentMedia_Start,m_CurrentMedia_Stop,m_CurrentMedia_Pos,SeekPosition);
		
		//for now, available only for linux; taskmanager will be included in windows version in few days
#ifndef WIN32
    {
		PlutoRectangle plutoRect = m_pObj->m_rPosition;
    	g_pPlutoLogger->Write( LV_WARNING, "SpeedControl: Refresh, x=%d, y=%d, w=%d, h=%d",
                           plutoRect.X, plutoRect.Y, plutoRect.Width, plutoRect.Height );
		
		SpeedControlCallBackData *pSpeedControlData = new SpeedControlCallBackData(plutoRect);

		//translate Aaron's defines to Remus's enums
		switch(Style)
		{
			case SPEED_STYLE_TIMELINE_IDLE:		pSpeedControlData->m_eStyle = SpeedControlCallBackData::TIME; break;
			case SPEED_STYLE_TIMELINE_SPEED:	pSpeedControlData->m_eStyle = SpeedControlCallBackData::TIME_SEEK; 	break;
			case SPEED_STYLE_TIMELINE_SEEK:		pSpeedControlData->m_eStyle = SpeedControlCallBackData::TIME_SPEED; 	break;
			case SPEED_STYLE_SPEED_ONLY:		pSpeedControlData->m_eStyle = SpeedControlCallBackData::SPEED; 		break;
			
			default: 
				pSpeedControlData->m_eStyle = SpeedControlCallBackData::UNUSED; 
				break;
		}
		
		pSpeedControlData->m_nSpeed = Speed;
		pSpeedControlData->m_nTimeStart = m_CurrentMedia_Start;
		pSpeedControlData->m_nTimeEnd = m_CurrentMedia_Stop;
		pSpeedControlData->m_nTimeNow = m_CurrentMedia_Pos;
		pSpeedControlData->m_nSeekToPos = SeekPosition;

		//for now SpeedOptions == "FULL"; Remus will receive all the speeds
		for(int i = 0; i < sizeof(m_iSpeeds) / sizeof(m_iSpeeds[0]); ++i)
			pSpeedControlData->m_listSpeeds.push_back(m_iSpeeds[i]);
		
        WMTask *pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, E_Dialog_SpeedControl, pSpeedControlData);
        TaskManager::Instance().AddTask(pTask);
	}
#endif
}

