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
	if( m_pMouseBehavior->m_pMouseHandler_Horizontal && m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj )
	{
		NeedToRender render( m_pMouseBehavior->m_pOrbiter, "start light" );
		m_pMouseBehavior->m_bMouseHandler_Vertical_Exclusive = true;
		m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj->m_bHidden = true;
		m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj);// Redraw even if the object was already in this state,  because maybe we're hiding this and something that
		if( m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj->m_pParentObject )
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync((DesignObj_Orbiter *) m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj->m_pParentObject);
	}

	m_pMouseBehavior->m_pMouseGovernor->SetBuffer(500);
	if( m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_iLastNotch=0;
		m_iCancelLevel = 50;
	}
	else
	{
		m_bTapAndRelease=false;
		m_iLastNotch=5;
	}

	int Y=m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2;
	m_iLastGoodPosition=Y;
	m_pMouseBehavior->SetMousePosition(m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2,Y);
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
		m_pMouseBehavior->Clear(true); // this will be deleted
	return false; // Keep processing
}

bool LightMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_2_CONST )
	{
		if( !m_bTapAndRelease )
		{
			DCE::CMD_Set_Level CMD_Set_Level(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(m_iCancelLevel));
			m_pMouseBehavior->m_pOrbiter->SendMessage(CMD_Set_Level.m_pMessage);
		}
		m_pMouseBehavior->Clear(true); // this will be deleted
		return false; // Keep processing
	}
	else if( PK_Button==BUTTON_Mouse_8_CONST && !m_bTapAndRelease )
	{
		m_pMouseBehavior->Clear(true); // this will be deleted
		return false; // Keep processing

	}
	return false; // Keep processing
}

void LightMouseHandler::Move(int X,int Y,int PK_Direction)
{
	m_iLastGoodPosition=Y;
	int NotchWidth = m_pObj->m_rPosition.Height/11; // Allow for 5 repeat levels in each direction
	int Notch = (Y - m_pObj->m_rPosition.Y - m_pObj->m_pPopupPoint.Y) * 11 / 
		(m_pObj->m_rPosition.Height + (m_pObj->m_rPosition.Height ? 1 : 0));
	if( Notch<0 )
		Notch=0;
	if( Notch>10 )
		Notch=10;

	if( m_bTapAndRelease )
		Notch = 5-Notch;  // Higher number at top
	else
		Notch = 10-Notch;  // Higher number at top

	if( Notch!=m_iLastNotch )
	{
		if( m_bTapAndRelease==false )
		{
			NeedToRender render( m_pMouseBehavior->m_pOrbiter, "LightMouseHandler" );
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj);
			DCE::CMD_Set_Level CMD_Set_Level(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_LightingPlugIn,StringUtils::itos(Notch*10));
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Level.m_pMessage);
			m_iLastNotch = Notch;
		}
		else
		{
			if( Notch==0 )
				m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
			else
				m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Light,Notch,500,this);
			m_iLastNotch = Notch;
		}
	}
}

bool LightMouseHandler::SlowDrift(int &X,int &Y)
{ 
	X = m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X + (m_pObj->m_rPosition.Width/2);
	Y = m_iLastGoodPosition;
	m_pMouseBehavior->SetMousePosition(X,Y);
	return true;
}

void LightMouseHandler::CustomRender()
{
	int CurrentLevel = m_bTapAndRelease ? m_iCancelLevel : m_iLastNotch*10;
	if( CurrentLevel > 100 )
		CurrentLevel=100;
	if( CurrentLevel < 0 )
		CurrentLevel = 0;
	CurrentLevel = m_pObj->m_rPosition.Height * CurrentLevel / 100;
	int X = m_pObj->m_rPosition.X + m_pObj->m_rPosition.Width -  m_pObj->m_rPosition.Width * .1;

    PrevSurfaceRestore();

    m_pMouseBehavior->m_pOrbiter->SolidRectangleAlpha(
		X,m_pObj->m_rPosition.Bottom()-CurrentLevel,
		m_pObj->m_rPosition.Width*.1,CurrentLevel,
		PlutoColor::Green(), 128);

	if( !m_bTapAndRelease )
		return;  // We only draw the square for tap and release

	int NotchHeight = m_pObj->m_rPosition.Height/11; // Allow for 7 repeat levels in each direction
	int NotchStart = (5-m_iLastNotch) * NotchHeight + m_pObj->m_rPosition.Y;

	X = m_pObj->m_rPosition.X + m_pObj->m_rPosition.Width * .1;

	m_pMouseBehavior->m_pOrbiter->SolidRectangleAlpha(
		X, NotchStart,
		m_pObj->m_rPosition.Width * .25, NotchHeight,
		PlutoColor::Blue(), 128);
	return;
}

void LightMouseHandler::DoIteration(int Parm)
{
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "LightMouseHandler it" );
	m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj);
	m_iCancelLevel += (Parm*10);
	if( m_iCancelLevel > 100 )
		m_iCancelLevel=100;
	if( m_iCancelLevel < 0 )
		m_iCancelLevel = 0;
	DCE::CMD_Set_Level CMD_Set_Level(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_LightingPlugIn,(Parm>0 ? "+" : "") + StringUtils::itos(Parm*10));
	m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Level.m_pMessage);
}
