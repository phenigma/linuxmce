/**
 *
 * @file DatabaseUtils.h
 * @brief header file for the DatabaseUtils namespace
 * @author
 * @todo notcommented
 *
 */



#ifndef DatabaseUtils_h
#define DatabaseUtils_h

#include "MySQLHelper.h"

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace DatabaseUtils
{
	int GetTopMostDevice(MySqlHelper *pMySqlHelper,int PK_Device);
	int GetNumberOfChildDevices(MySqlHelper *pMySqlHelper,int PK_Device);
	int GetDeviceTemplateForDevice(MySqlHelper *pMySqlHelper,int PK_Device);
	string HumanReadablePort(MySqlHelper *pMySqlHelper,int PK_Device,string sPort);
	void GetUnusedPortsOnPC(MySqlHelper *pMySqlHelper,int PK_Device,vector<string> &vectPorts);
	void GetUnusedPortsOnAllPCs(MySqlHelper *pMySqlHelper,vector< pair<int,string> > &vectAllPorts, long nFK_Installation = 0);
	void GetAllDevicesInTree(MySqlHelper *pMySqlHelper,int PK_Device,map<int,int> &mapDeviceTree,bool bCheckParent=true,int PK_Device_ChildExclude=0);
	void SetDeviceData(MySqlHelper *pMySqlHelper,int PK_Device,int PK_DeviceData,string IK_DeviceData);
	string GetDeviceData(MySqlHelper *pMySqlHelper,int PK_Device,int PK_DeviceData);
	string GetDescriptionForDevice(MySqlHelper *pMySqlHelper,int PK_Device);
	bool SetDeviceInRoom(MySqlHelper *pMySqlHelper,int PK_Device,int PK_Room);
	bool SetDeviceControlledVia(MySqlHelper *pMySqlHelper,int PK_Device,int PK_Device_ControlledVia);
	int GetCommMethodForDeviceTemplate(MySqlHelper *pMySqlHelper,int PK_DeviceTemplate);
	int FindControlledViaCandidate(MySqlHelper *pMySqlHelper,int iPK_Device,int iPK_DeviceTemplate,int iPK_Device_RelatedTo,int iPK_Installation);
	int FindControlledViaCandidate(MySqlHelper *pMySqlHelper,int iPK_Device,int iPK_DeviceTemplate,int iPK_Device_RelatedTo,PlutoSqlResult &result_cv);
	int ViolatesDuplicateRules(MySqlHelper *pMySqlHelper,int PK_Device_ControlledVia_temp,int iPK_DeviceTemplate);
};

#endif //#ifndef _DatabaseUtils
