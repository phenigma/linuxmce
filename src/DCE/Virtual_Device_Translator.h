#ifndef Virtual_Device_Translator_h
#define Virtual_Device_Translator_h

namespace DCE
{
	class DeviceData_Base;
}
using namespace DCE;

class Virtual_Device_Translator
{
	DeviceData_Base *m_pDeviceData_Base;

	// Some virtual devices local to this media director
	int m_dwPK_Device_MediaDirector,m_dwPK_Device_DiscDrive,m_dwPK_Device_AppServer,m_dwPK_Device_MediaBurner,m_dwPK_Device_VideoConf,m_dwPK_Device_CamcorderCapt,m_dwPK_Device_Orbiter,m_dwPK_Device_WebBrowser; 

	// Some global devices
	int m_dwPK_Device_Router,m_dwPK_Device_DatagridPlugIn,m_dwPK_Device_InfraredPlugIn,m_dwPK_Device_GeneralInfoPlugIn,m_dwPK_Device_EventPlugIn,m_dwPK_Device_OrbiterPlugIn,
		m_dwPK_Device_LightingPlugIn, m_dwPK_Device_ClimatePlugIn, m_dwPK_Device_MediaPlugIn, m_dwPK_Device_TelecomPlugIn, m_dwPK_Device_SecurityPlugIn,
		m_dwPK_Device_LocalAppServer;

public:
	Virtual_Device_Translator() {};
	virtual bool GetConfig(DeviceData_Base *pDeviceData_Base);

	int TranslateVirtualDevice(int PK_DeviceTemplate);

};

#endif
