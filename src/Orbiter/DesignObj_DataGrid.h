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
	int m_iHighlightedRow,m_iHighlightedColumn;
	int m_iPopulatedWidth,m_iPopulatedHeight; // The last known size during populate grid
	bool bReAcquire,m_bParsed;
	TextStyle *m_pTextStyle,*m_pTextStyle_FirstCol,*m_pTextStyle_FirstRow,*m_pTextStyle_Selected,*m_pTextStyle_Highlighted;
	vector<TextStyle *> m_vectTextStyle_Alt;

	string sSelVariable;
	bool m_bDontShowSelection,m_bIsMultiSelect,m_bKeepColHeader,m_bKeepRowHeader,m_bPersistXY,m_bHighlightSelectedCell;
	int m_iInitialColNum,m_iInitialRowNum,m_iPK_Datagrid,m_iPK_Variable,m_iSeekColumn,m_iPK_DeviceTemplate;
	string m_sGridID,m_sOptions,m_sExtraInfo,m_sSeek;
	DesignObj_Orbiter *m_pObjLeft, *m_pObjRight, *m_pObjUp, *m_pObjDown;
	DataGridTable *m_pDataGridTable_Current;
	int m_CachedCurRow,m_CachedCurCol;
	map< pair<int,int>, DataGridTable *> m_mapDataGridTable_Cache; // Map Row,Col to the cached grid.  These are all pages, or views of the grid that we've cached
	map<int,int> m_mapNumberOfPagesToCache;  // Map of PK_Direction to a number of pages to cache in that direction
	pair<int,int> m_PagesCached,m_CurrentLocation; // First int=PK_Direction, 2nd is how many pages it has cached in that direction.  This is for tracking the progress
	DataGridTable *DataGridTable_Get(int CurRow=-1,int CurCol=-1);
	void DataGridTable_Set(DataGridTable *pDataGridTable,int CurRow,int CurCol);
	/**
	* @brief Fetches the contents of the datagrid at the given start points
	*/
	class DataGridTable* RequestDatagridContents( int &GridCurCol, int &GridCurRow );

	bool CanGoUp();
	bool CanGoDown();
	bool CanGoLeft();
	bool CanGoRight();
	bool VerticalOnly();
	bool HasMoreUp();
	bool HasMoreDown();
	void CacheGrid(); // Cache one more page of grid
	virtual bool CalculateGridMovement(int Direction, int &Cur,  int CellsToSkip, DataGridTable *pDataGridTable=NULL);

	virtual void Flush(bool bFlushGraphics=false); // Flush data that is cached with this object, such as the contents of a datagrid.  If bFlushGraphics, then any cached graphics are also purged
	virtual bool Scroll_Grid(string sRelative_Level, int iPK_Direction,bool bMoveOneLineIfCannotPage);

	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	virtual void GetGridCellDimensions(int Colspan,  int Rowspan,  int Column,  int Row,  int &x,  int &y,  int &w,  int &h );
	virtual bool RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point = PlutoPoint(0, 0));
	virtual void PrepareRenderDataGrid(string& sDelSelections);
};
//-------------------------------------------------------------------------------------------------------
#endif //__DESIGNOBJ_DATAGRID_H__
