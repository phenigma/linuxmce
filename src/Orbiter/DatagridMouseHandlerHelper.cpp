#include "DatagridMouseHandlerHelper.h"
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

DatagridMouseHandlerHelper::DatagridMouseHandlerHelper(MouseHandler *pMouseHandler)
{ 
	m_pMouseHandler=pMouseHandler;
	m_pMouseBehavior=m_pMouseHandler->m_pMouseBehavior;
	m_dwPK_Direction_ScrollGrid=0; m_pObj_MediaBrowser_Alpha=m_pObj_ScrollingGrid=NULL;
}

void DatagridMouseHandlerHelper::Start(DesignObj_DataGrid *pObj_ScrollingGrid)
{
	m_dwPK_Direction_ScrollGrid = 0;
	m_pObj_ScrollingGrid=pObj_ScrollingGrid;
	m_pObj_MediaBrowser_Down = m_pMouseBehavior->m_pOrbiter->FindObject(m_pMouseHandler->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoDownIndicator_CONST));
	m_pObj_MediaBrowser_Up = m_pMouseBehavior->m_pOrbiter->FindObject(m_pMouseHandler->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoUpIndicator_CONST));
	m_pObj_MediaBrowser_Alpha = m_pMouseBehavior->m_pOrbiter->FindObject(m_pMouseHandler->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoAlpha_CONST));
	if( m_pObj_MediaBrowser_Down && !m_pObj_MediaBrowser_Down->m_bOnScreen )
		m_pObj_MediaBrowser_Down = NULL;
	if( m_pObj_MediaBrowser_Up && !m_pObj_MediaBrowser_Up->m_bOnScreen )
		m_pObj_MediaBrowser_Up = NULL;
	if( m_pObj_MediaBrowser_Alpha && !m_pObj_MediaBrowser_Alpha->m_bOnScreen )
		m_pObj_MediaBrowser_Alpha = NULL;
	if( m_pObj_MediaBrowser_Down )
		m_pObj_MediaBrowser_Down->m_GraphicToDisplay=GRAPHIC_NORMAL;
	if( m_pObj_MediaBrowser_Up )
		m_pObj_MediaBrowser_Up->m_GraphicToDisplay=GRAPHIC_NORMAL;
}

bool DatagridMouseHandlerHelper::StayInGrid(int PK_Direction,int X,int Y)
{
	if( (m_pObj_ScrollingGrid->m_rPosition + m_pObj_ScrollingGrid->m_pPopupPoint).Contains(X,Y) )
	{
		if( !m_pObj_ScrollingGrid->m_pDataGridTable )
			return true;  // Must be in the middle of rendering

		// We're still inside the grid, just highlight whatever cell is underneath us, figure that out
		int Row_Before = m_pObj_ScrollingGrid->m_iHighlightedRow;
		int Column_Before = m_pObj_ScrollingGrid->m_iHighlightedColumn;

		int DeltaY=0,DeltaX=0;
		if ( m_pObj_ScrollingGrid->m_FirstRowHeight > 0 && ( m_pObj_ScrollingGrid->m_GridCurRow == 0 || m_pObj_ScrollingGrid->m_bKeepRowHeader )  )
			DeltaY = m_pObj_ScrollingGrid->m_FirstRowHeight - m_pObj_ScrollingGrid->m_FixedRowHeight;
		if ( m_pObj_ScrollingGrid->m_FirstColumnWidth > 0 && ( m_pObj_ScrollingGrid->m_GridCurCol == 0 || m_pObj_ScrollingGrid->m_bKeepColHeader )  )
			DeltaX = m_pObj_ScrollingGrid->m_FirstColumnWidth - m_pObj_ScrollingGrid->m_FixedColumnWidth;

		int X2 = X - m_pObj_ScrollingGrid->m_pPopupPoint.X - DeltaX;
		int Y2 = Y - m_pObj_ScrollingGrid->m_pPopupPoint.Y - DeltaY;
		
		int Row = Y2 / (m_pObj_ScrollingGrid->m_FixedRowHeight+1);
		int Column = X2 / (m_pObj_ScrollingGrid->m_FixedColumnWidth+1);
		if( m_pObj_ScrollingGrid->m_iHighlightedRow!=-1 )
			m_pObj_ScrollingGrid->m_iHighlightedRow = Row;
		if( m_pObj_ScrollingGrid->m_iHighlightedColumn!=-1 )
			m_pObj_ScrollingGrid->m_iHighlightedColumn = Column;

		if( m_pObj_ScrollingGrid->m_iHighlightedRow > m_pObj_ScrollingGrid->m_MaxRow - 1 )
			m_pObj_ScrollingGrid->m_iHighlightedRow = m_pObj_ScrollingGrid->m_MaxRow - 1;
		if( m_pObj_ScrollingGrid->m_iHighlightedColumn > m_pObj_ScrollingGrid->m_MaxCol - 1 )
			m_pObj_ScrollingGrid->m_iHighlightedColumn = m_pObj_ScrollingGrid->m_MaxCol - 1;

		if( m_pObj_ScrollingGrid->m_iHighlightedRow!=-1 && m_pObj_ScrollingGrid->m_GridCurRow + m_pObj_ScrollingGrid->m_iHighlightedRow >= m_pObj_ScrollingGrid->m_pDataGridTable->GetRows() )
			m_pObj_ScrollingGrid->m_iHighlightedRow = m_pObj_ScrollingGrid->m_pDataGridTable->GetRows() - m_pObj_ScrollingGrid->m_GridCurRow - 1;

		if( m_pObj_ScrollingGrid->m_iHighlightedColumn!=-1 && m_pObj_ScrollingGrid->m_GridCurCol + m_pObj_ScrollingGrid->m_iHighlightedColumn >= m_pObj_ScrollingGrid->m_pDataGridTable->GetCols() )
			m_pObj_ScrollingGrid->m_iHighlightedColumn = m_pObj_ScrollingGrid->m_pDataGridTable->GetCols() - m_pObj_ScrollingGrid->m_GridCurCol - 1;

		if( m_pObj_ScrollingGrid->m_iHighlightedRow!=Row_Before || m_pObj_ScrollingGrid->m_iHighlightedColumn!=Column_Before )
		{
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj_ScrollingGrid);

//m_pMouseBehavior->m_pOrbiter->HighlightNextObject(PK_Direction);
			m_pMouseBehavior->m_pOrbiter->RedrawObjects();  // We may have scrolled past the end of a grid and need to re-render
		}
		return true;
	}

	if( (PK_Direction==DIRECTION_Up_CONST || PK_Direction==DIRECTION_Down_CONST) &&
		MovedPastTopBottomOfDataGrid((DesignObj_DataGrid *) m_pObj_ScrollingGrid,PK_Direction,Y) )
			return true; // We're now iterator mode
	return false;
}
		

