#include "HorizMenuMouseHandler.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "OrbiterRenderer.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Screen.h"
#include "DataGrid.h"
#include "ScreenHistory.h"

using namespace DCE;

HorizMenuMouseHandler::HorizMenuMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_pObj_ActiveMenuPad = NULL;
	m_pObj_ActiveSubMenu = NULL;
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
			m_pObj_ActiveMenuPad=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted=pObj_First;
			if( m_pObj_ActiveMenuPad )
				ShowMainMenuPopup(m_pObj_ActiveMenuPad);
		}
	}
}

void HorizMenuMouseHandler::Start()
{
}

void HorizMenuMouseHandler::Stop()
{
}

bool HorizMenuMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST )
	{
		m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), "");
		return true; // Don't process any more
	}

	return false; // Keep processing
}

bool HorizMenuMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_7_CONST && m_bTapAndRelease==false && m_bStartedMovement )
	{
//		return false;
	}

	return false; // Keep processing
}

void HorizMenuMouseHandler::Move(int X,int Y,int PK_Direction)
{
	if( !m_pObj_ActiveMenuPad )
		return;  // Shouldn't happen
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
	if(	X<m_pObj_ActiveMenuPad->m_rPosition.X ||
		X>m_pObj_ActiveMenuPad->m_rPosition.Right() )
	{
		DesignObj_Orbiter *pObj_ToHighlight=m_pMouseBehavior->FindChildObjectAtPosition(m_pMouseBehavior->m_pObj_Locked,X,-1);
		// The user has moved off the highlighted object.  Find the object under here to highlight

		if( pObj_ToHighlight && pObj_ToHighlight!=m_pObj_ActiveMenuPad )
		{
			m_pObj_ActiveMenuPad->m_GraphicToDisplay=GRAPHIC_NORMAL;
			if( m_pObj->m_iBaseObjectID==DESIGNOBJ_popMainMenu_CONST )
				ShowMainMenuPopup(pObj_ToHighlight);
			else
				m_pMouseBehavior->m_pOrbiter->SelectedObject( pObj_ToHighlight, smNavigation );
			m_pObj_ActiveMenuPad=pObj_ToHighlight;
		}
	}
	if( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted || m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition.Contains(X,Y)==false )
	{
		if( m_pObj_ActiveSubMenu )
		{
			DesignObj_Orbiter *pObj_ToHighlight=m_pMouseBehavior->FindChildObjectAtPosition(m_pObj_ActiveSubMenu,X,Y);
			if( pObj_ToHighlight )
				m_pMouseBehavior->HighlightObject(pObj_ToHighlight);
		}
	}
}

void HorizMenuMouseHandler::ShowMainMenuPopup(DesignObj_Orbiter *pObj_MenuPad)
{
	pObj_MenuPad->m_GraphicToDisplay=GRAPHIC_SELECTED;
	string sSubMenu;
	switch( pObj_MenuPad->m_iBaseObjectID )
	{
	case DESIGNOBJ_butCurrentMedia_CONST:
		sSubMenu = StringUtils::itos(m_pMouseBehavior->m_pOrbiter->m_iPK_DesignObj_Remote_Popup);
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
	case DESIGNOBJ_butLightsPopup_CONST: // lights
		sSubMenu = "4889.<%=L:0%>.0";
		break;
	case DESIGNOBJ_butMediaPopup_CONST:
		sSubMenu = "4870.<%=L:0%>.0";
		break;
	case DESIGNOBJ_butCurrentLocation_CONST:
		sSubMenu = "4894";
		break;
	case DESIGNOBJ_butFloorplans_CONST:
		sSubMenu = "4960";
		break;
	case DESIGNOBJ_butSettings_CONST:
		sSubMenu = "4961";
		break;
	}

	m_pObj_ActiveSubMenu = m_pMouseBehavior->m_pOrbiter->FindObject(sSubMenu);
	if( !m_pObj_ActiveSubMenu )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"LockedMouseHandler::ActivatedMainMenuPad cannot find %s",sSubMenu.c_str());
		return;
	}

	// We want to put it above us, left justified
	PlutoPoint pt = pObj_MenuPad->m_rPosition.Location();
	pt.Y -= m_pObj_ActiveSubMenu->m_rPosition.Height;
	if( pt.X + m_pObj_ActiveSubMenu->m_rPosition.Width > m_pMouseBehavior->m_pOrbiter->m_Width )
		pt.X = m_pMouseBehavior->m_pOrbiter->m_Width-m_pObj_ActiveSubMenu->m_rPosition.Width;

	m_pMouseBehavior->m_pOrbiter->CMD_Show_Popup(m_pObj_ActiveSubMenu->m_ObjectID,pt.X,pt.Y,"","submenu",false,false);
}

