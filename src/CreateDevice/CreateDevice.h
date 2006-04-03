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
	void ConfirmRelations(int PK_Device,bool bRecurseChildren=true,bool bOnlyAddDevicesOnCore=false);

	int FindControlledViaCandidate(int iPK_Device,int iPK_DeviceTemplate,int iPK_Device_RelatedTo);
	int FindControlledViaCandidate(int iPK_Device,int iPK_DeviceTemplate,int iPK_Device_RelatedTo,PlutoSqlResult &result_cv);
	void AssignDeviceData(int PK_Device,int PK_DeviceData,string sValue);
	void GetAllDevicesInTree(int PK_Device,map<int,int> &mapDeviceTree,bool bCheckParent=true,int PK_Device_ChildExclude=0);

	bool m_bDontCallConfigureScript;
};
#endif
