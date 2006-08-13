#include "DataGridRenderer.h"
#include "../DCE/Logger.h"
#include "Orbiter.h"
#include "OrbiterRenderer.h"
#include "DesignObj_DataGrid.h"
#include "../DCE/DataGrid.h"

using namespace DCE;

DataGridRenderer::DataGridRenderer(DesignObj_Orbiter *pOwner): ObjectRenderer(pOwner), m_DataGridCacheMutex("DataGrid Cache")
{
	pthread_mutexattr_t MutexAttr;
	pthread_mutexattr_init( &MutexAttr );
	pthread_mutexattr_settype( &MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_DataGridCacheMutex.Init( &MutexAttr );
	m_DataGridCacheThread = NULL;
	pthread_cond_init(&m_DataGridCacheCond, NULL);	
	m_pDataGridOwner = dynamic_cast<DesignObj_DataGrid *>(pOwner);
}

/*virtual*/ DataGridRenderer::~DataGridRenderer(void)
{
	for( int Direction=1;Direction<=4;Direction++)
	{
		FlushCache(Direction);
	}
}

/*virtual*/ void DataGridRenderer::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	if(NULL == m_pDataGridOwner)
		return;

	if(!PreRenderActions(pObj_Screen, point))
		return;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING,"RenderDataGrid %s %p",m_pDataGridOwner->m_ObjectID.c_str(),m_pDataGridOwner->m_pDataGridTable);
#endif
	PLUTO_SAFETY_LOCK( dg, m_pDataGridOwner->m_pOrbiter->m_DatagridMutex );
	string delSelections;
	string sSelectedIndex = "0";
	if(!m_pDataGridOwner->sSelVariable.empty())
	{
		PLUTO_SAFETY_LOCK( vm, m_pDataGridOwner->m_pOrbiter->m_VariableMutex )
			delSelections = "|" + m_pDataGridOwner->m_pOrbiter->m_mapVariable[atoi(m_pDataGridOwner->sSelVariable.c_str())] + "|";
		sSelectedIndex = m_pDataGridOwner->m_pOrbiter->m_mapVariable[atoi(m_pDataGridOwner->sSelVariable.c_str())];
		vm.Release();
	}

#if ( defined( PROFILING_GRID ) )
	clock_t clkStart = clock(  );
#endif

	m_pDataGridOwner->PrepareRenderDataGrid(delSelections);

#if ( defined( PROFILING_GRID ) )
	clock_t clkAcquired = clock(  );
