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

class MySqlHelper;

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace DatabaseUtils
{
	void GetUnusedPortsOnPC(MySqlHelper *pMySqlHelper,int PK_Device,vector<string> &vectPorts);
	void GetAllDevicesInTree(MySqlHelper *pMySqlHelper,int PK_Device,map<int,int> &mapDeviceTree,bool bCheckParent=true,int PK_Device_ChildExclude=0);
	void SetDeviceData(MySqlHelper *pMySqlHelper,int PK_Device,int PK_DeviceData,string IK_DeviceData);
};

#endif //#ifndef _DatabaseUtils
