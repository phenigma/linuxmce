#ifndef __DATAGRID_RENDERER_BLUETOOTH_H__
#define __DATAGRID_RENDERER_BLUETOOTH_H__

#include "../Orbiter/DataGridRenderer.h"

class DataGridRenderer_Bluetooth : public DataGridRenderer
{
public:
	DataGridRenderer_Bluetooth(DesignObj_Orbiter *pOwner);
	~DataGridRenderer_Bluetooth();

	void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
};

#endif //__DATAGRID_RENDERER_BLUETOOTH_H__