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
#include "pluto_main/Define_Variable.h"
#include "DataGrid.h"
#include "ScreenHistory.h"

using namespace DCE;

HorizMenuMouseHandler::HorizMenuMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_pObj_ActiveMenuPad = NULL;
	m_pObj_ActiveSubMenu = NULL;
	DesignObj_Orbiter *pObj_First = NULL;
	if( m_pObj->m_iBaseObjectID==DESIGNOBJ_popMainMenu_CONST )
		pObj_First = m_pMouseBehavior->m_pOrbiter->FindObject(
			StringUtils::itos(atoi(m_pObj->m_ObjectID.c_str())) + ".0.0." + StringUtils::itos(DESIGNOBJ_butCurrentMedia_CONST));
	else if( m_pObj->m_iBaseObjectID==5093 )
		pObj_First = m_pMouseBehavior->m_pOrbiter->FindObject(
			StringUtils::itos(atoi(m_pObj->m_ObjectID.c_str())) + ".0.0." + StringUtils::itos(5130));
	
	if( pObj_First )
	{
		m_pMouseBehavior->m_pStartMovement.X=pObj_First->m_rPosition.X + pObj_First->m_pPopupPoint.X + (pObj_First->m_rPosition.Width/2);
		m_pMouseBehavior->m_pStartMovement.Y=pObj_First->m_rPosition.Y + pObj_First->m_pPopupPoint.Y + (pObj_First->m_rPosition.Height/2);
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y);
		PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
		m_pObj_ActiveMenuPad=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted=pObj_First;
		if( m_pObj_ActiveMenuPad )
			ShowPopup(m_pObj_ActiveMenuPad);
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
			ShowPopup(pObj_ToHighlight);
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

void HorizMenuMouseHandler::ShowPopup(DesignObj_Orbiter *pObj_MenuPad)
{
	pObj_MenuPad->m_GraphicToDisplay=GRAPHIC_SELECTED;
	string sSubMenu;

	if( m_pObj->m_iBaseObjectID==DESIGNOBJ_popMainMenu_CONST )
		sSubMenu = GetMainMenuPopup(pObj_MenuPad);
	else if( m_pObj->m_iBaseObjectID==5093 )
		sSubMenu = GetFileBrowserPopup(pObj_MenuPad);

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

string HorizMenuMouseHandler::GetFileBrowserPopup(DesignObj_Orbiter *pObj_MenuPad)
{
	switch( pObj_MenuPad->m_iBaseObjectID )
	{
	case DESIGNOBJ_butFBSF_Show_MediaType_CONST: // what
		return TOSTRING(DESIGNOBJ_popFBSF_MediaType_CONST);
	case DESIGNOBJ_butFBSF_Show_MediaPrivate_CONST:  // private
		return TOSTRING(DESIGNOBJ_popFBSF_PrivateMedia_CONST);
	case DESIGNOBJ_butFBSF_Show_MediaRating_CONST:  // rating
		return TOSTRING(DESIGNOBJ_popFBSF_RatingsByUser_CONST);
	case DESIGNOBJ_butFBSF_Show_Sort_CONST: // sort
		return TOSTRING(DESIGNOBJ_popFBSF_Sort_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaGenre_CONST: // gemre
		return TOSTRING(DESIGNOBJ_popFBSF_Genres_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaSubType_CONST: // sub type
		return TOSTRING(DESIGNOBJ_popFBSF_PK_MediaSubType_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaFormat_CONST: // format
		return TOSTRING(DESIGNOBJ_popFBSF_PK_FileFormat_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaSource_CONST: // source
		return TOSTRING(DESIGNOBJ_popFBSF_MediaSource_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_Media_Go_CONST: // go
		return TOSTRING(DESIGNOBJ_popFBSF_Go_CONST);
	}

	return "";
}

string HorizMenuMouseHandler::GetMainMenuPopup(DesignObj_Orbiter *pObj_MenuPad)
{
	switch( pObj_MenuPad->m_iBaseObjectID )
	{
	case DESIGNOBJ_butCurrentMedia_CONST:
		return StringUtils::itos(m_pMouseBehavior->m_pOrbiter->m_iPK_DesignObj_Remote_Popup);
	case DESIGNOBJ_butClimate2_CONST:  // climate
		return "4957.<%=L:0%>.0";
	case DESIGNOBJ_butTelecom2_CONST:  // telecom
		return "4958.<%=L:0%>.0";//"#DESIGNOBJ_icoModeBabySitter_CONST.<%=L:0%>";// "4957.<%=L:0%>";  //
	case DESIGNOBJ_butSecurity2_CONST: // security
		return "4959.<%=L:0%>.0";
	case DESIGNOBJ_butLightsPopup_CONST: // lights
		return "4889.<%=L:0%>.0";
	case DESIGNOBJ_butMediaPopup_CONST:
		return "4870.<%=L:0%>.0";
	case DESIGNOBJ_butCurrentLocation_CONST:
		return "4894";
	case DESIGNOBJ_butFloorplans_CONST:
		return TOSTRING(DESIGNOBJ_popFloorplan_CONST);
	case DESIGNOBJ_butSettings_CONST:
		return TOSTRING(DESIGNOBJ_popAdvanced_CONST);
	}

	return "";
}

