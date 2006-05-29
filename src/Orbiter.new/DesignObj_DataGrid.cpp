#include "DesignObj_Orbiter.h"
#include "DesignObj_DataGrid.h"
#include "Orbiter.h"
#include "OrbiterRenderer.h"

#include "../DCE/Logger.h"
using namespace DCE;
//-------------------------------------------------------------------------------------------------------
DesignObj_DataGrid::~DesignObj_DataGrid() 
{
	for(int i=0;i<CACHE_SIZE;++i)
		delete m_pDataGridTableCache[i];		
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoUp()			
{ 
	return m_GridCurRow > 0;																
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoDown()		
{ 
	return m_pDataGridTable->GetRows() > m_GridCurRow + m_MaxRow - (HasMoreUp() ? 1 : 0); 
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoLeft()		
{ 
	return m_GridCurCol > 0;																
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoRight()		
{ 
	return m_pDataGridTable->GetCols() > m_GridCurCol + m_MaxCol;							
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::VerticalOnly()		
{ 
	return !CanGoLeft() && !CanGoRight();													
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::HasMoreUp()		
{ 
	return VerticalOnly() && CanGoUp();													
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::HasMoreDown()		
{ 
	return VerticalOnly() && CanGoDown();													
}
//-------------------------------------------------------------------------------------------------------
void DesignObj_DataGrid::RenderObject( DesignObj_Orbiter *pObj_Screen, PlutoPoint point, int Layer)
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING,"RenderDataGrid %s %p", m_ObjectID.c_str(), m_pDataGridTable);
#endif
	PLUTO_SAFETY_LOCK( dg, m_pOrbiter->m_DatagridMutex );
	string delSelections;
	string sSelectedIndex = "0";
	if ( !sSelVariable.empty(  ) )
	{
		PLUTO_SAFETY_LOCK( vm, m_pOrbiter->m_VariableMutex )
			delSelections = "|" + m_pOrbiter->m_mapVariable[atoi(sSelVariable.c_str())] + "|";
		sSelectedIndex = m_pOrbiter->m_mapVariable[atoi(sSelVariable.c_str())];
		vm.Release(  );
	}

#if ( defined( PROFILING_GRID ) )
	clock_t clkStart = clock(  );
#endif

	m_pOrbiter->PrepareRenderDataGrid(this, delSelections);

#if ( defined( PROFILING_GRID ) )
	clock_t clkAcquired = clock(  );
#endif

	if( !m_pDataGridTable )
		return;

	m_pOrbiter->Renderer()->SolidRectangle( point.X + m_rPosition.X, point.Y + m_rPosition.Y, m_rPosition.Width, m_rPosition.Height, PlutoColor( 0, 0, 0 ) );

	// short for "number of ARRow ROWS": ArrRows
	// last screen exception: we consider one up arrow as not being there so we don't skip a row when we scroll up
	int ArrRows = 0;

	DataGridTable *pT = m_pDataGridTable;
	int i,  j; //indexes

	bool bAddedUpButton=false, bAddedDownButton=false;

	// See if we should add page up/down cells -- see notes at top of file
	if(  m_sExtraInfo.find( 'P' )!=string::npos  )
	{
		ArrRows = CanGoDown(  ) + ( CanGoUp(  ) && CanGoDown(  ) );
		if ( CanGoUp(  ) )
		{
			m_iUpRow = 0;
			DataGridCell * pCell = new DataGridCell( "<Scroll up>" );
			pCell->m_Colspan = m_pDataGridTable->m_ColumnCount;
			RenderCell(pT,  pCell,  0,  0,  GRAPHIC_NORMAL, point);
			delete pCell;
			bAddedUpButton=true;
		}

		if ( CanGoDown(  ) )
		{
			m_dwIDownRow = m_pDataGridTable->m_RowCount - 1;
			DataGridCell * pCell = new DataGridCell( "<Scroll down>" );
			pCell->m_Colspan = m_pDataGridTable->m_ColumnCount;
			RenderCell(pT,  pCell,  0,  m_dwIDownRow,  GRAPHIC_NORMAL, point);
			delete pCell;
			bAddedDownButton=true;
		}
	}

	for ( i = 0; i < m_pDataGridTable->m_RowCount - ArrRows; i++ )
	{
		for ( j = 0; j < m_pDataGridTable->m_ColumnCount; j++ )
		{
			int DGRow = ( ( i == 0 && pT->m_bKeepRowHeader ) ? 0 : i + pT->m_StartingRow );
			int DGColumn = ( j == 0 && pT->m_bKeepColumnHeader ) ? 0 : j + pT->m_StartingColumn;

			DataGridCell * pCell = pT->GetData( DGColumn,  DGRow );

			if ( pCell )
			{
				if ( !m_bDontShowSelection )
				{
					int GraphicType = GRAPHIC_NORMAL;
					if(  delSelections.length(  ) > 2 && delSelections.find( "|"+string( pCell->GetValue(  ) )+"|" )!=string::npos  )
					{
						GraphicType = GRAPHIC_SELECTED;
						if( m_bHighlightSelectedCell )
						{
							m_iHighlightedRow = m_sExtraInfo.find( 'C' )==string::npos ? DGRow : -1;
							m_iHighlightedColumn = m_sExtraInfo.find( 'R' )==string::npos ? DGColumn : -1;
							m_bHighlightSelectedCell=false; // Only on the initial draw
						}
					}

					RenderCell(pT,  pCell,  j,  i + ( int ) bAddedUpButton,  GraphicType, point );
				}
				else
					RenderCell(pT,  pCell,  j,  i + ( int ) bAddedUpButton,  GRAPHIC_NORMAL, point );

				pCell = NULL;
			}
		}
	}

	m_pDataGridTable->m_RowCount = i + ArrRows;

#if ( defined( PROFILING_GRID ) )
	clock_t clkFinished = clock(  );

	g_pPlutoLogger->Write( LV_CONTROLLER, "Grid: %s took %d ms to acquire and %d ms to render",
		m_sGridID.c_str(), int(clkAcquired-clkStart), int(clkFinished-clkAcquired));
#endif
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ bool DesignObj_DataGrid::RenderCell(class DataGridTable *pT,  class DataGridCell *pCell,  
	int j,  int i,  int GraphicToDisplay, PlutoPoint point )
{
	TextStyle *pTextStyle = m_pTextStyle;
	bool bTransparentCell = false; // todo,  is transparency in PlutoColor? ( strchr( GetParameterValue( C_PARAMETER_CELL_COLOR_CONST ).c_str(  ),  ', ' )==NULL );

	if ( j==0 && pT->m_bKeepColumnHeader )
	{
		pTextStyle = m_pTextStyle_FirstCol;
		bTransparentCell = false;
	}
	if ( i==0 && pT->m_bKeepRowHeader )
	{
		pTextStyle = m_pTextStyle_FirstRow;
		bTransparentCell = false;
	}
	if ( pCell->m_AltColor > 0 && pCell->m_AltColor<( int ) m_vectTextStyle_Alt.size(  ) )
	{
		pTextStyle = m_vectTextStyle_Alt[pCell->m_AltColor];
		bTransparentCell = false;
	}
	if( GraphicToDisplay==GRAPHIC_SELECTED )
	{
		pTextStyle = m_pTextStyle_Selected;
		bTransparentCell = false;
	}
	else if ( GraphicToDisplay==GRAPHIC_HIGHLIGHTED )
	{
		pTextStyle = m_pTextStyle_Highlighted;
		bTransparentCell = false;
	}

	if(  !pTextStyle  )
		pTextStyle = m_pOrbiter->m_mapTextStyle_Find( 0 );
	if(  !pTextStyle  )
		pTextStyle = m_pOrbiter->m_mapTextStyle_Find( 1 );

	if(  !pTextStyle  )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Trying to render cell,  but cannot find any style" );
		return false;
	}
	int x, y, w, h;
	GetGridCellDimensions(pCell->m_Colspan,  pCell->m_Rowspan,  j,  i,  x,  y,  w,  h );
	if ( w>4 && h >4 )
	{
		if ( !bTransparentCell )
			m_pOrbiter->Renderer()->SolidRectangle( point.X + x,  point.Y + y,  w,  h,  pCell->m_AltColor ? pCell->m_AltColor : pTextStyle->m_BackColor);

		if ( pCell->m_pGraphicData )
		{
			PlutoGraphic *pPlutoGraphic = m_pOrbiter->Renderer()->CreateGraphic();
			pPlutoGraphic->m_GraphicFormat = pCell->m_GraphicFormat;
			pPlutoGraphic->LoadGraphic(pCell->m_pGraphicData,  pCell->m_GraphicLength, m_pOrbiter->m_iRotation);
			m_pOrbiter->Renderer()->RenderGraphic(pPlutoGraphic, PlutoRectangle(x,  y,  w,  h), m_bDisableAspectLock, point );
			delete pPlutoGraphic;
		}
g_pPlutoLogger->Write(LV_WARNING,"Rendering cell with %s",pCell->GetText());        

		// TODO -- temp hack -- don't show text on the media browser grid if we have cover art
		if( m_iPK_Datagrid==63 && pCell->m_pGraphicData )
			return bTransparentCell;

		DesignObjText Text(this);
		Text.m_rPosition = PlutoRectangle( x,  y,  w,  h );
		Text.m_iPK_HorizAlignment = pTextStyle->m_iPK_HorizAlignment;
		Text.m_iPK_VertAlignment = pTextStyle->m_iPK_VertAlignment;

		string sText = m_pOrbiter->SubstituteVariables(pCell->GetText(), this, 0, 0);
		m_pOrbiter->Renderer()->RenderText( sText, &Text, pTextStyle, point );
	}
	else
		g_pPlutoLogger->Write( LV_WARNING,  "Datagrid width or height is too small" );

	return bTransparentCell;
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void DesignObj_DataGrid::GetGridCellDimensions(int Colspan,  int Rowspan,  
	int Column,  int Row,  int &x,  int &y,  int &w,  int &h )
{
	if ( Colspan < 1 )
	{
		g_pPlutoLogger->Write( LV_WARNING,  "Data grid cell has column span of zero!" );
		x=y=w=h=0;
		return;
	}

	// These deltas make adjustments to the rest of the coodrinates when there's a first column
	// on the grid that's a different size from the rest.

	int DeltaX = 0,  DeltaY = 0;

	// Is the first row on the screen?
	if ( Row > 0 && m_FirstRowHeight > 0 && ( m_GridCurRow == 0 || m_bKeepRowHeader )  )
	{
		DeltaY = m_FirstRowHeight - m_FixedRowHeight;
	}
	// Is the first column on the screen?
	if ( Column > 0 && m_FirstColumnWidth > 0 && ( m_GridCurCol == 0 || m_bKeepColHeader )  )
	{
		DeltaX = m_FirstColumnWidth - m_FixedColumnWidth;
	}

	PLUTO_SAFETY_LOCK( vm, m_pOrbiter->m_VariableMutex )

	if( m_pOrbiter->m_iRotation==90 )
	{
		x = m_rPosition.Right()-( Row*( m_FixedRowHeight+1 ) ) + DeltaX - m_FixedRowHeight;
		y = m_rPosition.Y+( Column *( m_FixedColumnWidth+1 ) ) + DeltaY;
	}
	else if( m_pOrbiter->m_iRotation==180 )
	{
		x = m_rPosition.Right()-( (Column+Colspan-1)*( m_FixedColumnWidth+1 ) ) + DeltaX - m_FixedColumnWidth;
		y = m_rPosition.Bottom()-( (Row+Rowspan-1) *( m_FixedRowHeight+1 ) ) + DeltaY - m_FixedRowHeight;
	}
	else if( m_pOrbiter->m_iRotation==270 )
	{
		x = m_rPosition.X+( Row*( m_FixedRowHeight+1 ) ) + DeltaX;
		y = m_rPosition.Bottom()-( (Column+Colspan-1) *( m_FixedColumnWidth+1 ) ) + DeltaY - m_FixedColumnWidth;
	}
	else
	{
		x = m_rPosition.X+( Column*( m_FixedColumnWidth+1 ) ) + DeltaX;
		y = m_rPosition.Y+( Row *( m_FixedRowHeight+1 ) ) + DeltaY;
	}

	if ( Column == 0 && m_FirstColumnWidth > 0 && ( m_GridCurCol == 0 || m_bKeepColHeader ))
	{
		w = m_FirstColumnWidth + (Colspan-1) * m_FixedColumnWidth + ( Colspan-1 );
	}
	else
	{
		w = m_FixedColumnWidth * Colspan + ( Colspan-1 );
	}
	if ( Row == 0 && m_FirstRowHeight > 0  && ( m_GridCurRow == 0 || m_bKeepRowHeader ) )
	{
		h = m_FirstRowHeight + (Rowspan-1) * m_FixedRowHeight + ( Rowspan-1 );
	}
	else
	{
		h = m_FixedRowHeight * Rowspan+ ( Rowspan-1 );
	}

	if( m_pOrbiter->m_iRotation==0 )
	{
		if ( x+w > m_rPosition.X + m_rPosition.Width )
			// Oops,  width is greater than the size of the object.  Truncate.
			w = ( m_rPosition.X+m_rPosition.Width ) - x;
		if ( y+h > m_rPosition.Y + m_rPosition.Height )
			// Oops,  height is greater than the size of the object.  Truncate.
			h = ( m_rPosition.Y+m_rPosition.Height ) - y;
	}

	vm.Release(  );
	if ( w<0 || h<0 )
	{
		g_pPlutoLogger->Write( LV_WARNING,  "Data grid cell has negative size values."  );
		x=y=w=h=0;
		return;
	}

	if( m_pOrbiter->m_iRotation==90 || m_pOrbiter->m_iRotation==270 )
	{
		int ww = w;
		w=h;
		h=ww;
	}
}
//-------------------------------------------------------------------------------------------------------