bool DatagridMouseHandlerHelper::MovedPastTopBottomOfDataGrid(DesignObj_DataGrid *pObj,int PK_Direction,int Y)
{
	// Moving past the bottom causes it to start the iterator and begin first scrolling
	// at up to 5 speeds, and then paging at up to 5 speeds by continually moving in the same direction.  As
	// soon as the direction changes, or the user moves back to the top, reset everything.
	if( PK_Direction==DIRECTION_Up_CONST )
	{ 
		if( Y >=pObj->m_rPosition.Y + pObj->m_pPopupPoint.Y || pObj->m_GridCurRow==0 )
			return false;  // We've still got room to go up
		m_dwPK_Direction_ScrollGrid = DIRECTION_Up_CONST;
		pObj->m_iHighlightedRow=0;

		// Move to the bottom of the grid, since all up movements are now setting the iterator
		m_pMouseBehavior->SetMousePosition( pObj->m_rPosition.X + pObj->m_pPopupPoint.X + pObj->m_rPosition.Width/2 , pObj->m_rPosition.Bottom() + pObj->m_pPopupPoint.Y );
	}
	else
	{
		// Down is trickier since the last visible cell may not extend all the way to the bottom (the top cell is always the top)
		// So get the coorindates of the last visible cell
		PlutoRectangle r;
		m_pMouseBehavior->m_pOrbiter->GetGridCellDimensions( pObj,
			1, // col span
			1, // row span
			0, // column
			pObj->m_MaxRow - 1 /* 0 based */, // We only care about the position of the last visible row, whether or not anything is there
			r.X,  r.Y,  r.Width,  r.Height );

		r = r + pObj->m_pPopupPoint;

		if( Y<r.Bottom() )
			return false;

		m_dwPK_Direction_ScrollGrid = DIRECTION_Down_CONST;
		pObj->m_iHighlightedRow=pObj->m_MaxRow-1;
		if( pObj->m_iHighlightedRow!=-1 && pObj->m_GridCurRow + pObj->m_iHighlightedRow >= pObj->m_pDataGridTable->GetRows() )
			pObj->m_iHighlightedRow = pObj->m_pDataGridTable->GetRows() - pObj->m_GridCurRow - 1;

		// Move to the top of the grid, since all up movements are now setting the iterator
		m_pMouseBehavior->SetMousePosition( pObj->m_rPosition.X + pObj->m_pPopupPoint.X + pObj->m_rPosition.Width/2 , pObj->m_rPosition.Y + pObj->m_pPopupPoint.Y );
	}

	m_pObj_ScrollingGrid = (DesignObj_DataGrid *) m_pMouseBehavior->m_pOrbiter->m_pObj_Highlighted;
	m_pMouseHandler->m_iLastNotch = -999; // Start with no movement
	m_pObj_MediaBrowser_Down->m_GraphicToDisplay=GRAPHIC_SELECTED;
	m_pObj_MediaBrowser_Up->m_GraphicToDisplay=GRAPHIC_SELECTED;

	return true;
}

