#include "LightMouseHandler.h"
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

void LightMouseHandler::Start()
{
	m_pMouseBehavior->m_pMouseGovernor->SetBuffer(500);
	if( m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_iLastNotch=5;
		DrawSquare(10-m_iLastNotch,PlutoColor::White());
	}
	else
	{
		m_bTapAndRelease=false;
		m_iLastNotch=0;
		DrawSquare(5-m_iLastNotch,PlutoColor::White());
	}

	g_pPlutoLogger->Write(LV_FESTIVAL,"LightMouseHandler::LightControl starting");
	m_pMouseBehavior->SetMousePosition(m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
}

void LightMouseHandler::Stop()
{
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "change light" );
PLUTO_SAFETY_LOCK(nd,m_pMouseBehavior->m_pOrbiter->m_NeedRedrawVarMutex);
m_pMouseBehavior->m_pOrbiter->m_vectObjs_NeedRedraw.push_back(m_pObj);
g_pPlutoLogger->Write(LV_CORPCLIENT,"m_pObj.disabled = true");
}

bool LightMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_8_CONST )
		m_pMouseBehavior->Clear(); // this will be deleted
	return false; // Keep processing
}

bool LightMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_2_CONST && m_bTapAndRelease )
	{
		DCE::CMD_Set_Level CMD_Set_Level(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(m_iCancelLevel));
		m_pMouseBehavior->m_pOrbiter->SendMessage(CMD_Set_Level.m_pMessage);
		m_pMouseBehavior->Clear(); // this will be deleted
		return false; // Keep processing
	}
	else if( PK_Button==BUTTON_Mouse_8_CONST && !m_bTapAndRelease )
	{
		m_pMouseBehavior->Clear(); // this will be deleted
		return false; // Keep processing

	}
	return false; // Keep processing
}

void LightMouseHandler::Move(int X,int Y)
{
	int NotchWidth = m_pObj->m_rPosition.Height/11; // Allow for 5 repeat levels in each direction
	int Notch = (Y - m_pObj->m_rPosition.Y - m_pObj->m_pPopupPoint.Y) * 11 / m_pObj->m_rPosition.Height;
	if( Notch<0 )
		Notch=0;
	if( Notch>10 )
		Notch=10;

	if( m_bTapAndRelease )
		Notch = 10-Notch;  // Higher number at top
	else
		Notch = 5-Notch;  // Higher number at top

	if( Notch!=m_iLastNotch )
	{
		if( m_bTapAndRelease )
		{
g_pPlutoLogger->Write(LV_FESTIVAL,"Setting light to : %d  X %d",Notch,X);
			DrawSquare(10-m_iLastNotch,PlutoColor::Blue());
			DCE::CMD_Set_Level CMD_Set_Level(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_LightingPlugIn,StringUtils::itos(Notch*10));
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Level.m_pMessage);
			m_iLastNotch = Notch;
			DrawSquare(10-m_iLastNotch,PlutoColor::White());
		}
		else
		{
			DrawSquare(5-m_iLastNotch,PlutoColor::Blue());
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Light,Notch,500,NULL);
			m_iLastNotch = Notch;
			DrawSquare(5-m_iLastNotch,PlutoColor::White());
		}
	}
}

void LightMouseHandler::DrawSquare(int Notch,PlutoColor plutoColor)
{
	int NotchHeight = m_pObj->m_rPosition.Height/11; // Allow for 7 repeat levels in each direction
	int NotchStart = Notch * NotchHeight + m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y;
	m_pMouseBehavior->m_pOrbiter->HollowRectangle(
	m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.X, NotchStart,
	m_pObj->m_rPosition.Width, NotchHeight,
	plutoColor);
	m_pMouseBehavior->m_pOrbiter->UpdateRect(m_pObj->m_rPosition + m_pObj->m_pPopupPoint);
}
