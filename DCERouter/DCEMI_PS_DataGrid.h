#ifndef DCEMI_PS_DataGrid_h
#define DCEMI_PS_DataGrid_h

#include "PlugIn.h"

typedef map<string, class DataGridTable *> DataGridMap;

class DataGridGeneratorPlugIn {
};

// If a non-null pointer is returned, the framework will remove any existing datagrid by this name and
// replace it with this one.
typedef class DataGridTable *(DataGridGeneratorPlugIn::*DCEDataGridGeneratorFn)(string GridID,string Parms,void *pExtraData);

// Contains a pointer to the datagrid callback
class DataGridGeneratorCallBack
{
public:
	class DataGridGeneratorPlugIn *m_pDataGridGeneratorPlugIn;
	DCEDataGridGeneratorFn m_pDCEDataGridGeneratorFn;
	DataGridGeneratorCallBack(class DataGridGeneratorPlugIn *pDataGridGeneratorPlugIn,DCEDataGridGeneratorFn pDCEDataGridGeneratorFn)
	{
		m_pDataGridGeneratorPlugIn=pDataGridGeneratorPlugIn;
		m_pDCEDataGridGeneratorFn=pDCEDataGridGeneratorFn;
	}
};


class DCEMI_PS_DataGrid : public PlugIn
{
public:
	pluto_pthread_mutex_t m_DataGridMutex;
	DataGridMap m_DataGrids;
	bool RequestGridData(string GridID, int ColStart, int RowStart, int ColCount, int RowCount, bool bKeepColumnHeader, bool bKeepRowHeader, int &TableDataSize, char* &TableData);
	void RegisterDatagridGenerator(class DataGridGeneratorCallBack *pCallBack,int PK_DataGrid);
	map<int,class DataGridGeneratorCallBack *> m_mapDataGridGeneratorCallBack;

	// Required overrides
	DCEMI_PS_DataGrid(class Router *pRouter) : PlugIn(pRouter), m_DataGridMutex("DataGrid") {}
	virtual bool Register();
	virtual int ID_get() { return DCEPLUGIN_PS_Data_Grid_CONST; }
	virtual string Description_get() { return "Pluto DCE Plug-in: Datagrid interceptors v: 2.0"; }

	/*
		Message Interceptors
	*/

	// Handle an event indicating that the number of messages for a user has changed
	bool RequestDataGridPosition(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
	bool PopulateDataGrid(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo); 
	bool RequestDataGrid(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
};

#endif

