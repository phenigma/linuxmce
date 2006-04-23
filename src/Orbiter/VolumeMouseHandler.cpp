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

void VolumeMouseHandler::Start()
{
m_pMouseBehavior->m_pOrbiter->m_bPK_Device_NowPlaying_Audio_DiscreteVolume=true;

	if( m_pMouseBehavior->m_pMouseHandler_Vertical && m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj )
	{
		NeedToRender render( m_pMouseBehavior->m_pOrbiter, "start volume" );
		m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_bHidden = true;
		m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj);// Redraw even if the object was already in this state,  because maybe we're hiding this and something that
		if( m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_pParentObject )
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync((DesignObj_Orbiter *) m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_pParentObject);
	}

	if( m_pMouseBehavior->m_pOrbiter->m_bPK_Device_NowPlaying_Audio_DiscreteVolume==false || m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_pObj->m_GraphicToDisplay=GRAPHIC_NORMAL;
		m_pMouseBehavior->SetMousePosition(m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+m_pObj->m_rPosition.Width/2,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
		m_iLastNotch = 0;
	}
	else
	{
		m_pMouseBehavior->m_pMouseGovernor->SetBuffer(2000);
		m_bTapAndRelease=false;
		m_pObj->m_GraphicToDisplay=1;
		m_iCancelLevel = m_iLastNotch = atoi(m_pMouseBehavior->m_pOrbiter->GetEvents()->GetDeviceDataFromDatabase(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,DEVICEDATA_Volume_Level_CONST).c_str());
		int X = m_pObj->m_rPosition.Width * m_iLastNotch / 100;
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "change to discrete volume" );
PLUTO_SAFETY_LOCK(nd,m_pMouseBehavior->m_pOrbiter->m_NeedRedrawVarMutex);
m_pMouseBehavior->m_pOrbiter->m_vectObjs_NeedRedraw.push_back(m_pObj);
g_pPlutoLogger->Write(LV_FESTIVAL,"Starting discrete volume control notch: %d  X %d",m_iLastNotch,X);
		m_pMouseBehavior->SetMousePosition(m_pObj->m_rPosition.X+m_pObj->m_pPopupPoint.X+X,m_pObj->m_rPosition.Y+m_pObj->m_pPopupPoint.Y+m_pObj->m_rPosition.Height/2);
	}
	DrawSquare(m_iLastNotch,PlutoColor::White());
}

void VolumeMouseHandler::Stop()
{
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "change to discrete volume" );
PLUTO_SAFETY_LOCK(nd,m_pMouseBehavior->m_pOrbiter->m_NeedRedrawVarMutex);
m_pMouseBehavior->m_pOrbiter->m_vectObjs_NeedRedraw.push_back(m_pObj);
g_pPlutoLogger->Write(LV_CORPCLIENT,"m_pObj.disabled = true");
}

bool VolumeMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_8_CONST )
		m_pMouseBehavior->Clear(); // this will be deleted
	return false; // Keep processing
}

bool VolumeMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_2_CONST )
	{
		if( !m_bTapAndRelease )
		{
			DCE::CMD_Set_Volume CMD_Set_Volume(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(m_iCancelLevel));
			m_pMouseBehavior->m_pOrbiter->SendMessage(CMD_Set_Volume.m_pMessage);
		}
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

void VolumeMouseHandler::Move(int X,int Y)
{
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
			DrawSquare(m_iLastNotch,PlutoColor::Blue());
			m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_Volume,Notch,500,this);
			m_iLastNotch = Notch;
			DrawSquare(m_iLastNotch,PlutoColor::White());
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
g_pPlutoLogger->Write(LV_FESTIVAL,"Setting volume to : %d  X %d",Notch,X);
			DrawSquare(m_iLastNotch,PlutoColor::Blue());
			DCE::CMD_Set_Volume CMD_Set_Volume(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying_Audio,StringUtils::itos(Notch));
			m_pMouseBehavior->m_pMouseGovernor->SendMessage(CMD_Set_Volume.m_pMessage);
			m_iLastNotch = Notch;
			DrawSquare(m_iLastNotch,PlutoColor::White());
		}
	}
}

void VolumeMouseHandler::DrawSquare(int Notch,PlutoColor plutoColor)
{
	int NotchWidth = m_bTapAndRelease==false ? m_pObj->m_rPosition.Width/100 : m_pObj->m_rPosition.Width/15; // Allow for 7 repeat levels in each direction
	int NotchStart = (7 + Notch) * NotchWidth + m_pObj->m_rPosition.X + m_pObj->m_pPopupPoint.X;
	m_pMouseBehavior->m_pOrbiter->HollowRectangle(
		NotchStart, m_pObj->m_rPosition.Y + m_pObj->m_pPopupPoint.Y,
		NotchWidth, m_pObj->m_rPosition.Height,
		plutoColor);
	m_pMouseBehavior->m_pOrbiter->UpdateRect(m_pObj->m_rPosition + m_pObj->m_pPopupPoint);
}
