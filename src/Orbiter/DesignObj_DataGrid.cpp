#include "DesignObj_DataGrid.h"
#include "Orbiter.h"
#include "OrbiterRenderer.h"
#include "DataGridRenderer.h"
#include "../DCE/DataGrid.h"

using namespace DCE;
//-------------------------------------------------------------------------------------------------------
DesignObj_DataGrid::DesignObj_DataGrid(Orbiter *pOrbiter) : DesignObj_Orbiter(pOrbiter)
{
	for(int i=0;i<CACHE_SIZE;++i)
		m_pDataGridTableCache[i]=NULL;

	m_pObjLeft=m_pObjRight=m_pObjUp=m_pObjDown=NULL;
	bReAcquire=false;
	m_dwIDownRow=m_iUpRow=-1;
	m_iPopulatedWidth=m_iPopulatedHeight=0;
	m_bParsed=false;
}
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
/*virtual*/ void DesignObj_DataGrid::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	m_pObjectRenderer->RenderObject(pObj_Screen, point);
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void DesignObj_DataGrid::PrepareRenderDataGrid(string& delSelections )
{
	m_pOrbiter->AcquireGrid(this,  m_GridCurCol,  m_GridCurRow,  m_pDataGridTable );
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING,"from grid %s m_pDataGridTable is now %p",m_ObjectID.c_str(),m_pDataGridTable);
#endif

	if (m_pDataGridTable )
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

		// number of the row that has the respective scroll arrows
		m_dwIDownRow = -1;
		m_iUpRow = -1;
	}
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
