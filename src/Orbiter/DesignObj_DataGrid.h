#ifndef __DESIGNOBJ_DATAGRID_H__
#define __DESIGNOBJ_DATAGRID_H__
//-------------------------------------------------------------------------------------------------------
#include "DesignObj_Orbiter.h"
//-------------------------------------------------------------------------------------------------------
class TextStyle;

namespace DCE
{
	class DataGridCell;
};


//-------------------------------------------------------------------------------------------------------
class DesignObj_DataGrid : public DesignObj_Orbiter 
{
	friend class DataGridRenderer;

public:
	DesignObj_DataGrid(Orbiter *pOrbiter);
	virtual ~DesignObj_DataGrid(); 

	int m_GridCurRow, m_GridCurCol;  // The current top, left grid and column being displayed.  Zero based.  Example: In a grid with 10 rows per page, on the second page m_GridCurRow will = 10
	int m_MaxRow, m_MaxCol; // The total number of rows and columns visible on the screen.  Not related to the actual rows and columns in the grid.  
	int m_FixedRowHeight, m_FixedColumnWidth, m_RowSpacing;
	int m_ColumnSpacing, m_FirstRowHeight, m_FirstColumnWidth;
	int m_dwIDownRow,m_iUpRow;  // These are the rows which have up/down arrows.  If up==-1, there is no up arrow, same for down.  Otherwise it's 0 based
	int m_iHighlightedRow,m_iHighlightedColumn;
	int m_iPopulatedWidth,m_iPopulatedHeight; // The last known size during populate grid
	bool bReAcquire,m_bParsed;
	int m_iCacheColumns, m_iCacheRows;
	TextStyle *m_pTextStyle,*m_pTextStyle_FirstCol,*m_pTextStyle_FirstRow,*m_pTextStyle_Selected,*m_pTextStyle_Highlighted;
	vector<TextStyle *> m_vectTextStyle_Alt;

	string sSelVariable;
	bool m_bDontShowSelection,m_bIsMultiSelect,m_bKeepColHeader,m_bKeepRowHeader,m_bPersistXY,m_bHighlightSelectedCell;
	int m_iInitialColNum,m_iInitialRowNum,m_iPK_Datagrid,m_iPK_Variable,m_iSeekColumn,m_iPK_DeviceTemplate;
	string m_sGridID,m_sOptions,m_sExtraInfo,m_sSeek;
	DesignObj_Orbiter *m_pObjLeft, *m_pObjRight, *m_pObjUp, *m_pObjDown;
	DataGridTable *m_pDataGridTable_Current;
	int m_CachedCurRow,m_CachedCurCol;
	DataGridTable *m_pDataGridTable_Current;
	map< pair<int,int>, DataGridTable *> m_mapDataGridTable_Cache; // Map Row,Col to the cached grid.  These are all pages, or views of the grid that we've cached
	DataGridTable *DataGridTable_Get() 
	{
		PLUTO_SAFETY_LOCK( vm, m_pOrbiter->m_VariableMutex );
		if( m_CachedCurRow!=m_GridCurRow || m_CachedCurCol!=m_GridCurCol )
		{
			map< pair<int,int>, DataGridTable *>::iterator it=m_mapDataGridTable_Cache.find( make_pair<int,int> (m_GridCurRow,m_GridCurCol) );
			if( it==m_mapDataGridTable_Cache.end() )
				m_pDataGridTable_Current = NULL;
			else
				m_pDataGridTable_Current = it->second;
		}
		return m_pDataGridTable_Current;
	

	bool CanGoUp();
	bool CanGoDown();
	bool CanGoLeft();
	bool CanGoRight();
	bool VerticalOnly();
	bool HasMoreUp();
	bool HasMoreDown();

	void FlushCache();

	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	virtual void GetGridCellDimensions(int Colspan,  int Rowspan,  int Column,  int Row,  int &x,  int &y,  int &w,  int &h );
	virtual bool RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point = PlutoPoint(0, 0));
	virtual void PrepareRenderDataGrid(string& sDelSelections);
};
//-------------------------------------------------------------------------------------------------------
#endif //__DESIGNOBJ_DATAGRID_H__
