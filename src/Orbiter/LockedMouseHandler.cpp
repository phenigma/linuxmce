#include "LockedMouseHandler.h"
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
#include "DataGrid.h"

using namespace DCE;

LockedMouseHandler::LockedMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_pObj_Highlighted = NULL;
	m_bFirstTime=true;
	m_bActivatedObject = false;
	if( m_pObj->m_iBaseObjectID==DESIGNOBJ_popMainMenu_CONST )
	{
		DesignObj_Orbiter *pObj_First = m_pMouseBehavior->m_pOrbiter->FindObject(
			StringUtils::itos(atoi(m_pObj->m_ObjectID.c_str())) + ".0.0." + StringUtils::itos(DESIGNOBJ_butCurrentMedia_CONST));
		if( pObj_First )
		{
			m_pMouseBehavior->m_pStartMovement.X=pObj_First->m_rPosition.X + pObj_First->m_pPopupPoint.X + (pObj_First->m_rPosition.Width/2);
			m_pMouseBehavior->m_pStartMovement.Y=pObj_First->m_rPosition.Y + pObj_First->m_pPopupPoint.Y + (pObj_First->m_rPosition.Height/2);
			m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y);
			PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
			m_pObj_Highlighted=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted=pObj_First;
			m_pMouseBehavior->HighlightObject(m_pObj_Highlighted);
		}
	}
}

void LockedMouseHandler::Start()
{
	m_PK_Direction_Last=0;
	m_bActivatedObject = false;
	if( m_bFirstTime )
		m_bFirstTime=false;
	else if( m_pObj_Highlighted )
	{
		m_pMouseBehavior->m_pStartMovement.X=m_pObj_Highlighted->m_rPosition.X + m_pObj_Highlighted->m_pPopupPoint.X + (m_pObj_Highlighted->m_rPosition.Width/2);
		m_pMouseBehavior->m_pStartMovement.Y=m_pObj_Highlighted->m_rPosition.Y + m_pObj_Highlighted->m_pPopupPoint.Y + (m_pObj_Highlighted->m_rPosition.Height/2);
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y);
	}

	if( m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		if( m_sOptions[0]=='M' )
			m_pMouseBehavior->m_bMouseHandler_Horizontal_Exclusive=true;
		m_bTapAndRelease=true;
	}
	else
		m_bTapAndRelease=false;
}

void LockedMouseHandler::Stop()
{
}

bool LockedMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST )
	{
		if( m_sOptions[0]=='M' )
		{
			ActivatedMainMenuPad();
			return true; // Don't process any more
		}
		else
		{
			m_bActivatedObject = true;
			m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), "");
			return true; // Don't process any more
		}
	}

	return false; // Keep processing
}

bool LockedMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_7_CONST && m_pMouseBehavior->m_iTime_Last_Mouse_Up==0 )
	{
		PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
		if( m_sOptions[0]=='M' )
			ActivatedMainMenuPad();
		else if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->IsHidden(  )  )
		{
			m_bActivatedObject = true;
			m_pMouseBehavior->m_pOrbiter->SelectedObject( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted, smNavigation );
		}
		else
			m_pMouseBehavior->Clear(true);   // ** WARNING -- this will be deleted exit immediately
		return false;
	}

	return false; // Keep processing
}

