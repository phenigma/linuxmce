#ifndef __DATAGRID_RENDERER_PROXY_H__
#define __DATAGRID_RENDERER_PROXY_H__

#include "../Orbiter/DataGridRenderer.h"

class DataGridRenderer_Proxy : public DataGridRenderer
{
public:
	DataGridRenderer_Proxy(DesignObj_Orbiter *pOwner);
	~DataGridRenderer_Proxy();

	bool RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point = PlutoPoint(0, 0));
};

#endif //__DATAGRID_RENDERER_PROXY_H__