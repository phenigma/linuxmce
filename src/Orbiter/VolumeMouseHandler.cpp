#include "VolumeMouseHandler.h"
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

VolumeMouseHandler::VolumeMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
        : MouseHandler(pObj,sOptions,pMouseBehavior)
{
}

VolumeMouseHandler::~VolumeMouseHandler()
{
}

void VolumeMouseHandler::Start()
{
	PlutoRectangle rect(m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X,
		m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y + int(m_pObj->m_rPosition.Height*.2),
		m_pObj->m_rPosition.Width,1);
	m_pMouseBehavior->ConstrainMouse(rect);
	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_LeftRight);

	NeedToRender render( g_pOrbiter, "start volume" );
	g_pOrbiter->RenderObjectAsync(m_pObj);
g_pOrbiter->m_bPK_Device_NowPlaying_Audio_DiscreteVolume=true;
	if( g_pOrbiter->m_bPK_Device_NowPlaying_Audio_DiscreteVolume==false || m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_pObj->m_GraphicToDisplay=GRAPHIC_NORMAL;
		int X=m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2;
		m_iLastGoodPosition=X;
		m_pMouseBehavior->SetMousePosition(X,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
		m_iLastNotch = 0;
		m_iCancelLevel = 50;
	}
	else
	{
		m_pMouseBehavior->m_pMouseGovernor->SetBuffer(250);
		m_bTapAndRelease=false;
		m_pObj->m_GraphicToDisplay=1;
		m_iCancelLevel = m_iLastNotch = atoi(g_pOrbiter->GetEvents()->GetDeviceDataFromDatabase(g_pOrbiter->m_dwPK_Device_NowPlaying_Audio,DEVICEDATA_Volume_Level_CONST).c_str());
		int X = m_pObj->m_rPosition.Width * m_iLastNotch / 100;
		m_iLastGoodPosition=X;
		m_pMouseBehavior->SetMousePosition(m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+X,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
	}
}

void VolumeMouseHandler::Stop()
{
NeedToRender render( g_pOrbiter, "change to discrete volume" );
PLUTO_SAFETY_LOCK(nd,g_pOrbiter->m_NeedRedrawVarMutex);
g_pOrbiter->m_vectObjs_NeedRedraw.push_back(m_pObj);
g_pPlutoLogger->Write(LV_CORPCLIENT,"m_pObj.disabled = true");
}

bool VolumeMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_8_CONST )
		m_pMouseBehavior->Clear(true); // this will be deleted
	return false; // Keep processing
}

bool VolumeMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_2_CONST )
	{
		if( !m_bTapAndRelease )
		{
			DCE::CMD_Set_Volume CMD_Set_Volume(g_pOrbiter->m_dwPK_Device,g_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(m_iCancelLevel));
			g_pOrbiter->SendMessage(CMD_Set_Volume.m_pMessage);
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

void VolumeMouseHandler::Move(int X,int Y,int PK_Direction)
{
	m_iLastGoodPosition=X;
	if( m_bTapAndRelease )
	{
		int NotchWidth = m_pObj->m_rPosition.Width/15; // Allow for 7 repeat levels in each direction
		int Notch = (X-m_pObj->m_rPosition.X-m_pObj->m_pPopupPoint.X)/NotchWidth - 7;
		if( Notch<-7 )
			Notch=-7;
		if( Notch>7 )
			Notch=7;
		if( Notch!=m_iLastNotch )
		{
			NeedToRender render( g_pOrbiter, "start volume" );
			g_pOrbiter->RenderObjectAsync(m_pObj);
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Volume,Notch,500,this);
			m_iLastNotch = Notch;
		}
	}
	else
	{
		int NotchWidth = m_pObj->m_rPosition.Width/100; // Allow for 7 repeat levels in each direction
		int Notch = (X - m_pObj->m_rPosition.X - m_pObj->m_pPopupPoint.X) * 100 / m_pObj->m_rPosition.Width;
		if( Notch<0 )
			Notch=0;
		if( Notch>100 )
			Notch=100;
		if( Notch!=m_iLastNotch )
		{
			NeedToRender render( g_pOrbiter, "start volume" );
			g_pOrbiter->RenderObjectAsync(m_pObj);
			DCE::CMD_Set_Volume CMD_Set_Volume(g_pOrbiter->m_dwPK_Device,g_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(Notch));
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Volume.m_pMessage);
			m_iLastNotch = Notch;
		}
	}
}

bool VolumeMouseHandler::SlowDrift(int &X,int &Y)
{
	X = m_iLastGoodPosition;
	Y = m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y + (m_pObj->m_rPosition.Height/2);
	m_pMouseBehavior->SetMousePosition(X,Y);
	return true;
}

void VolumeMouseHandler::CustomRender()
{
	int CurrentLevel = m_bTapAndRelease ? m_iCancelLevel : m_iLastNotch;
	if( CurrentLevel > 100 )
		CurrentLevel=100;
	if( CurrentLevel < 0 )
		CurrentLevel = 0;
	CurrentLevel = m_pObj->m_rPosition.Width * CurrentLevel / 100;
	int Y = int(m_pObj->m_rPosition.Y + m_pObj->m_rPosition.Height * .1);

    g_pRenderer->SolidRectangle(
		m_pObj->m_rPosition.X, Y,
		CurrentLevel, int(m_pObj->m_rPosition.Height * .1),
		PlutoColor::White().SetAlpha(128));
}

void VolumeMouseHandler::DoIteration(int Parm)
{
	NeedToRender render( g_pOrbiter, "start volume" );
	g_pOrbiter->RenderObjectAsync(m_pObj);
	m_iCancelLevel += Parm;
	if( m_iCancelLevel > 100 )
		m_iCancelLevel=100;
	if( m_iCancelLevel < 0 )
		m_iCancelLevel = 0;
	if( Parm<0 )
	{
		DCE::CMD_Vol_Down CMD_Vol_Down(g_pOrbiter->m_dwPK_Device,g_pOrbiter->m_dwPK_Device_NowPlaying_Audio,Parm*-1);
		m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Vol_Down.m_pMessage);
	}
	else if( Parm>0 )
	{
		DCE::CMD_Vol_Up CMD_Vol_Up(g_pOrbiter->m_dwPK_Device,g_pOrbiter->m_dwPK_Device_NowPlaying_Audio,Parm);
		m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Vol_Up.m_pMessage);
	}
}
