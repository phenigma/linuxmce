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

	int m_dwIDownRow,m_iUpRow;  // These are the rows which have up/down arrows.  If up==-1, there is no up arrow, same for down.  Otherwise it's 0 based
	int m_iHighlightedRow,m_iHighlightedColumn;
	int m_iPopulatedWidth,m_iPopulatedHeight; // The last known size during populate grid
	bool bReAcquire,m_bParsed;
	DataGridTable *m_pDataGridTableCache[CACHE_SIZE];
	TextStyle *m_pTextStyle,*m_pTextStyle_FirstCol,*m_pTextStyle_FirstRow,*m_pTextStyle_Selected,*m_pTextStyle_Highlighted;
	vector<TextStyle *> m_vectTextStyle_Alt;

	string sSelVariable;
	bool m_bDontShowSelection,m_bIsMultiSelect,m_bKeepColHeader,m_bKeepRowHeader,m_bPersistXY,m_bHighlightSelectedCell;
	int m_iInitialColNum,m_iInitialRowNum,m_iPK_Datagrid,m_iPK_Variable,m_iSeekColumn,m_iPK_DeviceTemplate;
	string m_sGridID,m_sOptions,m_sExtraInfo,m_sSeek;
	DesignObj_Orbiter *m_pObjLeft, *m_pObjRight, *m_pObjUp, *m_pObjDown;

	bool CanGoUp();
	bool CanGoDown();
	bool CanGoLeft();
	bool CanGoRight();
	bool VerticalOnly();
	bool HasMoreUp();
	bool HasMoreDown();

	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	virtual void GetGridCellDimensions(int Colspan,  int Rowspan,  int Column,  int Row,  int &x,  int &y,  int &w,  int &h );
	virtual bool RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point = PlutoPoint(0, 0));
	virtual void PrepareRenderDataGrid(string& sDelSelections);
};
//-------------------------------------------------------------------------------------------------------
#endif //__DESIGNOBJ_DATAGRID_H__
