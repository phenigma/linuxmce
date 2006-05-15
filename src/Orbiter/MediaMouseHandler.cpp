#include "MediaMouseHandler.h"
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

MediaMouseHandler::MediaMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior)
	: MouseHandler(pObj,sOptions,pMouseBehavior)
{
	m_spDatagridMouseHandlerHelper.reset(new DatagridMouseHandlerHelper(this));
}

void MediaMouseHandler::Start()
{
	if( !m_pObj || m_pObj->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
		return; // Shouldn't happen, this should be the volume control

	DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) m_pObj;

	m_pMouseBehavior->m_pMouseGovernor->SetBuffer(2000);

	m_pMouseBehavior->SetMouseCursorStyle(MouseBehavior::mcs_UpDown);

	if( m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_pMouseBehavior->SetMousePosition(pObj_Grid->m_rPosition.X + pObj_Grid->m_pPopupPoint.X + (pObj_Grid->m_rPosition.Width/2),
			pObj_Grid->m_rPosition.Y + pObj_Grid->m_pPopupPoint.Y + (pObj_Grid->m_rPosition.Height/2));
		m_spDatagridMouseHandlerHelper->Start(pObj_Grid);

	}
	else
	{
		m_bTapAndRelease=false;
		if( pObj_Grid->m_pDataGridTable )
		{
			int Rows = pObj_Grid->m_pDataGridTable->GetRows();
			int CurrentRow = pObj_Grid->m_iHighlightedRow>0 ? pObj_Grid->m_iHighlightedRow : 0;
			double RowHeight = (double) pObj_Grid->m_rPosition.Height / Rows;
			int Y = int(CurrentRow * RowHeight + pObj_Grid->m_rPosition.Y + pObj_Grid->m_pPopupPoint.Y);
			int X = pObj_Grid->m_rPosition.X + pObj_Grid->m_pPopupPoint.X + (pObj_Grid->m_rPosition.Width/2);
			m_pMouseBehavior->SetMousePosition(X,Y);
		}
	}
	m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted=pObj_Grid;
}

void MediaMouseHandler::Stop()
{
	m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted=NULL;
}

bool MediaMouseHandler::ButtonDown(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_6_CONST || PK_Button==BUTTON_Mouse_2_CONST )
	{
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000,false);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);

		if( PK_Button==BUTTON_Mouse_1_CONST )
			m_pMouseBehavior->m_pOrbiter->SelectedObject(m_pObj,smNavigation);

		m_pMouseBehavior->Clear(true);
		return false; // this is now invalid
	}
	return false; // Keep processing
}

bool MediaMouseHandler::ButtonUp(int PK_Button)
{
	if( PK_Button==BUTTON_Mouse_6_CONST && !m_bTapAndRelease )  // The user was in press and hold mode, or tapped again after the menu appeared on screen
	{
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000,false);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
		m_pMouseBehavior->m_pOrbiter->SelectedObject(m_pObj,smNavigation);
		m_pMouseBehavior->Clear(true);
		return false; // this is now invalid
	}
	return false; // Keep processing
}

void MediaMouseHandler::Move(int X,int Y,int PK_Direction)
{
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Always lock this before datagrid to prevent a deadlock
	PLUTO_SAFETY_LOCK( dng, m_pMouseBehavior->m_pOrbiter->m_DatagridMutex );
	if( !m_pObj || m_pObj->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
		return; // Also shouldn't happen
	DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) m_pObj;
	if( !pObj_Grid->m_pDataGridTable )
		return; // Again shouldn't happen

	int Rows = pObj_Grid->m_pDataGridTable->GetRows();
	if( m_bTapAndRelease==false )  // Discrete positioning
	{
		double RowHeight = (double) pObj_Grid->m_rPosition.Height / Rows;
		int YDiff = Y - pObj_Grid->m_rPosition.Y - pObj_Grid->m_pPopupPoint.Y;
		int Row = int(YDiff / RowHeight);
		if( Row>Rows-1 )
			Row=Rows-1;
		pObj_Grid->m_iHighlightedRow = 1;
		pObj_Grid->m_GridCurRow = Row;
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::MediaTracks  *discrete* highlighted row %d",Row);
		m_pMouseBehavior->m_pOrbiter->SelectedObject(pObj_Grid,smMouseGovernor);
		delete pObj_Grid->m_pDataGridTable;
		pObj_Grid->m_pDataGridTable=NULL;
		pObj_Grid->bReAcquire=true;
		NeedToRender render( m_pMouseBehavior->m_pOrbiter, "MOUSE BEHAVIOR SCROLL" );
		m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(pObj_Grid);
	}
	else
		m_spDatagridMouseHandlerHelper->StayInGrid(PK_Direction,X,Y);
}