void LockedMouseHandler::Move(int X,int Y,int PK_Direction)
{
	if( m_PK_Direction_Last && PK_Direction!=m_PK_Direction_Last && m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		// The user is reversing direction.  Recenter so we don't switch back to easily, to give that 'pop' feel to the button
		int X = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.X + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint.X + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Width/2;
		int Y = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Y + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint.Y + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Height/2;
		m_pMouseBehavior->SetMousePosition(X,Y);
		m_PK_Direction_Last=PK_Direction;
		return;
	}
	m_PK_Direction_Last=PK_Direction;
	//	g_pPlutoLogger->Write(LV_FESTIVAL,"Move %d,%d last %d,%d start %d,%d locked axis: %d current %d pos: %d",
//X,Y,m_pMouseBehavior->m_pSamples[0].X,m_pMouseBehavior->m_pSamples[0].Y,m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y,(int) m_pMouseBehavior->m_cLockedAxes,(int) m_pMouseBehavior->m_cLocked_Axis_Current,(int) m_pMouseBehavior->m_iLockedPosition);

	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
//	if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && 
//		( (m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition+m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint).Contains(X,Y)==false || m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ChildObjects.size()) )
	if( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted || 
		(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition+m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint).Contains(X,Y)==false )
	{
		DesignObj_Orbiter *pObj_ToHighlight=m_pMouseBehavior->FindChildObjectAtPosition(m_pMouseBehavior->m_cLocked_Axis_Current == AXIS_LOCK_X ? m_pMouseBehavior->m_pObj_Locked_Horizontal : m_pMouseBehavior->m_pObj_Locked_Vertical,X,Y);
		// The user has moved off the highlighted object.  Find the object under here to highlight

		if( pObj_ToHighlight && pObj_ToHighlight!=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
			m_pObj_Highlighted = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted = pObj_ToHighlight;
			m_pMouseBehavior->HighlightObject(m_pObj_Highlighted);
		}
	}
}

void LockedMouseHandler::Notch(int PK_Direction,int iRepeat)
{
	return;
	if( m_pObj->m_iBaseObjectID!=DESIGNOBJ_popMainMenu_CONST )
		return; 
NeedToRender render( m_pMouseBehavior->m_pOrbiter, "LockedMouseHandler::Notch" );
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
	for(int i=0;i<iRepeat;++i)
		m_pMouseBehavior->m_pOrbiter->HighlightNextObject(PK_Direction);
	m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y); // So we don't hit limits too fast
}

void LockedMouseHandler::ActivatedMainMenuPad()
{
	if( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		return;

	string sSubMenu;
	switch( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_iBaseObjectID )
	{
	case 4865:
		sSubMenu = "<%=NP_R%>";
		break;
	case 4954:  // climate
		sSubMenu = "4957.<%=L:0%>.0";
		break;
	case 4955:  // telecom
		sSubMenu = "4958.<%=L:0%>.0";//"#DESIGNOBJ_icoModeBabySitter_CONST.<%=L:0%>";// "4957.<%=L:0%>";  //
		break;
	case 4956: // security
		sSubMenu = "4959.<%=L:0%>.0";
		break;
	case 4890: // lights
		sSubMenu = "4889.<%=L:0%>.0";
		break;
	case 4873:
		sSubMenu = "4870.<%=L:0%>.0";
		break;
	case 4867:
		sSubMenu = "4894";
		break;
	case 4952:
		sSubMenu = "4960";
		break;
	case 4953:
		sSubMenu = "4961";
		break;
	}

	DesignObj_Orbiter *pObj = m_pMouseBehavior->m_pOrbiter->FindObject(sSubMenu);
	if( !pObj )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"LockedMouseHandler::ActivatedMainMenuPad cannot find %s",sSubMenu.c_str());
		return;
	}

	// We want to put it above us, left justified
	PlutoPoint pt = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Location();
	pt.Y -= pObj->m_rPosition.Height;
	if( pt.X + pObj->m_rPosition.Width > m_pMouseBehavior->m_pOrbiter->m_Width )
		pt.X = m_pMouseBehavior->m_pOrbiter->m_Width-pObj->m_rPosition.Width;

	m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(pObj->m_ObjectID,pt.X,pt.Y,"","submenu",false,false);
	m_pMouseBehavior->Set_Mouse_Behavior("LS",true,"Y",pObj->m_ObjectID);
}

bool LockedMouseHandler::SlowDrift(int &X,int &Y)
{
	if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
	{
		X = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.X + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint.X + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Width/2;
		Y = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Y + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint.Y + m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Height/2;
		m_pMouseBehavior->SetMousePosition(X,Y);
		return true;
	}
	return false;
}

