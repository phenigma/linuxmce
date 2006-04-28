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

MediaMouseHandler::MediaMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior) : MouseHandler(pObj,sOptions,pMouseBehavior)
{
	/*
	// TODO - Temporary nasty hack so we don't have popups because they cause the whole screen to refresh and flicker
	pObj->m_ChildObjects.clear();
	DesignObj_Orbiter *pObj_Child = m_pMouseBehavior->m_pOrbiter->FindObject("<%=NP_PR%>");
	
	VectDesignObj_Orbiter vectDesignObj_Orbiter_OnScreen;
	m_pMouseBehavior->m_pOrbiter->ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, pObj_Child, NULL );

	if( pObj_Child )
		pObj->m_ChildObjects.push_back(pObj_Child);
		*/
}

void MediaMouseHandler::Start()
{
	if( m_pMouseBehavior->m_pMouseHandler_Horizontal && m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj )
	{
		NeedToRender render( m_pMouseBehavior->m_pOrbiter, "start media" );
		m_pMouseBehavior->m_bMouseHandler_Vertical_Exclusive = true;
		m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj->m_bHidden = true;
		m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj);// Redraw even if the object was already in this state,  because maybe we're hiding this and something that
		if( m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj->m_pParentObject )
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync((DesignObj_Orbiter *) m_pMouseBehavior->m_pMouseHandler_Horizontal->m_pObj->m_pParentObject);
	}

	if( !m_pObj || m_pObj->m_ChildObjects.size()==0 )
		return; // Shouldn't happen, this should be the volume control

	DesignObj_Orbiter *pObj_Child = (DesignObj_Orbiter *) *(m_pObj->m_ChildObjects.begin());
	if( pObj_Child )
		pObj_Child = (DesignObj_Orbiter *) *(pObj_Child->m_ChildObjects.begin());
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Always lock this before datagrid to prevent a deadlock
	PLUTO_SAFETY_LOCK( dng, m_pMouseBehavior->m_pOrbiter->m_DatagridMutex );
	if(  NULL == pObj_Child || pObj_Child->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
		return; // Also shouldn't happen
	DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObj_Child;
	if( !pObj_Grid->m_pDataGridTable )
		return; // Again shouldn't happen

	m_pObj_MediaBrowser_Down = m_pMouseBehavior->m_pOrbiter->FindObject(m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoDownIndicator_CONST));
	m_pObj_MediaBrowser_Up = m_pMouseBehavior->m_pOrbiter->FindObject(m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoUpIndicator_CONST));
	if( m_pObj_MediaBrowser_Down && !m_pObj_MediaBrowser_Down->m_bOnScreen )
		m_pObj_MediaBrowser_Down = NULL;
	if( m_pObj_MediaBrowser_Up && !m_pObj_MediaBrowser_Up->m_bOnScreen )
		m_pObj_MediaBrowser_Up = NULL;
	if( m_pObj_MediaBrowser_Down )
		m_pObj_MediaBrowser_Down->m_GraphicToDisplay=GRAPHIC_NORMAL;
	if( m_pObj_MediaBrowser_Up )
		m_pObj_MediaBrowser_Up->m_GraphicToDisplay=GRAPHIC_NORMAL;

	int Rows = pObj_Grid->m_pDataGridTable->GetRows();

	m_pMouseBehavior->m_pMouseGovernor->SetBuffer(2000);

	if( m_pMouseBehavior->m_iTime_Last_Mouse_Up )
	{
		m_bTapAndRelease=true;
		m_pMouseBehavior->SetMousePosition(pObj_Grid->m_rPosition.X + pObj_Grid->m_pPopupPoint.X + (pObj_Grid->m_rPosition.Width/2),
			pObj_Grid->m_rPosition.Y + pObj_Grid->m_pPopupPoint.Y + (pObj_Grid->m_rPosition.Height/2));
	}
	else
	{
		m_bTapAndRelease=false;
		int CurrentRow = pObj_Grid->m_iHighlightedRow>0 ? pObj_Grid->m_iHighlightedRow : 0;
		double RowHeight = (double) pObj_Grid->m_rPosition.Height / Rows;
		int Y = CurrentRow * RowHeight + pObj_Grid->m_rPosition.Y + pObj_Grid->m_pPopupPoint.Y;
		int X = pObj_Grid->m_rPosition.X + pObj_Grid->m_pPopupPoint.X + (pObj_Grid->m_rPosition.Width/2);
		m_pMouseBehavior->SetMousePosition(X,Y);
	}
	m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted=pObj_Grid;
}

void MediaMouseHandler::Stop()
{
	m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted=NULL;
}

bool MediaMouseHandler::ButtonDown(int PK_Button)
{
	if( !m_pObj || m_pObj->m_ChildObjects.size()==0 )
		return false; // Shouldn't happen, this should be the volume control

	DesignObj_Orbiter *pObj_Child = (DesignObj_Orbiter *) *(m_pObj->m_ChildObjects.begin());
	if( pObj_Child )
		pObj_Child = (DesignObj_Orbiter *) *(pObj_Child->m_ChildObjects.begin());

	if( PK_Button==BUTTON_Mouse_1_CONST || PK_Button==BUTTON_Mouse_6_CONST || PK_Button==BUTTON_Mouse_2_CONST )
	{
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000,false);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);

		if( PK_Button==BUTTON_Mouse_1_CONST )
			m_pMouseBehavior->m_pOrbiter->SelectedObject(pObj_Child,smNavigation);

		m_pMouseBehavior->Clear(true);
		return false; // this is now invalid
	}
	return false; // Keep processing
}

