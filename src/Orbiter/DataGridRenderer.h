#ifndef __DATAGRID_RENDERER_H__
#define __DATAGRID_RENDERER_H__

#include "Orbiter.h"
#include "ObjectRenderer.h"
#include "DesignObj_Orbiter.h"

class TextStyle;

namespace DCE
{
	class DataGridCell;
};

class DataGridRenderer : public ObjectRenderer
{
	friend class MediaBrowserMouseHandler;  // This handles a special case caching with 2 grids that must stay sync'd

	pluto_pthread_mutex_t m_DataGridCacheMutex;
    pthread_t m_DataGridCacheThread;
	pthread_cond_t m_DataGridCacheCond;

	listDataGridTablePtr m_listDataGridCache[5];
	bool m_bStopCaching;
public:
	DataGridRenderer(DesignObj_Orbiter *pOwner);
	virtual ~DataGridRenderer(void);

	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	virtual void GetGridCellDimensions(int Colspan,  int Rowspan,  int Column,  int Row,  int &x,  int &y,  int &w,  int &h );
	virtual bool RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point = PlutoPoint(0, 0));
	virtual bool Scroll_Grid(string sRelative_Level, int iPK_Direction,bool bMoveOneLineIfCannotPage);
	virtual bool CalculateGridMovement(int Direction, int &Cur,  int CellsToSkip);
	virtual void Flush(bool bFlushGraphics) { FlushCache(); }

	void FlushCache(int Direction=0);
	void StartCacheThread();
	void StopCacheThread();
	void DataGridCacheThread();

protected:

	int GetAlphaLevel();
};

#endif //__DATAGRID_RENDERER_H__
