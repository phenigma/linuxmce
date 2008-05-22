#ifndef __DATA_LAYER_H__
#define __DATA_LAYER_H__
//----------------------------------------------------------------------------------------------
#include <map>
#include "DeviceData_Router.h"
#include "Scene_Data.h"
#include "DeviceTemplate_Data.h"
//----------------------------------------------------------------------------------------------
using namespace DCE;
//----------------------------------------------------------------------------------------------
class IDataLayer
{
public:

	IDataLayer(void)
	{
	}

	virtual ~IDataLayer(void)
	{
	}

	//load data
	virtual bool Load() = 0;

	//get devices
	virtual std::map<int, DeviceData_Router *>& Devices() = 0;

	//queries for scenes and device templates
	virtual Scene_Data* Scene(int nSceneID) = 0;
	virtual DeviceTemplate_Data* DeviceTemplate(int nPK_DeviceTemplate) = 0;

	//queries for devices
	virtual int LargestDeviceNumber() = 0;
	virtual int ChildMatchingDeviceData(int nPK_Device, int nFK_DeviceData, string sValue) = 0;
};
//----------------------------------------------------------------------------------------------
#endif //__DATA_LAYER_H__