void DatagridMouseHandlerHelper::ScrollGrid(int dwPK_Direction,int X,int Y)
{
	if( dwPK_Direction!=m_dwPK_Direction_ScrollGrid)
	{
g_pPlutoLogger->Write(LV_FESTIVAL,"DatagridMouseHandlerHelper::ScrollGrid direction changed from %d to %d",
					  m_dwPK_Direction_ScrollGrid,dwPK_Direction);
		// We moved past the top
		m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,0,NULL); // In case we're scrolling a grid

		if( m_dwPK_Direction_ScrollGrid==DIRECTION_Up_CONST )
			m_pMouseBehavior->SetMousePosition( m_pObj_ScrollingGrid->m_rPosition.X + m_pObj_ScrollingGrid->m_pPopupPoint.X + m_pObj_ScrollingGrid->m_rPosition.Width/2 , m_pObj_ScrollingGrid->m_rPosition.Y + m_pObj_ScrollingGrid->m_pPopupPoint.Y );
		else
		{
			if( m_pObj_ScrollingGrid->m_pDataGridTable && m_pObj_ScrollingGrid->m_iHighlightedRow!=-1 && m_pObj_ScrollingGrid->m_GridCurRow + m_pObj_ScrollingGrid->m_iHighlightedRow >= m_pObj_ScrollingGrid->m_pDataGridTable->GetRows() )
				m_pObj_ScrollingGrid->m_iHighlightedRow = m_pObj_ScrollingGrid->m_pDataGridTable->GetRows() - m_pObj_ScrollingGrid->m_GridCurRow - 1;

			PlutoRectangle r;
			m_pMouseBehavior->m_pOrbiter->GetGridCellDimensions( m_pObj_ScrollingGrid,
				1, // col span
				1, // row span
				0, // column
				m_pObj_ScrollingGrid->m_iHighlightedRow /* 0 based */, // We only care about the position of the last visible row, whether or not anything is there
				r.X,  r.Y,  r.Width,  r.Height );

			// Move back to the bottom of the grid, since we're done with the iterator
			m_pMouseBehavior->SetMousePosition( m_pObj_ScrollingGrid->m_rPosition.X + m_pObj_ScrollingGrid->m_pPopupPoint.X + m_pObj_ScrollingGrid->m_rPosition.Width/2 , r.Bottom() + m_pObj_ScrollingGrid->m_pPopupPoint.Y );
		}

		// Unhighlight the speed
		m_dwPK_Direction_ScrollGrid = 0;
		m_pMouseHandler->m_iLastNotch = -999;
		m_pObj_MediaBrowser_Down->m_GraphicToDisplay=GRAPHIC_SELECTED;
		m_pObj_MediaBrowser_Up->m_GraphicToDisplay=GRAPHIC_SELECTED;

		return;
	}

	// 11 notches, 0=pause, 5 scroll, 5 pages
	int NotchWidth = m_pObj_ScrollingGrid->m_rPosition.Height/11;
	int Notch = (Y-m_pObj_ScrollingGrid->m_rPosition.Y-m_pObj_ScrollingGrid->m_pPopupPoint.Y)/NotchWidth;
	if( Notch<0 )
		Notch = 0;
	if( Notch>10 )
		Notch = 10;

	if( dwPK_Direction==DIRECTION_Up_CONST )
		Notch = 10 - Notch; // It's reversed if we're moving up

	if( Notch==0 )
	{
		m_pMouseHandler->m_iLastNotch=Notch;
		return;
	}

	if( Notch!=m_pMouseHandler->m_iLastNotch )
	{
g_pPlutoLogger->Write(LV_FESTIVAL,"DatagridMouseHandlerHelper::ScrollGrid Notch %d",Notch);
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "scroll" );
		m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj_ScrollingGrid);
		// We know the notch can't go in reverse since that would stop the scrolling
		if( Notch<=5 )
		{
			if( dwPK_Direction==DIRECTION_Up_CONST )
				m_pMouseBehavior->m_pOrbiter->CMD_Scroll_Grid("","",DIRECTION_Up_CONST);
			else
				m_pMouseBehavior->m_pOrbiter->CMD_Scroll_Grid("","",DIRECTION_Down_CONST);

			m_pMouseHandler->m_iLastNotch=Notch;
			return;
		}

		int Frequency;
		bool bPage=false;
		Frequency = (11-Notch)*100;
		m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaTracks,dwPK_Direction==DIRECTION_Down_CONST ? 2 : -2,Frequency,NULL);
		m_pMouseHandler->m_iLastNotch=Notch;
		if( dwPK_Direction==DIRECTION_Down_CONST )
		{
			m_pObj_MediaBrowser_Down->m_GraphicToDisplay=m_pMouseHandler->m_iLastNotch-5;
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj_MediaBrowser_Down);
		}
		else
		{
			m_pObj_MediaBrowser_Up->m_GraphicToDisplay=m_pMouseHandler->m_iLastNotch-5;
			m_pMouseBehavior->m_pOrbiter->RenderObjectAsync(m_pObj_MediaBrowser_Up);
		}
//remus  m_pObj_ScrollingGrid->SetSpeed(Speed);
	}
}
