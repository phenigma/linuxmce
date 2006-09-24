#ifndef CreateDevice_h
#define CreateDevice_h

#include "PlutoUtils/MySQLHelper.h"

class CreateDevice : public MySqlHelper
{
	int m_iPK_Installation;
public:
	bool m_bDontInstallPackages,m_bInstallPackagesInBackground;

	CreateDevice(int PK_Installation,string host, string user, string pass, string db_name, int port=3306) 
		: MySqlHelper(host, user, pass, db_name, port) { m_iPK_Installation=PK_Installation; m_bDontCallConfigureScript=false; m_bInstallPackagesInBackground=false; }

	int DoIt(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sIPAddress,string sMacAddress,int PK_Device_ControlledVia=0,string sDeviceData="",int iPK_Device_RelatedTo=0,int iPK_Room=0);
	void CreateChildrenByCategory(int iPK_Device,int iPK_DeviceCategory);
	void CreateChildrenByTemplate(int iPK_Device,int iPK_DeviceTemplate);
	// For CreateAutoCreateChildDevice specify either PK_DeviceTemplate_DeviceTemplate_ControlledVia or PK_DeviceTemplate_DeviceCategory_ControlledVia
	// but not both.  Which one is specified determines which table the relationship was taken from
	void CreateAutoCreateChildDevice(int iPK_Device_Parent,int PK_DeviceTemplate,bool bRerouteMessagesToParent,int PK_DeviceTemplate_DeviceTemplate_ControlledVia,int PK_DeviceTemplate_DeviceCategory_ControlledVia);
	void ConfirmAllRelations();
	void ConfirmRelations(int PK_Device,bool bRecurseChildren=true,bool bOnlyAddDevicesOnCore=false);

	void AssignDeviceData(int PK_Device,int PK_DeviceData,string sValue);
	void GetAllDevicesInTree(int PK_Device,map<int,int> &mapDeviceTree,bool bCheckParent=true,int PK_Device_ChildExclude=0);

	void FixControlledViaIfEmbeddedIsMoreValid(int PK_DeviceTemplate,int &PK_Device_ControlledVia);

	bool m_bDontCallConfigureScript;
};
#endif
