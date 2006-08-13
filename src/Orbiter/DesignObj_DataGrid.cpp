#include "DesignObj_DataGrid.h"
#include "Orbiter.h"
#include "OrbiterRenderer.h"
#include "DataGridRenderer.h"
#include "../DCE/DataGrid.h"

using namespace DCE;
//-------------------------------------------------------------------------------------------------------
DesignObj_DataGrid::DesignObj_DataGrid(Orbiter *pOrbiter) : DesignObj_Orbiter(pOrbiter)
{
	m_pObjLeft=m_pObjRight=m_pObjUp=m_pObjDown=NULL;
	m_pDataGridTable_Current=NULL;
	m_MaxRow=m_MaxCol=0;
	m_GridCurRow=-1;
	m_GridCurCol=-1;
	m_CachedCurRow=-1;
	m_CachedCurCol=-1;
	bReAcquire=false;
	m_iPopulatedWidth=m_iPopulatedHeight=0;
	m_bParsed=false;
	// TODO: These should come from the database

}
//-------------------------------------------------------------------------------------------------------
DesignObj_DataGrid::~DesignObj_DataGrid() 
{
	Flush();
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoUp()			
{ 
	return m_GridCurRow > 0;																
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoDown()		
{ 
	return DataGridTable_Get()->GetRows() > m_GridCurRow + m_MaxRow - (HasMoreUp() ? 1 : 0); 
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoLeft()		
{ 
	return m_GridCurCol > 0;																
}
//-------------------------------------------------------------------------------------------------------
bool DesignObj_DataGrid::CanGoRight()		
{ 
	return DataGridTable_Get()->GetCols() > m_GridCurCol + m_MaxCol;							
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
/*virtual*/ void DesignObj_DataGrid::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	m_pObjectRenderer->RenderObject(pObj_Screen, point);
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void DesignObj_DataGrid::PrepareRenderDataGrid(string& delSelections )
{
g_pPlutoLogger->Write(LV_ACTION, "Orbiter::AcquireGrid orbiter grid %s max row %d max col %d cur row %d cur col %d", m_sGridID.c_str(),m_MaxRow,m_MaxCol,m_GridCurRow,m_GridCurCol);
	DataGridTable *pDataGridTable = RequestDatagridContents( m_GridCurCol,  m_GridCurRow );
	m_pOrbiter->CallMaintenanceInMiliseconds(100,&Orbiter::StartCachingGrid,this,pe_Match_Data);
	m_PagesCached = make_pair<int,int> (0,0); // Start caching again

g_pPlutoLogger->Write(LV_ACTION, "Orbiter::AcquireGrid orbiter grid %s max row %d max col %d cur row %d cur col %d", m_sGridID.c_str(),m_MaxRow,m_MaxCol,m_GridCurRow,m_GridCurCol);
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING,"from grid %s DataGridTable_Get() is now %p",m_ObjectID.c_str(),DataGridTable_Get());
#endif

	m_CachedCurRow=m_GridCurRow;
	m_CachedCurCol=m_GridCurCol;
	m_pDataGridTable_Current=pDataGridTable;

	if (pDataGridTable )
	{
		// Hide arrow if not on the first row.
		if(m_pObjUp)
			m_pOrbiter->CMD_Show_Object( m_pObjUp->m_ObjectID, 0, "", "",  CanGoUp() ? "1" : "0" );
		// Hide down arrow if the curren row + rows < the total rows in the grid
		if(m_pObjDown)
			m_pOrbiter->CMD_Show_Object( m_pObjDown->m_ObjectID, 0, "", "",  CanGoDown() ? "1" : "0" );
		// Hide arrow if not on the first column.
		if(m_pObjLeft)
			m_pOrbiter->CMD_Show_Object( m_pObjLeft->m_ObjectID, 0, "", "",  CanGoLeft() ? "1" : "0" );
		// Hide down arrow if the curren row + rows < the total rows in the grid
		if(m_pObjRight)
			m_pOrbiter->CMD_Show_Object( m_pObjRight->m_ObjectID, 0, "", "",  CanGoRight() ? "1" : "0" );

	}
}
DataGridTable *DesignObj_DataGrid::RequestDatagridContents( int &GridCurCol, int &GridCurRow )
{
	PLUTO_SAFETY_LOCK( vm, m_pOrbiter->m_VariableMutex );
	DataGridTable *pDataGridTable = DataGridTable_Get(GridCurRow,GridCurCol);
	if (  bReAcquire || !pDataGridTable || m_sSeek.length() )
	{
g_pPlutoLogger->Write(LV_ACTION,"acquiring %s",m_ObjectID.c_str());
		bReAcquire=false;

		++m_pOrbiter->m_dwIDataGridRequestCounter;

#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS,  "requesting grid ( # %d ),  %s.",  m_pOrbiter->m_dwIDataGridRequestCounter,
			m_sGridID.c_str(  ) );
#endif
		int size = 0;
		char *data = NULL;

		DCE::CMD_Request_Datagrid_Contents CMD_Request_Datagrid_Contents( m_pOrbiter->m_dwPK_Device,  m_pOrbiter->m_dwPK_Device_DatagridPlugIn,
			StringUtils::itos( m_pOrbiter->m_dwIDataGridRequestCounter ), m_sGridID,
			m_MaxRow, m_MaxCol, m_bKeepRowHeader, m_bKeepColHeader, true, m_sSeek, m_iSeekColumn, &data, &size, &GridCurRow, &GridCurCol );

		if(  !m_pOrbiter->SendCommand( CMD_Request_Datagrid_Contents )  )
			g_pPlutoLogger->Write( LV_CRITICAL, "Request datagrid: %s failed", m_ObjectID.c_str(  ) );
		else
		{
			if ( size && data )
			{
				// See if we should add page up/down cells -- see notes at top of file
				bool bAddArrows = m_sExtraInfo.find( 'P' )!=string::npos;
				
				if( bAddArrows && GridCurRow>0 )
					pDataGridTable = new DataGridTable( size,  data, true );
				else
					pDataGridTable = new DataGridTable( size,  data );

				delete[] data;
				data = NULL;

				DataGridTable_Set(pDataGridTable,GridCurRow, GridCurCol );
				pDataGridTable->m_iUpRow=pDataGridTable->m_iDownRow=-1;
				if( bAddArrows )
				{
					if( GridCurRow>0  )
					{
						pDataGridTable->m_iUpRow = GridCurRow + (pDataGridTable->m_bKeepRowHeader ? 1 : 0);
						DataGridCell * pCell = new DataGridCell( "<Scroll up>" );
						pCell->m_Colspan = pDataGridTable->m_ColumnCount;
						pDataGridTable->SetData(GridCurCol,pDataGridTable->m_iUpRow,pCell);
					}

					if ( pDataGridTable->GetRows() > GridCurRow + m_MaxRow - (pDataGridTable->m_iUpRow!=-1 ? 1 : 0) )
					{
						pDataGridTable->m_iDownRow = GridCurRow+m_MaxRow-1;
						DataGridCell * pCell = new DataGridCell( "<Scroll down>" );
						pCell->m_Colspan = pDataGridTable->m_ColumnCount;
						pDataGridTable->SetData(GridCurCol,pDataGridTable->m_iDownRow,pCell);
					}
				}

#ifdef DEBUG
				g_pPlutoLogger->Write( LV_STATUS, "Got %d rows %d cols in new %s DataGridTable_Get() %p", pDataGridTable->GetRows(  ), pDataGridTable->GetCols(  ), m_ObjectID.c_str(), pDataGridTable );
#endif
				if(  !pDataGridTable->GetRows(  ) || !pDataGridTable->GetCols(  )  )
				{
					// Initialize grid will set these to 0,  assuming there will be data.  If the grid is empty,  change that
					m_iHighlightedRow=-1;
					m_iHighlightedColumn=-1;
				}
				m_sSeek="";  // Only do the seek 1 time
			}
			else
			{
				g_pPlutoLogger->Write( LV_CRITICAL,  "Error loading grid ID: %d (not implemented?)",  m_pOrbiter->m_dwIDataGridRequestCounter );
			}
		}
	}
	return pDataGridTable;
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ bool DesignObj_DataGrid::RenderCell(class DataGridTable *pT,  class DataGridCell *pCell,  
	int j,  int i,  int GraphicToDisplay, PlutoPoint point )
{
	DataGridRenderer *pDataGridRenderer = dynamic_cast<DataGridRenderer *>(m_pObjectRenderer);

	if(NULL != pDataGridRenderer)
		return pDataGridRenderer->RenderCell(pT, pCell, j, i, GraphicToDisplay, point);

	return false;
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void DesignObj_DataGrid::GetGridCellDimensions(int Colspan,  int Rowspan,  int Column,  int Row,  
	int &x,  int &y,  int &w,  int &h )
{
	DataGridRenderer *pDataGridRenderer = dynamic_cast<DataGridRenderer *>(m_pObjectRenderer);

	if(NULL != pDataGridRenderer)
		pDataGridRenderer->GetGridCellDimensions(Colspan, Rowspan, Column, Row, x, y, w, h);
}
//-------------------------------------------------------------------------------------------------------
DataGridTable *DesignObj_DataGrid::DataGridTable_Get(int CurRow,int CurCol)
{
	PLUTO_SAFETY_LOCK( vm, m_pOrbiter->m_DatagridMutex );
	if( CurRow==-1 )
		CurRow=m_GridCurRow;
	if( CurCol==-1 )
		CurCol=m_GridCurCol;

	if( m_CachedCurRow!=CurRow || m_CachedCurCol!=CurCol )
	{
		map< pair<int,int>, DataGridTable *>::iterator it=m_mapDataGridTable_Cache.find( make_pair<int,int> (CurRow,CurCol) );
		if( it==m_mapDataGridTable_Cache.end() )
			return NULL;
		else
			return it->second;
	}
	return m_pDataGridTable_Current;
}

void DesignObj_DataGrid::DataGridTable_Set(DataGridTable *pDataGridTable,int CurRow,int CurCol)
{
	PLUTO_SAFETY_LOCK( vm, m_pOrbiter->m_DatagridMutex );
	map< pair<int,int>, DataGridTable *>::iterator it=m_mapDataGridTable_Cache.find( make_pair<int,int> (CurRow,CurCol) );
	if( it!=m_mapDataGridTable_Cache.end() )
	{
		if( it->second && it->second!=pDataGridTable ) // The grids have been changed
			Flush();
		else
			return; // Nothing to do
	}
	m_mapDataGridTable_Cache[ make_pair<int,int> (CurRow,CurCol) ] = pDataGridTable;
}

/*virtual*/ void DesignObj_DataGrid::Flush(bool bFlushGraphics)
{
	DesignObj_Orbiter::Flush(bFlushGraphics);
	m_pDataGridTable_Current=NULL;
	for(map< pair<int,int>, DataGridTable *>::iterator it=m_mapDataGridTable_Cache.begin();it!=m_mapDataGridTable_Cache.end();++it)
		delete it->second;
	m_mapDataGridTable_Cache.clear();
	m_PagesCached = make_pair<int,int> (0,0);
	m_CurrentLocation = make_pair<int,int> (0,0);
}

void DesignObj_DataGrid::CacheGrid()
{
	PLUTO_SAFETY_LOCK( dg, m_pOrbiter->m_DatagridMutex );
	// Cache down, then right, then up, then left
	if( m_PagesCached.first==0 )
	{
		m_PagesCached.first=DIRECTION_Down_CONST;
		m_PagesCached.second=0;
		m_CurrentLocation = make_pair<int,int> (m_GridCurRow,m_GridCurCol);
	}
	else if( m_PagesCached.first==DIRECTION_Down_CONST && m_PagesCached.second>=m_mapNumberOfPagesToCache[DIRECTION_Down_CONST] )
	{
		m_PagesCached.first=DIRECTION_Right_CONST;
		m_PagesCached.second=0;
		m_CurrentLocation = make_pair<int,int> (m_GridCurRow,m_GridCurCol);
	}
	else if( m_PagesCached.first==DIRECTION_Right_CONST && m_PagesCached.second>=m_mapNumberOfPagesToCache[DIRECTION_Right_CONST] )
	{
		m_PagesCached.first=DIRECTION_Up_CONST;
		m_PagesCached.second=0;
		m_CurrentLocation = make_pair<int,int> (m_GridCurRow,m_GridCurCol);
	}
	else if( m_PagesCached.first==DIRECTION_Up_CONST && m_PagesCached.second>=m_mapNumberOfPagesToCache[DIRECTION_Up_CONST] )
	{
		m_PagesCached.first=DIRECTION_Left_CONST;
		m_PagesCached.second=0;
		m_CurrentLocation = make_pair<int,int> (m_GridCurRow,m_GridCurCol);
	}
	else if( m_PagesCached.first==DIRECTION_Left_CONST && m_PagesCached.second>=m_mapNumberOfPagesToCache[DIRECTION_Left_CONST] )
		return;  // We've finished caching in all directions

	DataGridTable *pDataGridTable = DataGridTable_Get( m_CurrentLocation.first, m_CurrentLocation.second );
	if( !pDataGridTable )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"DesignObj_DataGrid::CacheGrid  -- no current grid"); // Shouldn't happen
		return;
	}
	bool bResult;
	if( m_PagesCached.first==DIRECTION_Up_CONST || m_PagesCached.first==DIRECTION_Down_CONST )
		bResult = CalculateGridMovement(  m_PagesCached.first, m_CurrentLocation.first, 0, pDataGridTable );
	else
		bResult = CalculateGridMovement(  m_PagesCached.first, m_CurrentLocation.second, 0, pDataGridTable );
	if( bResult )  // We succeeded
	{
		pDataGridTable = RequestDatagridContents(m_CurrentLocation.second,m_CurrentLocation.first);  // Get the grid at this location
		m_PagesCached.second++;
	}
	else
		m_PagesCached.second = m_mapNumberOfPagesToCache[m_PagesCached.first];  // We've hit the end, so set this to the maximum so we'll start in the other direction next time
	m_pOrbiter->CallMaintenanceInMiliseconds(10,&Orbiter::StartCachingGrid,this,pe_Match_Data); // Start again after just a short 10 ms pause to let other tasks process
}

bool DesignObj_DataGrid::CalculateGridMovement(int Direction, int &Cur,  int CellsToSkip, DataGridTable *pDataGridTable)
{
	if( !pDataGridTable )
		pDataGridTable = DataGridTable_Get();
	PLUTO_SAFETY_LOCK( dg, m_pOrbiter->m_DatagridMutex );
	if( !pDataGridTable )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"DesignObj_DataGrid::CalculateGridMovement no current grid");
		return false;
	}
	int InitialCur = Cur;

	switch(Direction)
	{
	case DIRECTION_Up_CONST:
		if ( CellsToSkip == 0 )
			CellsToSkip = m_MaxRow - ( pDataGridTable->m_bKeepRowHeader ? 1 : 0 );

		// Are we going to display 'scroll up/down' cells?  If so, we may not be able to scroll up a full page
		if(  m_sExtraInfo.find( 'P' )!=string::npos  )
		{
			// After doing the scroll, will we still be able to go up?  If so, reduce the cells to skip by 1
			if( Cur - CellsToSkip > 0)
				CellsToSkip--;
			// After doing the scroll, will we still be able to go down?  If so, reduce the cells to skip by 1
			if( pDataGridTable->GetRows() > (Cur - CellsToSkip) + m_MaxRow )
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
			if ( CellsToSkip == 0 )
				CellsToSkip = m_MaxRow - ( pDataGridTable->m_bKeepRowHeader ? 1 : 0 );
		
			bool bCanGoDown = pDataGridTable->m_iDownRow >= 0;
			bool bCanGoUp = pDataGridTable->m_iUpRow >= 0;
			CellsToSkip -= ( bCanGoDown + bCanGoUp );

			if ( CellsToSkip < 0 )
				CellsToSkip = 0;

			if( Cur+CellsToSkip >= pDataGridTable->GetRows() )
				return false;

			Cur += CellsToSkip;
/*
			if ( Cur+m_MaxRow > pDataGridTable->GetRows(  ) )
			{
				// Add an extra 1,  becuase if we end  up making the top row an up ( unknown at this point )
				// We'll be one short
				Cur = pDataGridTable->GetRows(  ) - m_MaxRow + 1;
				if ( Cur<0 )
					Cur=0;
			}
			*/
		}
		break;
	case DIRECTION_Left_CONST:
		if ( CellsToSkip==0 )
			CellsToSkip = m_MaxCol-( pDataGridTable->m_bKeepColumnHeader ? 2 : 1 );
		if ( CellsToSkip<=0 )
			CellsToSkip = 1;

		Cur-=CellsToSkip;
		if ( Cur<0 )
			Cur=0;
		break;
	case DIRECTION_Right_CONST:
		if ( CellsToSkip==0 )
			CellsToSkip = m_MaxCol-( pDataGridTable->m_bKeepColumnHeader ? 2 : 1 );
		if ( CellsToSkip<=0 )
			CellsToSkip = 1;

		if( Cur+CellsToSkip >= pDataGridTable->GetCols() )
			return false;

		Cur+=CellsToSkip;
/*
		if ( Cur+m_MaxCol > pDataGridTable->GetCols(  ) )
		{
			Cur = pDataGridTable->GetCols(  ) - m_MaxCol;
			if ( Cur<0 )
				Cur=0;
		}
		*/
		break;
	}
	return (Cur!=InitialCur);
}

