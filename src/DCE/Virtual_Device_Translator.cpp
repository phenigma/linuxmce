#include "PlutoUtils/CommonIncludes.h"
#include "DCE/Logger.h"
#include "DCE/DeviceData_Base.h"
#include "Virtual_Device_Translator.h"
#include "pluto_main/Define_DeviceTemplate.h"

using namespace DCE;

bool Virtual_Device_Translator::GetConfig(DeviceData_Base *pDeviceData_Base)
{
	m_pDeviceData_Base=pDeviceData_Base;
	m_dwPK_Device_MediaDirector=m_dwPK_Device_DiscDrive=m_dwPK_Device_AppServer=m_dwPK_Device_MediaBurner=m_dwPK_Device_VideoConf=m_dwPK_Device_CamcorderCapt=m_dwPK_Device_Orbiter=m_dwPK_Device_WebBrowser=0;
	m_dwPK_Device_Router=m_dwPK_Device_DatagridPlugIn=m_dwPK_Device_InfraredPlugIn=m_dwPK_Device_GeneralInfoPlugIn=m_dwPK_Device_EventPlugIn=m_dwPK_Device_OrbiterPlugIn=
		m_dwPK_Device_LightingPlugIn=m_dwPK_Device_ClimatePlugIn=m_dwPK_Device_MediaPlugIn=m_dwPK_Device_TelecomPlugIn=m_dwPK_Device_SecurityPlugIn=
		m_dwPK_Device_LocalAppServer=0;

	DeviceData_Base *pDevice_ControlledVia = m_pDeviceData_Base->m_AllDevices.m_mapDeviceData_Base_Find( m_pDeviceData_Base->m_dwPK_Device_ControlledVia );

	for(Map_DeviceData_Base::iterator it=m_pDeviceData_Base->m_AllDevices.m_mapDeviceData_Base.begin();it!=m_pDeviceData_Base->m_AllDevices.m_mapDeviceData_Base.end();++it)
	{
		DeviceData_Base *pDeviceData_Base = (*it).second;
		switch(pDeviceData_Base->m_dwPK_DeviceCategory)
		{
		case DEVICECATEGORY_DCE_Router_CONST:
			m_dwPK_Device_Router = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_Lighting_Plugins_CONST:
			m_dwPK_Device_LightingPlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_Climate_Plugins_CONST:
			m_dwPK_Device_ClimatePlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_Media_Plugins_CONST:
			m_dwPK_Device_MediaPlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_Telecom_Plugins_CONST:
			m_dwPK_Device_TelecomPlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_Security_Plugins_CONST:
			m_dwPK_Device_SecurityPlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_Datagrid_Plugins_CONST:
			m_dwPK_Device_DatagridPlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_General_Info_Plugins_CONST:
			m_dwPK_Device_GeneralInfoPlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_Infrared_Plugins_CONST:
			m_dwPK_Device_InfraredPlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_Event_Plugins_CONST:
			m_dwPK_Device_EventPlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		case DEVICECATEGORY_Orbiter_Plugins_CONST:
			m_dwPK_Device_OrbiterPlugIn = pDeviceData_Base->m_dwPK_Device;
			break;
		};

		if( pDevice_ControlledVia && pDeviceData_Base->IsChildOf( pDevice_ControlledVia ) )
		{
			switch(pDeviceData_Base->m_dwPK_DeviceCategory)
			{
			case DEVICECATEGORY_App_Server_CONST:
				m_dwPK_Device_AppServer = m_dwPK_Device_LocalAppServer = pDeviceData_Base->m_dwPK_Device;
				break;
			case DEVICECATEGORY_Disc_Drives_CONST:
				m_dwPK_Device_DiscDrive = pDeviceData_Base->m_dwPK_Device;
				break;
	//todo		case DEVICECATEGORY_WebBrowser_CONST:
	//			m_dwPK_Device_WebBrowser = pDeviceData_Base->m_dwPK_Device;
	//			break;
	//todo		case DEVICECATEGORY_General_Info_CONST:
	//			m_dwPK_Device_MediaBurner = pDeviceData_Base->m_dwPK_Device;
	//			break;
	//todo		case DEVICECATEGORY_General_Info_CONST:
	//			m_dwPK_Device_VideoConf = pDeviceData_Base->m_dwPK_Device;
	//			break;
	//todo		case DEVICECATEGORY_General_Info_CONST:
	//			m_dwPK_Device_CamcorderCapt = pDeviceData_Base->m_dwPK_Device;
	//			break;
			case DEVICECATEGORY_Standard_Orbiter_CONST:
				m_dwPK_Device_Orbiter = pDeviceData_Base->m_dwPK_Device;
				break;
			}
		}
	}

    return true;
}



int Virtual_Device_Translator::TranslateVirtualDevice(int PK_DeviceTemplate)
{
	// This is going to a virtual device
	switch(  PK_DeviceTemplate )
	{
	case DEVICETEMPLATE_VirtDev_AppServer_CONST:
		return m_dwPK_Device_AppServer;

	case DEVICETEMPLATE_VirtDev_Orbiter_Onscreen_CONST:
		return m_dwPK_Device_Orbiter;

	case DEVICETEMPLATE_VirtDev_Media_Director_CONST:
		return m_dwPK_Device_MediaDirector;

	case DEVICETEMPLATE_VirtDev_Local_Media_Director_CONST:
		return m_dwPK_Device_MediaDirector;

	case DEVICETEMPLATE_This_Orbiter_CONST:
		return m_pDeviceData_Base->m_dwPK_Device;

	case DEVICETEMPLATE_VirtDev_Security_Plugin_CONST:
		return m_dwPK_Device_SecurityPlugIn;

	case DEVICETEMPLATE_VirtDev_Telecom_Plugin_CONST:
		return m_dwPK_Device_TelecomPlugIn;

	case DEVICETEMPLATE_VirtDev_Media_Plugin_CONST:
		return m_dwPK_Device_MediaPlugIn;

	case DEVICETEMPLATE_VirtDev_Climate_PlugIn_CONST:
		return m_dwPK_Device_ClimatePlugIn;

	case DEVICETEMPLATE_VirtDev_Lighting_PlugIn_CONST:
		return m_dwPK_Device_LightingPlugIn;

	case DEVICETEMPLATE_VirtDev_Infrared_Plugin_CONST:
		return m_dwPK_Device_InfraredPlugIn;

	case DEVICETEMPLATE_VirtDev_General_Info_Plugin_CONST:
		return m_dwPK_Device_GeneralInfoPlugIn;

	case DEVICETEMPLATE_VirtDev_Event_Plugin_CONST:
		return m_dwPK_Device_EventPlugIn;

	case DEVICETEMPLATE_VirtDev_Datagrid_Plugin_CONST:
		return m_dwPK_Device_DatagridPlugIn;

	case DEVICETEMPLATE_VirtDev_Orbiter_Plugin_CONST:
		return m_dwPK_Device_OrbiterPlugIn;

	case DEVICETEMPLATE_VirtDev_Local_AppServer_CONST:
		return m_dwPK_Device_LocalAppServer;

	}
	return -1;
}
