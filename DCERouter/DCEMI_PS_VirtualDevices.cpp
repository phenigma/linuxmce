#include "PlutoUtils/CommonIncludes.h"	
#include "Router.h"
#include "../pluto_main/Table_DeviceTemplate.h"
#include "../pluto_main/Table_Command.h"
#include "../pluto_main/Table_CommandParameter.h"
#include "../pluto_main/Table_FloorplanType.h"
#include "../pluto_main/Table_EventParameter.h"
#include "../pluto_main/Table_FloorplanObjectType_Color.h"
#include "../pluto_main/Table_DeviceData.h"
#include "../pluto_main/Table_HouseMode.h"
#include "../pluto_main/Table_DeviceCategory.h"
#include "../pluto_main/Table_Text.h"
#include "../pluto_main/Table_Event.h"
#include "../pluto_main/Table_DesignObj.h"
#include "../pluto_main/Table_Variable.h"
#include "../pluto_main/Table_FloorplanObjectType.h"
#include "PlutoOrbiter.h"
#include "MobileOrbiter.h"
#include "MediaAttributes.h"
#include "DCEMI_PS_VirtualDevices.h"

// all the datagrid generators are in a separate file
#include "DCEDG_PlutoServer.cpp"

DCEMI_PS_VirtualDevices::DCEMI_PS_VirtualDevices(class Router *pRouter,class PlutoEvents *pPlutoEvents) 
: PlutoPlugIn(pRouter), MySqlHelper(), m_DataGridMutex("datagrid"), m_MediaMutex("media")
{
	pthread_mutexattr_init(&m_MutexAttr);
	pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);

	m_DataGridMutex.Init(&m_MutexAttr);
	m_MediaMutex.Init(&m_MutexAttr);
	m_pPlutoEvents=pPlutoEvents;
	m_pMediaAttributes = new MediaAttributes(this);
	CheckForSpecialDevices();
}

DCEMI_PS_VirtualDevices::~DCEMI_PS_VirtualDevices()
{
	pthread_mutex_destroy(&m_DataGridMutex.mutex);
	pthread_mutex_destroy(&m_MediaMutex.mutex);
	pthread_mutexattr_destroy(&m_MutexAttr);
}

void DCEMI_PS_VirtualDevices::RegisterDatagridGenerator(class DataGridGeneratorCallBack *pCallBack,int PK_DataGrid)
{
	map<int,class DataGridGeneratorCallBack *>::iterator it = m_mapDataGridGeneratorCallBack.find(PK_DataGrid);
	if( it!=m_mapDataGridGeneratorCallBack.end() )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Two plug-ins tried to register for DataGrid %d",PK_DataGrid);
		return;
	}
	m_mapDataGridGeneratorCallBack[PK_DataGrid]=pCallBack;
}

void DCEMI_PS_VirtualDevices::RegisterMediaHandler(class MediaHandlerCallBack *pCallBack)
{
	m_listMediaHandlerCallBack.push_back(pCallBack);
}

void DCEMI_PS_VirtualDevices::CheckForSpecialDevices()
{
	map<int,class DeviceData_Router *>::iterator itDevices;
	for(itDevices=m_pRouter->m_mapDeviceData_Router.begin();
		itDevices!=m_pRouter->m_mapDeviceData_Router.end();
		++itDevices)
	{
		DeviceData_Router *pDevice = (*itDevices).second;
		if( !pDevice )
			continue; 

		if( pDevice->m_iPK_DeviceCategory==DEVICECATEGORY_ORBITER_CELL_CONST )
		{
			string MacAddress = pDevice->m_Parameters[C_DEVICEDATA_ID_CONST];
			if( MacAddress.length()>0 )
			{
				PlutoOrbiter *po = new PlutoOrbiter(this);
				MobileOrbiter *pmo = new MobileOrbiter(this,po,MacAddress,pDevice);
				m_mapMobileOrbiter[pDevice->m_iPK_Device]=pmo;
				m_mapMobileOrbiter_Mac[MacAddress]=pmo;
				m_mapPlutoOrbiter[pDevice->m_iPK_Device]=po;
				po->m_pDeviceData_Router = pDevice;
				po->m_pMobileOrbiter = pmo;
			}
		}
		else if( pDevice->WithinCategory(DEVICECATEGORY_ORBITER_TABLET_CONST) )
		{
			PlutoOrbiter *po = new PlutoOrbiter(this);
			m_mapPlutoOrbiter[pDevice->m_iPK_Device]=po;
			po->m_pDeviceData_Router = pDevice;
		}
	}
}

int DCEMI_PS_VirtualDevices::Register()
{











	RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&DCEMI_PS_VirtualDevices::PopulateWithDevicesCommands))
		,DGTYPE_DEVICES_ACTIONS);

	RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&DCEMI_PS_VirtualDevices::PopulateWithFileList))
		,DGTYPE_FILELIST);

	return DEVICETEMPLATE_SERVER_CONST;
}
