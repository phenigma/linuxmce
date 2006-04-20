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

using namespace DCE;

const int SpeedMouseHandler::m_iSpeeds[] = {0,250,500,1000,2000,3000,4000,6000,8000,10000,15000,20000,30000,50000,100000,200000,400000};

SpeedMouseHandler::SpeedMouseHandler(DesignObj_Orbiter *pObj,MouseBehavior *pMouseBehavior) 
	: MouseHandler(pObj,pMouseBehavior)
{
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
	m_iLastNotch=0;
	m_pMouseBehavior->m_iTime_Last_Mouse_Down=ProcessUtils::GetMsTime();  // The above may have taken too much time already
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
		m_iLastNotch=0;
		int Percentage = m_CurrentMedia_Pos==0 ? 0 : (m_CurrentMedia_Stop-m_CurrentMedia_Start)/m_CurrentMedia_Pos;
		int X = (m_pObj->m_rPosition.Width * Percentage / 100) + m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X;
		int Y = m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y + (m_pObj->m_rPosition.Height/2);
		m_pMouseBehavior->SetMousePosition(X,Y);
	// remus -- we are using absolute seeking
	}
	else
	{
		m_pMouseBehavior->SetMousePosition(m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
		// remus -- we are using relative seeking
	}
}

void SpeedMouseHandler::Stop()
{
}

bool SpeedMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_6_CONST || PK_Button==BUTTON_Mouse_2_CONST )
	{
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
	if( PK_Button==BUTTON_Mouse_6_CONST && !m_bTapAndRelease )  // The user was in press and hold mode, or tapped again after the menu appeared on screen
	{
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
g_pPlutoLogger->Write(LV_FESTIVAL,"speed %d",Speed);
			DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,Speed);
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Change_Playback_Speed.m_pMessage);
			m_iLastNotch=Speed;
//remus  m_pObj->SetSpeed(Speed);
		}
	}
	else  // holding down, go to position
	{
		int Offset = X - m_pObj->m_rPosition.X-m_pObj->m_pPopupPoint.X;
		double Perc = (double) Offset / m_pObj->m_rPosition.Width;
		int Time = (m_CurrentMedia_Stop-m_CurrentMedia_Start) * Perc + m_CurrentMedia_Start;
		DCE::CMD_Set_Media_Position CMD_Set_Media_Position(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,"POS:" + StringUtils::itos(Time*1000));
		m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Media_Position.m_pMessage);
//remus  m_pObj->SetSpeed(Speed);
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
