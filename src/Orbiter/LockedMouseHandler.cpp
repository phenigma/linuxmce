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

LockedMouseHandler::LockedMouseHandler(DesignObj_Orbiter *pObj,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,pMouseBehavior)
{
	m_pObj_Highlighted = NULL;
	m_bFirstTime=true;
	if( m_pObj->m_iBaseObjectID==DESIGNOBJ_popMainMenu_CONST )
	{
		DesignObj_Orbiter *pObj_First = m_pMouseBehavior->m_pOrbiter->FindObject(
			StringUtils::itos(DESIGNOBJ_popMainMenu_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_butCurrentMedia_CONST));
		if( pObj_First )
		{
			m_pMouseBehavior->m_pStartMovement.X=pObj_First->m_rPosition.X + pObj_First->m_pPopupPoint.X + (pObj_First->m_rPosition.Width/2);
			m_pMouseBehavior->m_pStartMovement.Y=pObj_First->m_rPosition.Y + pObj_First->m_pPopupPoint.Y + (pObj_First->m_rPosition.Height/2);
			m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y);
			PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
			if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted!=pObj_First )
				m_pMouseBehavior->m_pOrbiter->ExecuteCommandsInList( &m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_Action_UnhighlightList, m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted, smHighlight, 0, 0 );
			m_pObj_Highlighted=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted=pObj_First;
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted);
		}
	}
}

void LockedMouseHandler::Start()
{
	if( m_bFirstTime )
		m_bFirstTime=false;
	else if( m_pObj_Highlighted )
	{
		m_pMouseBehavior->m_pStartMovement.X=m_pObj_Highlighted->m_rPosition.X + m_pObj_Highlighted->m_pPopupPoint.X + (m_pObj_Highlighted->m_rPosition.Width/2);
		m_pMouseBehavior->m_pStartMovement.Y=m_pObj_Highlighted->m_rPosition.Y + m_pObj_Highlighted->m_pPopupPoint.Y + (m_pObj_Highlighted->m_rPosition.Height/2);
		m_pMouseBehavior->SetMousePosition(m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y);
	}

	if( m_pMouseBehavior->m_iTime_Last_Mouse_Up )
		m_bTapAndRelease=true;
	else
		m_bTapAndRelease=false;
}

void LockedMouseHandler::Stop()
{
}

bool LockedMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST )
		m_pMouseBehavior->m_pOrbiter->CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), "");

	return false; // Keep processing
}

bool LockedMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_7_CONST && !m_bTapAndRelease )
	{
		PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
		if(  m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->IsHidden(  )  )
			m_pMouseBehavior->m_pOrbiter->SelectedObject( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted, smNavigation );
		else
			m_pMouseBehavior->Clear();   // ** WARNING -- this will be deleted exit immediately
		return false;
	}

	return false; // Keep processing
}

void LockedMouseHandler::Move(int X,int Y)
{
//	g_pPlutoLogger->Write(LV_FESTIVAL,"Move %d,%d last %d,%d start %d,%d locked axis: %d current %d pos: %d",
//X,Y,m_pMouseBehavior->m_pSamples[0].X,m_pMouseBehavior->m_pSamples[0].Y,m_pMouseBehavior->m_pStartMovement.X,m_pMouseBehavior->m_pStartMovement.Y,(int) m_pMouseBehavior->m_cLockedAxes,(int) m_pMouseBehavior->m_cLocked_Axis_Current,(int) m_pMouseBehavior->m_iLockedPosition);

	if( m_pMouseBehavior->m_cLocked_Axis_Current == AXIS_LOCK_X && m_pMouseBehavior->m_pObj_Locked_Horizontal )
	{
		if( X < m_pMouseBehavior->m_pObj_Locked_Horizontal->m_rPosition.X+m_pMouseBehavior->m_pObj_Locked_Horizontal->m_pPopupPoint.X )
		{
			X = m_pMouseBehavior->m_pObj_Locked_Horizontal->m_rPosition.X+m_pMouseBehavior->m_pObj_Locked_Horizontal->m_pPopupPoint.X;
			m_pMouseBehavior->SetMousePosition(X,Y);
		}
		if( X > m_pMouseBehavior->m_pObj_Locked_Horizontal->m_rPosition.Right()+m_pMouseBehavior->m_pObj_Locked_Horizontal->m_pPopupPoint.X )
		{
			X = m_pMouseBehavior->m_pObj_Locked_Horizontal->m_rPosition.Right()+m_pMouseBehavior->m_pObj_Locked_Horizontal->m_pPopupPoint.X;
			m_pMouseBehavior->SetMousePosition(X,Y);
		}
	}
	else if( m_pMouseBehavior->m_cLocked_Axis_Current == AXIS_LOCK_Y && m_pMouseBehavior->m_pObj_Locked_Vertical )
	{
		if( Y < m_pMouseBehavior->m_pObj_Locked_Vertical->m_rPosition.Y+m_pMouseBehavior->m_pObj_Locked_Vertical->m_pPopupPoint.Y )
		{
			X = m_pMouseBehavior->m_pObj_Locked_Vertical->m_rPosition.Y+m_pMouseBehavior->m_pObj_Locked_Vertical->m_pPopupPoint.Y;
			m_pMouseBehavior->SetMousePosition(X,Y);
		}
		if( X > m_pMouseBehavior->m_pObj_Locked_Vertical->m_rPosition.Bottom()+m_pMouseBehavior->m_pObj_Locked_Vertical->m_pPopupPoint.Y )
		{
			X = m_pMouseBehavior->m_pObj_Locked_Vertical->m_rPosition.Bottom()+m_pMouseBehavior->m_pObj_Locked_Vertical->m_pPopupPoint.Y;
			m_pMouseBehavior->SetMousePosition(X,Y);
		}
	}

	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Protect the highlighed object
//	if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted && 
//		( (m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition+m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint).Contains(X,Y)==false || m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ChildObjects.size()) )
	if( !m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted || 
		(m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_rPosition+m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_pPopupPoint).Contains(X,Y)==false )
	{
		DesignObj_Orbiter *pObj_ToHighlight=m_pMouseBehavior->FindChildObjectAtPosition(m_pMouseBehavior->m_cLocked_Axis_Current == AXIS_LOCK_X ? m_pMouseBehavior->m_pObj_Locked_Horizontal : m_pMouseBehavior->m_pObj_Locked_Vertical,X,Y);
		// The user has moved off the highlighted object.  Find the object under here to highlight

g_pPlutoLogger->Write(LV_CRITICAL,"change hl doesn't contain %d,%d h:%p v:%p old obj to hl: %s, new: %s",
X,Y,m_pMouseBehavior->m_pObj_Locked_Horizontal,m_pMouseBehavior->m_pObj_Locked_Vertical,
m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted ? m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_ObjectID.c_str() : "NONE",
pObj_ToHighlight ? pObj_ToHighlight->m_ObjectID.c_str() : "NONE");
		if( pObj_ToHighlight && pObj_ToHighlight!=m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
		{
			m_pMouseBehavior->m_pOrbiter->UnHighlightObject();
			if( m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted )
				m_pMouseBehavior->m_pOrbiter->ExecuteCommandsInList( &m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted->m_Action_UnhighlightList, m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted, smHighlight, 0, 0 );
			m_pObj_Highlighted = m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted = pObj_ToHighlight;
			m_pMouseBehavior->m_pOrbiter->DoHighlightObject();
		}
	}

}