bool MediaMouseHandler::ButtonUp(int PK_Button)
{
	if( !m_pObj || m_pObj->m_ChildObjects.size()==0 )
		return false; // Shouldn't happen, this should be the volume control

	DesignObj_Orbiter *pObj_Child = (DesignObj_Orbiter *) *(m_pObj->m_ChildObjects.begin());
	if( pObj_Child )
		pObj_Child = (DesignObj_Orbiter *) *(pObj_Child->m_ChildObjects.begin());

	if( PK_Button==BUTTON_Mouse_6_CONST && !m_bTapAndRelease )  // The user was in press and hold mode, or tapped again after the menu appeared on screen
	{
		DCE::CMD_Change_Playback_Speed CMD_Change_Playback_Speed(m_pMouseBehavior->m_pOrbiter->m_dwPK_Device,m_pMouseBehavior->m_pOrbiter->m_dwPK_Device_NowPlaying,0,1000,false);
		m_pMouseBehavior->m_pOrbiter->SendCommand(CMD_Change_Playback_Speed);
		m_pMouseBehavior->m_pOrbiter->SelectedObject(pObj_Child,smNavigation);
		m_pMouseBehavior->Clear(true);
		return false; // this is now invalid
	}
	return false; // Keep processing
}

void MediaMouseHandler::Move(int X,int Y,int PK_Direction)
{
	if( !m_pObj || m_pObj->m_ChildObjects.size()==0 )
		return; // Shouldn't happen, this should be the volume control

	DesignObj_Orbiter *pObj_Child = (DesignObj_Orbiter *) *(m_pObj->m_ChildObjects.begin());
	if( pObj_Child )
		pObj_Child = (DesignObj_Orbiter *) *(pObj_Child->m_ChildObjects.begin());
	PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Always lock this before datagrid to prevent a deadlock
	PLUTO_SAFETY_LOCK( dng, m_pMouseBehavior->m_pOrbiter->m_DatagridMutex );
	if( !pObj_Child || pObj_Child->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
		return; // Also shouldn't happen
	DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObj_Child;
	if( !pObj_Grid->m_pDataGridTable )
		return; // Again shouldn't happen

	int Rows = pObj_Grid->m_pDataGridTable->GetRows();
	if( m_bTapAndRelease==false )  // Discrete positioning
	{
		double RowHeight = (double) pObj_Grid->m_rPosition.Height / Rows;
		int YDiff = Y - pObj_Grid->m_rPosition.Y - pObj_Grid->m_pPopupPoint.Y;
		int Row = YDiff / RowHeight;
		if( Row>Rows-1 )
			Row=Rows-1;
		pObj_Grid->m_iHighlightedRow = 1;
		pObj_Grid->m_GridCurRow = Row;
g_pPlutoLogger->Write(LV_FESTIVAL,"MouseBehavior::MediaTracks  *discrete* highlighted row %d",Row);
		delete pObj_Grid->m_pDataGridTable;
		pObj_Grid->m_pDataGridTable=NULL;
		pObj_Grid->bReAcquire=true;
		NeedToRender render( m_pMouseBehavior->m_pOrbiter, "MOUSE BEHAVIOR SCROLL" );
		m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(pObj_Grid);
		m_pMouseBehavior->m_pOrbiter->SelectedObject(pObj_Grid,smMouseGovernor);
	}
	else
	{
		// 20 notches, -10 to +10.  >5 do pages
		int NotchWidth = m_pObj->m_rPosition.Height/20;
		int Notch = (Y-m_pObj->m_rPosition.Y-m_pObj->m_pPopupPoint.Y)/NotchWidth - 10;
		if( Notch<-10 )
			Notch = -10;
		if( Notch>10 )
			Notch = 10;

		if( Notch!=m_iLastNotch )
		{
			if( Notch==0 )
				m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL);
			else
			{
				int Frequency;
				bool bPage=false;
				if( abs(Notch)>5 )
				{
					bPage=true;
					if( Notch>0 )
						Frequency = (6-(Notch-5))*100;
					else
						Frequency = (6+(Notch+5))*100;
				}
				else
					Frequency = (6-abs(Notch))*100;

if( Frequency<1 )
int k=2;
g_pPlutoLogger->Write(LV_FESTIVAL,"Frequency %d",Frequency);
				if( bPage )
					m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaTracks,Notch > 0 ? 2 : -2,Frequency,NULL);
				else
					m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaTracks,Notch > 0 ? 1 : -1,Frequency,NULL);
			}
			m_iLastNotch=Notch;

			Notch/=2;
			if( Notch > 0 ) // Down
			{
				m_pObj_MediaBrowser_Down->m_GraphicToDisplay=abs(Notch);
				m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj_MediaBrowser_Down);
			}
			else
			{
				m_pObj_MediaBrowser_Up->m_GraphicToDisplay=abs(Notch);
				m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj_MediaBrowser_Up);
			}

//remus  m_pObj->SetSpeed(Speed);
		}
	}
}
