#ifndef DESIGNOBJ_DATAGRID_H
#define DESIGNOBJ_DATAGRID_H

#include "DesignObj_Orbiter.h"
#include "../DCE/DataGrid.h"

#define CACHE_SIZE 5 // The maximum number of directions

class DesignObj_DataGrid : public DesignObj_Orbiter 
{
public:
	DesignObj_DataGrid()
	{
		for(int i=0;i<CACHE_SIZE;++i)
			m_pDataGridTableCache[i]=NULL;

		m_pObjLeft=m_pObjRight=m_pObjUp=m_pObjDown=NULL;
		bReAcquire=false;
		m_dwIDownRow=m_iUpRow=-1;
		m_iPopulatedWidth=m_iPopulatedHeight=0;
		m_bParsed=false;
	}
	virtual ~DesignObj_DataGrid(); 

	virtual void RenderObject( DesignObj_Orbiter *pObj_Screen, PlutoPoint point, int Layer);

	int m_dwIDownRow,m_iUpRow;  // These are the rows which have up/down arrows.  If up==-1, there is no up arrow, same for down.  Otherwise it's 0 based
	int m_iHighlightedRow,m_iHighlightedColumn;
	int m_iPopulatedWidth,m_iPopulatedHeight; // The last known size during populate grid
	bool bReAcquire,m_bParsed;
	class DataGridTable *m_pDataGridTableCache[CACHE_SIZE];
	class TextStyle *m_pTextStyle,*m_pTextStyle_FirstCol,*m_pTextStyle_FirstRow,*m_pTextStyle_Selected,*m_pTextStyle_Highlighted;
	vector<class TextStyle *> m_vectTextStyle_Alt;

	string sSelVariable;
	bool m_bDontShowSelection,m_bIsMultiSelect,m_bKeepColHeader,m_bKeepRowHeader,m_bPersistXY,m_bHighlightSelectedCell;
	int m_iInitialColNum,m_iInitialRowNum,m_iPK_Datagrid,m_iPK_Variable,m_iSeekColumn,m_iPK_DeviceTemplate;
	string m_sGridID,m_sOptions,m_sExtraInfo,m_sSeek;
	DesignObj_Orbiter *m_pObjLeft,*m_pObjRight,*m_pObjUp,*m_pObjDown;

	bool CanGoUp();
	bool CanGoDown();
	bool CanGoLeft();
	bool CanGoRight();
	bool VerticalOnly();
	bool HasMoreUp();
	bool HasMoreDown();
};


#endif