bool DesignObj_DataGrid::Scroll_Grid(string sRelative_Level, int iPK_Direction,bool bMoveOneLineIfCannotPage)
{
	if ( DataGridTable_Get() )
	{
		if(  sRelative_Level=="-1"  )
		{
			if(  iPK_Direction == DIRECTION_Up_CONST  )
				m_GridCurRow = 0;
		}
		else
		{
//			int CurrentRow = m_GridCurRow;
//			int CurrentCol = m_GridCurCol;
			if(  iPK_Direction == DIRECTION_Up_CONST )
			{
				if (!CalculateGridMovement(DIRECTION_Up_CONST, m_GridCurRow,  atoi( sRelative_Level.c_str(  ) ) ))
				{
					if( bMoveOneLineIfCannotPage )
						m_pOrbiter->CMD_Move_Up();
					return false;
				}
/*				
				if (m_iCacheRows > 0) // Are we caching? 
				{                            // If so the current table needs to be pushed onto the opposing cache list.
//					m_listDataGridCache[DIRECTION_Down_CONST].push_front(DataGridTable_Get()); // The cache acquisition thread will delete the excess cache.
				}
				else 
				{
					// delete DataGridTable_Get();
				}	

				if (m_listDataGridCache[DIRECTION_Up_CONST].size() > 0)
				{
					DataGridTable_Get() = m_listDataGridCache[DIRECTION_Up_CONST].front();
					m_listDataGridCache[DIRECTION_Up_CONST].pop_front();
				}
				else
				{
					DataGridTable_Get() = NULL;
					bReAcquire=true;
				}
*/
			}
			else if(  iPK_Direction == DIRECTION_Down_CONST  )
			{
				if (!CalculateGridMovement(DIRECTION_Down_CONST, m_GridCurRow,  atoi( sRelative_Level.c_str(  ) ) ) )
				{
					if (DataGridTable_Get()->getTotalRowCount() > 0 && bMoveOneLineIfCannotPage)
					{
						m_pOrbiter->CMD_Move_Down();
					}
					return false;
				}
/*
				PLUTO_SAFETY_LOCK( dgc, m_DataGridCacheMutex );
				if (m_iCacheRows > 0) // Are we caching? 
				{                            // If so the current table needs to be pushed onto the opposing cache list.
					m_listDataGridCache[DIRECTION_Up_CONST].push_front(DataGridTable_Get()); // The cache acquisition thread will delete the excess cache.
				}
				else 
				{
					//delete DataGridTable_Get();
				}	
				Flush(DIRECTION_Left_CONST);
				Flush(DIRECTION_Right_CONST);

				if (m_listDataGridCache[DIRECTION_Down_CONST].size() > 0)
				{
					DataGridTable_Get() = m_listDataGridCache[DIRECTION_Down_CONST].front();
					m_listDataGridCache[DIRECTION_Down_CONST].pop_front();
				}
				else
				{
					DataGridTable_Get() = NULL;
					bReAcquire=true;
				}
*/
			}
			else if(  iPK_Direction == DIRECTION_Left_CONST  )
			{
				if (!CalculateGridMovement(DIRECTION_Left_CONST,  m_GridCurCol,  atoi( sRelative_Level.c_str(  ) ) ))
				{
					if( bMoveOneLineIfCannotPage )
						m_pOrbiter->CMD_Move_Left();
					return false;
				}
/*
				PLUTO_SAFETY_LOCK( dgc, m_DataGridCacheMutex );

				if (m_iCacheRows > 0) // Are we caching? 
				{                            // If so the current table needs to be pushed onto the opposing cache list.
					m_listDataGridCache[DIRECTION_Right_CONST].push_front(DataGridTable_Get()); // The cache acquisition thread will delete the excess cache.
				}
				else 
				{
					//delete DataGridTable_Get();
				}	
				Flush(DIRECTION_Left_CONST);
				Flush(DIRECTION_Right_CONST);

				if (m_listDataGridCache[DIRECTION_Left_CONST].size() > 0)
				{
					DataGridTable_Get() = m_listDataGridCache[DIRECTION_Left_CONST].front();
					m_listDataGridCache[DIRECTION_Left_CONST].pop_front();
				}
				else
				{
					DataGridTable_Get() = NULL;
					bReAcquire=true;
				}
				*/
			}
			else if(  iPK_Direction == DIRECTION_Right_CONST  )
			{
				if (!CalculateGridMovement(DIRECTION_Right_CONST,  m_GridCurCol,  atoi( sRelative_Level.c_str(  ) ) ))
				{
					if( bMoveOneLineIfCannotPage )
						m_pOrbiter->CMD_Move_Right();
					return false;
				}
				/*
				PLUTO_SAFETY_LOCK( dgc, m_DataGridCacheMutex );

				if (m_iCacheRows > 0) // Are we caching? 
				{                            // If so the current table needs to be pushed onto the opposing cache list.
					m_listDataGridCache[DIRECTION_Left_CONST].push_front(DataGridTable_Get()); // The cache acquisition thread will delete the excess cache.
				}
				else 
				{
					//delete DataGridTable_Get();
				}	
				Flush(DIRECTION_Up_CONST);
				Flush(DIRECTION_Down_CONST);

				if (m_listDataGridCache[DIRECTION_Right_CONST].size() > 0)
				{
					DataGridTable_Get() = m_listDataGridCache[DIRECTION_Right_CONST].front();
					m_listDataGridCache[DIRECTION_Right_CONST].pop_front();
				}
				else
				{
					DataGridTable_Get() = NULL;
					bReAcquire=true;
				}
				*/
			}
		}
	}
	m_pOrbiter->Renderer()->RenderObjectAsync(this);
	return true;
}
