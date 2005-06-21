#ifndef CreateDevice_h
#define CreateDevice_h

#include "PlutoUtils/MySQLHelper.h"

class CreateDevice : public MySqlHelper
{
	int m_iPK_Installation;
public:
	CreateDevice(int PK_Installation,string host, string user, string pass, string db_name, int port=3306) 
		: MySqlHelper(host, user, pass, db_name, port) { m_iPK_Installation=PK_Installation; m_bDontCallConfigureScript=false; }

	int DoIt(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sIPAddress,string sMacAddress,int PK_Device_ControlledVia=0);
	void CreateChildrenByCategory(int iPK_Device,int iPK_DeviceCategory);
	void CreateChildrenByTemplate(int iPK_Device,int iPK_DeviceTemplate);
	void ConfirmRelations(int PK_Device,bool bRecurseChildren=true,bool bOnlyAddDevicesOnCore=false);

	bool m_bDontCallConfigureScript;
};
#endif