#endif

	if( !m_pDataGridOwner->m_pDataGridTable )
		return;

	int nAlphaChannel = GetAlphaLevel();

	m_pDataGridOwner->m_pOrbiter->Renderer()->SolidRectangle( point.X + m_pDataGridOwner->m_rPosition.X, 
		point.Y + m_pDataGridOwner->m_rPosition.Y, m_pDataGridOwner->m_rPosition.Width, 
		m_pDataGridOwner->m_rPosition.Height, PlutoColor(0, 0, 0, nAlphaChannel));

	// short for "number of ARRow ROWS": ArrRows
	// last screen exception: we consider one up arrow as not being there so we don't skip a row when we scroll up
	int ArrRows = 0;

	DataGridTable *pT = m_pDataGridOwner->m_pDataGridTable;
	int i,  j; //indexes

	bool bAddedUpButton=false, bAddedDownButton=false;

	// See if we should add page up/down cells -- see notes at top of file
	if(  m_pDataGridOwner->m_sExtraInfo.find( 'P' )!=string::npos  )
	{
		ArrRows = m_pDataGridOwner->CanGoDown(  ) + ( m_pDataGridOwner->CanGoUp(  ) && m_pDataGridOwner->CanGoDown(  ) );
		if ( m_pDataGridOwner->CanGoUp(  ) )
		{
			m_pDataGridOwner->m_iUpRow = 0;
			DataGridCell * pCell = new DataGridCell( "<Scroll up>" );
			pCell->m_Colspan = m_pDataGridOwner->m_pDataGridTable->m_ColumnCount;
			RenderCell(pT,  pCell,  0,  0,  GRAPHIC_NORMAL, point );
			delete pCell;
			bAddedUpButton=true;
		}

		if ( m_pDataGridOwner->CanGoDown(  ) )
		{
			m_pDataGridOwner->m_dwIDownRow = m_pDataGridOwner->m_pDataGridTable->m_RowCount - 1;
			DataGridCell * pCell = new DataGridCell( "<Scroll down>" );
			pCell->m_Colspan = m_pDataGridOwner->m_pDataGridTable->m_ColumnCount;
			RenderCell(pT,  pCell,  0,  m_pDataGridOwner->m_dwIDownRow,  GRAPHIC_NORMAL, point );
			delete pCell;
			bAddedDownButton=true;
		}
	}

	for ( i = 0; i < m_pDataGridOwner->m_pDataGridTable->m_RowCount - ArrRows; i++ )
	{
		for ( j = 0; j < m_pDataGridOwner->m_pDataGridTable->m_ColumnCount; j++ )
		{
			int DGRow = ( ( i == 0 && pT->m_bKeepRowHeader ) ? 0 : i + pT->m_StartingRow );
			int DGColumn = ( j == 0 && pT->m_bKeepColumnHeader ) ? 0 : j + pT->m_StartingColumn;

			DataGridCell * pCell = pT->GetData( DGColumn,  DGRow );

			if ( pCell )
			{
				if ( !m_pDataGridOwner->m_bDontShowSelection )
				{
					int GraphicType = GRAPHIC_NORMAL;
					if(  delSelections.length(  ) > 2 && delSelections.find( "|"+string( pCell->GetValue(  ) )+"|" )!=string::npos  )
					{
						GraphicType = GRAPHIC_SELECTED;
						if( m_pDataGridOwner->m_bHighlightSelectedCell )
						{
							m_pDataGridOwner->m_iHighlightedRow = m_pDataGridOwner->m_sExtraInfo.find( 'C' )==string::npos ? DGRow : -1;
							m_pDataGridOwner->m_iHighlightedColumn = m_pDataGridOwner->m_sExtraInfo.find( 'R' )==string::npos ? DGColumn : -1;
							m_pDataGridOwner->m_bHighlightSelectedCell=false; // Only on the initial draw
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

	m_pDataGridOwner->m_pDataGridTable->m_RowCount = i + ArrRows;

#if ( defined( PROFILING_GRID ) )
	clock_t clkFinished = clock(  );

	g_pPlutoLogger->Write( LV_CONTROLLER, "Grid: %s took %d ms to acquire and %d ms to render",
		m_pDataGridOwner->m_sGridID.c_str(), int(clkAcquired-clkStart), int(clkFinished-clkAcquired));
#endif

	PostRenderActions(pObj_Screen, point);
}

/*virtual*/ void DataGridRenderer::GetGridCellDimensions(int Colspan,  int Rowspan,  int Column,  int Row,  int &x,  int &y,  int &w,  int &h )
{
	if(NULL == m_pDataGridOwner)
		return;

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
	if ( Row > 0 && m_pDataGridOwner->m_FirstRowHeight > 0 && ( m_pDataGridOwner->m_GridCurRow == 0 || m_pDataGridOwner->m_bKeepRowHeader )  )
	{
		DeltaY = m_pDataGridOwner->m_FirstRowHeight - m_pDataGridOwner->m_FixedRowHeight;
	}
	// Is the first column on the screen?
	if ( Column > 0 && m_pDataGridOwner->m_FirstColumnWidth > 0 && ( m_pDataGridOwner->m_GridCurCol == 0 || m_pDataGridOwner->m_bKeepColHeader )  )
	{
		DeltaX = m_pDataGridOwner->m_FirstColumnWidth - m_pDataGridOwner->m_FixedColumnWidth;
	}

	if( m_pDataGridOwner->m_pOrbiter->m_iRotation==90 )
	{
		x = m_pDataGridOwner->m_rPosition.Right()-( Row*( m_pDataGridOwner->m_FixedRowHeight+1 ) ) + DeltaX - m_pDataGridOwner->m_FixedRowHeight;
		y = m_pDataGridOwner->m_rPosition.Y+( Column *( m_pDataGridOwner->m_FixedColumnWidth+1 ) ) + DeltaY;
	}
	else if( m_pDataGridOwner->m_pOrbiter->m_iRotation==180 )
	{
		x = m_pDataGridOwner->m_rPosition.Right()-( (Column+Colspan-1)*( m_pDataGridOwner->m_FixedColumnWidth+1 ) ) + DeltaX - m_pDataGridOwner->m_FixedColumnWidth;
		y = m_pDataGridOwner->m_rPosition.Bottom()-( (Row+Rowspan-1) *( m_pDataGridOwner->m_FixedRowHeight+1 ) ) + DeltaY - m_pDataGridOwner->m_FixedRowHeight;
	}
	else if( m_pDataGridOwner->m_pOrbiter->m_iRotation==270 )
	{
		x = m_pDataGridOwner->m_rPosition.X+( Row*( m_pDataGridOwner->m_FixedRowHeight+1 ) ) + DeltaX;
		y = m_pDataGridOwner->m_rPosition.Bottom()-( (Column+Colspan-1) *( m_pDataGridOwner->m_FixedColumnWidth+1 ) ) + DeltaY - m_pDataGridOwner->m_FixedColumnWidth;
	}
	else
	{
		x = m_pDataGridOwner->m_rPosition.X+( Column*( m_pDataGridOwner->m_FixedColumnWidth+1 ) ) + DeltaX;
		y = m_pDataGridOwner->m_rPosition.Y+( Row *( m_pDataGridOwner->m_FixedRowHeight+1 ) ) + DeltaY;
	}

	if ( Column == 0 && m_pDataGridOwner->m_FirstColumnWidth > 0 && ( m_pDataGridOwner->m_GridCurCol == 0 || m_pDataGridOwner->m_bKeepColHeader ))
	{
		w = m_pDataGridOwner->m_FirstColumnWidth + (Colspan-1) * m_pDataGridOwner->m_FixedColumnWidth + ( Colspan-1 );
	}
	else
	{
		w = m_pDataGridOwner->m_FixedColumnWidth * Colspan + ( Colspan-1 );
	}
	if ( Row == 0 && m_pDataGridOwner->m_FirstRowHeight > 0  && ( m_pDataGridOwner->m_GridCurRow == 0 || m_pDataGridOwner->m_bKeepRowHeader ) )
	{
		h = m_pDataGridOwner->m_FirstRowHeight + (Rowspan-1) * m_pDataGridOwner->m_FixedRowHeight + ( Rowspan-1 );
	}
	else
	{
		h = m_pDataGridOwner->m_FixedRowHeight * Rowspan+ ( Rowspan-1 );
	}

	if( m_pDataGridOwner->m_pOrbiter->m_iRotation==0 )
	{
		if ( x+w > m_pDataGridOwner->m_rPosition.X + m_pDataGridOwner->m_rPosition.Width )
			// Oops,  width is greater than the size of the object.  Truncate.
			w = ( m_pDataGridOwner->m_rPosition.X+m_pDataGridOwner->m_rPosition.Width ) - x;
		if ( y+h > m_pDataGridOwner->m_rPosition.Y + m_pDataGridOwner->m_rPosition.Height )
			// Oops,  height is greater than the size of the object.  Truncate.
			h = ( m_pDataGridOwner->m_rPosition.Y+m_pDataGridOwner->m_rPosition.Height ) - y;
	}

	if ( w<0 || h<0 )
	{
		g_pPlutoLogger->Write( LV_WARNING,  "Data grid cell has negative size values."  );
		x=y=w=h=0;
		return;
	}

	if( m_pDataGridOwner->m_pOrbiter->m_iRotation==90 || m_pDataGridOwner->m_pOrbiter->m_iRotation==270 )
	{
		int ww = w;
		w=h;
		h=ww;
	}
}

/*virtual*/ bool DataGridRenderer::RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point)
{
	if(NULL == m_pDataGridOwner)
		return false;

	int nAlphaChannel = GetAlphaLevel();

	TextStyle *pTextStyle = m_pDataGridOwner->m_pTextStyle;
	bool bTransparentCell = false; // todo,  is transparency in PlutoColor? ( strchr( GetParameterValue( C_PARAMETER_CELL_COLOR_CONST ).c_str(  ),  ', ' )==NULL );

	if ( j==0 && pT->m_bKeepColumnHeader )
	{
		pTextStyle = m_pDataGridOwner->m_pTextStyle_FirstCol;
		bTransparentCell = false;
	}
	if ( i==0 && pT->m_bKeepRowHeader )
	{
		pTextStyle = m_pDataGridOwner->m_pTextStyle_FirstRow;
		bTransparentCell = false;
	}
	if ( pCell->m_AltColor > 0 && pCell->m_AltColor<( int ) m_pDataGridOwner->m_vectTextStyle_Alt.size(  ) )
	{
		pTextStyle = m_pDataGridOwner->m_vectTextStyle_Alt[pCell->m_AltColor];
		bTransparentCell = false;
	}
	if( iGraphicToDisplay==GRAPHIC_SELECTED )
	{
		pTextStyle = m_pDataGridOwner->m_pTextStyle_Selected;
		bTransparentCell = false;
	}
	else if ( iGraphicToDisplay==GRAPHIC_HIGHLIGHTED )
	{
		pTextStyle = m_pDataGridOwner->m_pTextStyle_Highlighted;
		bTransparentCell = false;
	}

	if(  !pTextStyle  )
		pTextStyle = m_pDataGridOwner->m_pOrbiter->m_mapTextStyle_Find( 0 );
	if(  !pTextStyle  )
		pTextStyle = m_pDataGridOwner->m_pOrbiter->m_mapTextStyle_Find( 1 );

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
		{
			PlutoColor CellColor = pCell->m_AltColor ? pCell->m_AltColor : pTextStyle->m_BackColor;
			CellColor.SetAlpha(nAlphaChannel);
			m_pDataGridOwner->m_pOrbiter->Renderer()->SolidRectangle( point.X + x,  point.Y + y,  w,  h,  CellColor);
		}

		PLUTO_SAFETY_LOCK(M,m_pDataGridOwner->m_pOrbiter->Renderer()->m_bgImageReqMutex);

		const char *pPath = pCell->GetImagePath();

		if (pPath && !pCell->m_pGraphicData && !pCell->m_pGraphic)
		{
			if (1 == 1) // todo: if background image loading is requested
 			{
				M.Release();
g_pPlutoLogger->Write(LV_EVENT,"DataGridRenderer::RenderCell loading %s in bg for %d,%d",pPath,j,i);
				m_pDataGridOwner->m_pOrbiter->Renderer()->BackgroundImageLoad(pPath, &pCell->m_pGraphic);              
				M.Relock();
			}
			else
			{
				if (m_pDataGridOwner->m_pOrbiter->m_bIsOSD)
				{
					size_t Length; 

					pCell->m_pGraphicData = FileUtils::ReadFileIntoBuffer(pPath, Length);
					pCell->m_GraphicLength = (int)Length;
				}
				else
				{
					int Length=0;

					DCE::CMD_Request_File_Cat CMD_Request_File_Cat( m_pDataGridOwner->m_pOrbiter->m_dwPK_Device, DEVICECATEGORY_General_Info_Plugins_CONST, false,  BL_SameHouse, pPath,
						&pCell->m_pGraphicData, &Length );
					m_pDataGridOwner->m_pOrbiter->SendCommand( CMD_Request_File_Cat );
					if (Length > 0)
					{
						pCell->m_GraphicLength = Length;
						pCell->m_GraphicFormat = GR_JPG;
					}
				}
			}
		} 
		if ( pCell->m_pGraphicData && !pCell->m_pGraphic )
		{
			pCell->m_pGraphic = m_pDataGridOwner->m_pOrbiter->Renderer()->CreateGraphic();
			pCell->m_pGraphic->m_GraphicFormat = pCell->m_GraphicFormat;
			pCell->m_pGraphic->LoadGraphic(pCell->m_pGraphicData,  pCell->m_GraphicLength, m_pDataGridOwner->m_pOrbiter->m_iRotation);
			// Todo, since we're leaving the graphic in native format from this point foward,
			// we can probably delete the compressed-format image here.
		}
		if (pCell->m_pGraphic)
		{
			m_pDataGridOwner->m_pOrbiter->Renderer()->RenderGraphic(pCell->m_pGraphic, PlutoRectangle(x,  y,  w,  h), m_pDataGridOwner->m_bDisableAspectLock, point );
		}
		g_pPlutoLogger->Write(LV_WARNING,"Rendering cell with %s",pCell->GetText());        

		// TODO -- temp hack -- don't show text on the media browser grid if we have cover art
		if( m_pDataGridOwner->m_iPK_Datagrid==63 && pCell->m_pGraphicData )
			return bTransparentCell;

		DesignObjText Text(m_pDataGridOwner);
		// todo         Text.m_Rect = PlutoRectangle( x+pObj->BorderWidth,  y+pObj->BorderWidth,  w-( 2*pObj->BorderWidth ),  h-( 2*pObj->BorderWidth ) );
		Text.m_rPosition = PlutoRectangle( x,  y,  w,  h );
		//pTextStyle->m_BackColor = PlutoColor( 0, 0, 0, 255 );
		//pTextStyle->m_ForeColor = PlutoColor( 255, 255, 255, 255 );
		//pTextStyle->m_iPK_VertAlignment=2;

		Text.m_iPK_HorizAlignment = pTextStyle->m_iPK_HorizAlignment;
		Text.m_iPK_VertAlignment = pTextStyle->m_iPK_VertAlignment;

		string sText = m_pDataGridOwner->m_pOrbiter->SubstituteVariables(pCell->GetText(), m_pDataGridOwner, 0, 0);
		m_pDataGridOwner->m_pOrbiter->Renderer()->RenderText(sText, &Text, pTextStyle, point);
	}
	else
		g_pPlutoLogger->Write( LV_WARNING,  "Datagrid width or height is too small" );


	return bTransparentCell;
}

bool DataGridRenderer::Scroll_Grid(string sRelative_Level, int iPK_Direction,bool bMoveOneLineIfCannotPage)
{
	if ( m_pDataGridOwner->m_pDataGridTable )
	{
		StopCacheThread();

		if(  sRelative_Level=="-1"  )
		{
			if(  iPK_Direction == DIRECTION_Up_CONST  )
				m_pDataGridOwner->m_GridCurRow = 0;
		}
		else
		{
			int CurrentRow = m_pDataGridOwner->m_GridCurRow;
			int CurrentCol = m_pDataGridOwner->m_GridCurCol;
			if(  iPK_Direction == DIRECTION_Up_CONST )
			{
				if (!CalculateGridMovement(DIRECTION_Up_CONST, m_pDataGridOwner->m_GridCurRow,  atoi( sRelative_Level.c_str(  ) ) ))
				{
					if( bMoveOneLineIfCannotPage )
						m_pDataGridOwner->m_pOrbiter->CMD_Move_Up();
					return false;
				}
				
				PLUTO_SAFETY_LOCK( dgc, m_DataGridCacheMutex );

				if (m_pDataGridOwner->m_iCacheRows > 0) // Are we caching? 
				{                            // If so the current table needs to be pushed onto the opposing cache list.
					m_listDataGridCache[DIRECTION_Down_CONST].push_front(m_pDataGridOwner->m_pDataGridTable); // The cache acquisition thread will delete the excess cache.
				}
				else 
				{
					delete m_pDataGridOwner->m_pDataGridTable;
				}	
				FlushCache(DIRECTION_Left_CONST);
				FlushCache(DIRECTION_Right_CONST);

				if (m_listDataGridCache[DIRECTION_Up_CONST].size() > 0)
				{
					m_pDataGridOwner->m_pDataGridTable = m_listDataGridCache[DIRECTION_Up_CONST].front();
					m_listDataGridCache[DIRECTION_Up_CONST].pop_front();
				}
				else
				{
					m_pDataGridOwner->m_pDataGridTable = NULL;
					m_pDataGridOwner->bReAcquire=true;
				}
			}
			else if(  iPK_Direction == DIRECTION_Down_CONST  )
			{
				if (!CalculateGridMovement(DIRECTION_Down_CONST, m_pDataGridOwner->m_GridCurRow,  atoi( sRelative_Level.c_str(  ) ) ) )
				{
					if (m_pDataGridOwner->m_pDataGridTable->getTotalRowCount() > 0 && bMoveOneLineIfCannotPage)
					{
						m_pDataGridOwner->m_pOrbiter->CMD_Move_Down();
					}
					return false;
				}
				PLUTO_SAFETY_LOCK( dgc, m_DataGridCacheMutex );
				if (m_pDataGridOwner->m_iCacheRows > 0) // Are we caching? 
				{                            // If so the current table needs to be pushed onto the opposing cache list.
					m_listDataGridCache[DIRECTION_Up_CONST].push_front(m_pDataGridOwner->m_pDataGridTable); // The cache acquisition thread will delete the excess cache.
				}
				else 
				{
					delete m_pDataGridOwner->m_pDataGridTable;
				}	
				FlushCache(DIRECTION_Left_CONST);
				FlushCache(DIRECTION_Right_CONST);

				if (m_listDataGridCache[DIRECTION_Down_CONST].size() > 0)
				{
					m_pDataGridOwner->m_pDataGridTable = m_listDataGridCache[DIRECTION_Down_CONST].front();
					m_listDataGridCache[DIRECTION_Down_CONST].pop_front();
				}
				else
				{
					m_pDataGridOwner->m_pDataGridTable = NULL;
					m_pDataGridOwner->bReAcquire=true;
				}
			}
			else if(  iPK_Direction == DIRECTION_Left_CONST  )
			{
				if (!CalculateGridMovement(DIRECTION_Left_CONST,  m_pDataGridOwner->m_GridCurCol,  atoi( sRelative_Level.c_str(  ) ) ))
				{
					if( bMoveOneLineIfCannotPage )
						m_pDataGridOwner->m_pOrbiter->CMD_Move_Left();
					return false;
				}
				PLUTO_SAFETY_LOCK( dgc, m_DataGridCacheMutex );

				if (m_pDataGridOwner->m_iCacheRows > 0) // Are we caching? 
				{                            // If so the current table needs to be pushed onto the opposing cache list.
					m_listDataGridCache[DIRECTION_Right_CONST].push_front(m_pDataGridOwner->m_pDataGridTable); // The cache acquisition thread will delete the excess cache.
				}
				else 
				{
					delete m_pDataGridOwner->m_pDataGridTable;
				}	
				FlushCache(DIRECTION_Left_CONST);
				FlushCache(DIRECTION_Right_CONST);

				if (m_listDataGridCache[DIRECTION_Left_CONST].size() > 0)
				{
					m_pDataGridOwner->m_pDataGridTable = m_listDataGridCache[DIRECTION_Left_CONST].front();
					m_listDataGridCache[DIRECTION_Left_CONST].pop_front();
				}
				else
				{
					m_pDataGridOwner->m_pDataGridTable = NULL;
					m_pDataGridOwner->bReAcquire=true;
				}
			}
			else if(  iPK_Direction == DIRECTION_Right_CONST  )
			{
				if (!CalculateGridMovement(DIRECTION_Right_CONST,  m_pDataGridOwner->m_GridCurCol,  atoi( sRelative_Level.c_str(  ) ) ))
				{
					if( bMoveOneLineIfCannotPage )
						m_pDataGridOwner->m_pOrbiter->CMD_Move_Right();
					return false;
				}
				PLUTO_SAFETY_LOCK( dgc, m_DataGridCacheMutex );

				if (m_pDataGridOwner->m_iCacheRows > 0) // Are we caching? 
				{                            // If so the current table needs to be pushed onto the opposing cache list.
					m_listDataGridCache[DIRECTION_Left_CONST].push_front(m_pDataGridOwner->m_pDataGridTable); // The cache acquisition thread will delete the excess cache.
				}
				else 
				{
					delete m_pDataGridOwner->m_pDataGridTable;
				}	
				FlushCache(DIRECTION_Up_CONST);
				FlushCache(DIRECTION_Down_CONST);

				if (m_listDataGridCache[DIRECTION_Right_CONST].size() > 0)
				{
					m_pDataGridOwner->m_pDataGridTable = m_listDataGridCache[DIRECTION_Right_CONST].front();
					m_listDataGridCache[DIRECTION_Right_CONST].pop_front();
				}
				else
				{
					m_pDataGridOwner->m_pDataGridTable = NULL;
					m_pDataGridOwner->bReAcquire=true;
				}
			}
		}
	}
	m_pDataGridOwner->m_pOrbiter->Renderer()->RenderObjectAsync(m_pDataGridOwner);
	return true;
}

bool DataGridRenderer::CalculateGridMovement(int Direction, int &Cur,  int CellsToSkip)
{
	PLUTO_SAFETY_LOCK( dg, m_pDataGridOwner->m_pOrbiter->m_DatagridMutex );

	int InitialCur = Cur;

	switch(Direction)
	{
	case DIRECTION_Up_CONST:
		if ( CellsToSkip == 0 )
			CellsToSkip = m_pDataGridOwner->m_MaxRow - ( m_pDataGridOwner->m_pDataGridTable->m_bKeepRowHeader ? 1 : 0 );

		// Are we going to display 'scroll up/down' cells?  If so, we may not be able to scroll up a full page
		if(  m_pDataGridOwner->m_sExtraInfo.find( 'P' )!=string::npos  )
		{
			// After doing the scroll, will we still be able to go up?  If so, reduce the cells to skip by 1
			if( Cur - CellsToSkip > 0)
				CellsToSkip--;
			// After doing the scroll, will we still be able to go down?  If so, reduce the cells to skip by 1
			if( m_pDataGridOwner->m_pDataGridTable->GetRows() > (Cur - CellsToSkip) + m_pDataGridOwner->m_MaxRow )
				CellsToSkip--;
		}

		if ( CellsToSkip < 0 )
			CellsToSkip = 0;

		Cur -= CellsToSkip;
		if ( Cur <= 1 )
			Cur = 0;
		break;
	case DIRECTION_Down_CONST:
		{
			if ( !m_pDataGridOwner->CanGoDown() && CellsToSkip == 0 )
				return false;

			if ( CellsToSkip == 0 )
				CellsToSkip = m_pDataGridOwner->m_MaxRow - ( m_pDataGridOwner->m_pDataGridTable->m_bKeepRowHeader ? 1 : 0 );
		
			bool bCanGoDown = m_pDataGridOwner->m_dwIDownRow >= 0;
			bool bCanGoUp = m_pDataGridOwner->m_iUpRow >= 0;
			CellsToSkip -= ( bCanGoDown + bCanGoUp );

			if ( CellsToSkip < 0 )
				CellsToSkip = 0;

			Cur += CellsToSkip;

			if ( Cur+m_pDataGridOwner->m_MaxRow > ( m_pDataGridOwner->m_pDataGridTable )->GetRows(  ) )
			{
				// Add an extra 1,  becuase if we end  up making the top row an up ( unknown at this point )
				// We'll be one short
				Cur = m_pDataGridOwner->m_pDataGridTable->GetRows(  ) - m_pDataGridOwner->m_MaxRow + 1;
				if ( Cur<0 )
					Cur=0;
			}
		}
		break;
	case DIRECTION_Left_CONST:
		if ( !m_pDataGridOwner->CanGoRight() && CellsToSkip == 0 ) // Todo - This doesn't look right, don't we mean left? -- Rob
			return false;

		if ( CellsToSkip==0 )
			CellsToSkip = m_pDataGridOwner->m_MaxCol-( m_pDataGridOwner->m_pDataGridTable->m_bKeepColumnHeader ? 2 : 1 );
		if ( CellsToSkip<=0 )
			CellsToSkip = 1;

		Cur-=CellsToSkip;
		if ( Cur<0 )
			Cur=0;
		break;
	case DIRECTION_Right_CONST:
		if ( CellsToSkip==0 )
			CellsToSkip = m_pDataGridOwner->m_MaxCol-( m_pDataGridOwner->m_pDataGridTable->m_bKeepColumnHeader ? 2 : 1 );
		if ( CellsToSkip<=0 )
			CellsToSkip = 1;

		Cur+=CellsToSkip;

		if ( Cur+m_pDataGridOwner->m_MaxCol > ( m_pDataGridOwner->m_pDataGridTable )->GetCols(  ) )
		{
			Cur = ( m_pDataGridOwner->m_pDataGridTable )->GetCols(  ) - m_pDataGridOwner->m_MaxCol;
			if ( Cur<0 )
				Cur=0;
		}
		break;
	}
	return (Cur!=InitialCur);
}

void DataGridRenderer::FlushCache(int Direction)
{
	if( !Direction )
	{
		FlushCache(DIRECTION_Left_CONST);
		FlushCache(DIRECTION_Right_CONST);
		FlushCache(DIRECTION_Up_CONST);
		FlushCache(DIRECTION_Down_CONST);
		return;
	}
	StopCacheThread();

	while(m_listDataGridCache[Direction].size())
	{
		delete m_listDataGridCache[Direction].front();
		m_listDataGridCache[Direction].pop_front();
	}
}

void *StartDataGridCacheThread(void *p)
{
	DataGridRenderer *dgr = (DataGridRenderer *)p;

	dgr->DataGridCacheThread();
	return NULL;
}

void DataGridRenderer::StartCacheThread()
{
	StopCacheThread();
	m_bStopCaching=false;
	pthread_create(&m_DataGridCacheThread, NULL, StartDataGridCacheThread, (void*)this);
}

void DataGridRenderer::StopCacheThread()
{
	if (m_DataGridCacheThread)
	{
		m_bStopCaching = true;
		pthread_join(m_DataGridCacheThread, NULL);
	}
}

void DataGridRenderer::DataGridCacheThread()
{
	int Direction, Step, MaxSteps = max(m_pDataGridOwner->m_iCacheRows,m_pDataGridOwner->m_iCacheColumns);
	int DirectionCur[5];
	for(Direction=1;Direction<=4;Direction++)
		DirectionCur[Direction]=(Direction <= 2) ? m_pDataGridOwner->m_GridCurRow : m_pDataGridOwner->m_GridCurCol;

	for(Step=1;Step<=MaxSteps && !m_bStopCaching;Step++)
	{
		for(Direction=1;Direction<=4 && !m_bStopCaching;Direction++)
		{
			if (DirectionCur[Direction]==-1)
				continue;

			int MaxCache = (Direction <= 2) ? m_pDataGridOwner->m_iCacheRows : m_pDataGridOwner->m_iCacheColumns;
			if (Step > MaxCache)
			{
				DirectionCur[Direction] = -1;
				continue;
			}

			PLUTO_SAFETY_LOCK( dgc, m_DataGridCacheMutex );

			int CurSize = (int)m_listDataGridCache[Direction].size();
			
			if (CurSize > MaxCache)
			{
				// There's more cache than the specified amount (probably as a result of moving). Delete the back.
				delete m_listDataGridCache[Direction].back();
				m_listDataGridCache[Direction].pop_back();
			}

			if (CurSize > Step) 
			{
				// This step is already cached.
				continue;
			}

			// Starting at the origin, we need to "walk" Step number of steps to request the appropriate grid.
			dgc.Release();

			// CalculateGridMovement is going to try to lock the m_DatagridMutex.  But it's possible a rendering thread
			// is locking it and will try to kill us.  So we're doing a peek to see if the thread is locked manually 
			// using trylock, and if it is, will keep checking if we're supposed to exit.  If it's not,
			// we need to manually unluck it below
			bool bKeepTryingToLock=true;
			while( bKeepTryingToLock )
			{
				int iResult = pthread_mutex_trylock(&m_pDataGridOwner->m_pOrbiter->m_DatagridMutex.mutex);
				if( iResult!=0 )
				{
					if( m_bStopCaching )
					{
						m_DataGridCacheThread = NULL;
						return;
					}
					else
						Sleep(50);  // Something else is rendering or something using grids, give it a moment
				}
				else
					bKeepTryingToLock=false;
			}

			if (!CalculateGridMovement( Direction, DirectionCur[Direction],  0 ))
			{				
				pthread_mutex_unlock(&m_pDataGridOwner->m_pOrbiter->m_DatagridMutex.mutex);  // See try lock above
				DirectionCur[Direction] = -1;
				continue;
			}
			pthread_mutex_unlock(&m_pDataGridOwner->m_pOrbiter->m_DatagridMutex.mutex);  // See try lock above
			dgc.Relock();

			int GridCurRow = (Direction <= 2) ? DirectionCur[Direction] : m_pDataGridOwner->m_GridCurRow;
			int GridCurCol = (Direction <= 2) ? m_pDataGridOwner->m_GridCurCol : DirectionCur[Direction];
			int size = 0;
			char *data = NULL;

			dgc.Release();

			DCE::CMD_Request_Datagrid_Contents CMD_Request_Datagrid_Contents( m_pDataGridOwner->m_pOrbiter->m_dwPK_Device,  m_pDataGridOwner->m_pOrbiter->m_dwPK_Device_DatagridPlugIn,
			StringUtils::itos( m_pDataGridOwner->m_pOrbiter->m_dwIDataGridRequestCounter ), m_pDataGridOwner->m_sGridID,
			m_pDataGridOwner->m_MaxRow, m_pDataGridOwner->m_MaxCol, m_pDataGridOwner->m_bKeepRowHeader, m_pDataGridOwner->m_bKeepColHeader, true, m_pDataGridOwner->m_sSeek, m_pDataGridOwner->m_iSeekColumn, &data, &size, &GridCurRow, &GridCurCol );
		
			if(  !m_pDataGridOwner->m_pOrbiter->SendCommand( CMD_Request_Datagrid_Contents )  )
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "Request datagrid: %s failed", m_pDataGridOwner->m_ObjectID.c_str(  ) );
				DirectionCur[Direction] = -1;
			}
			else
			{
				if ( size && data )
				{
					dgc.Relock();
					DataGridTable *pDataGridTable = new DataGridTable( size,  data );
					m_listDataGridCache[Direction].push_back(pDataGridTable);
					delete[] data;
					data = NULL;
					PLUTO_SAFETY_LOCK(M,m_pDataGridOwner->m_pOrbiter->Renderer()->m_bgImageReqMutex);
					for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
					{
						DataGridCell *pDataGridCell = it->second;
						const char *pPath = pDataGridCell->GetImagePath();
						if (pPath && !pDataGridCell->m_pGraphicData && !pDataGridCell->m_pGraphic)
 						{
							M.Release();
							m_pDataGridOwner->m_pOrbiter->Renderer()->BackgroundImageLoad(pPath, &pDataGridCell->m_pGraphic);              
							M.Relock();
						}
					}
				}
			}
			Sleep(0);
		}
	}
	m_DataGridCacheThread = NULL;
}

