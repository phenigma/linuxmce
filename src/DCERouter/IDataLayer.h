#ifndef __DATA_LAYER_H__
#define __DATA_LAYER_H__
//----------------------------------------------------------------------------------------------
#include <map>
#include "DeviceData_Router.h"
#include "Scene_Data.h"
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

	virtual bool GetDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router) = 0;
	virtual bool ReadStaticConfiguration(std::map<int, DeviceData_Router *>& mapDeviceData_Router) = 0;
	virtual int GetLargestDeviceNumber() = 0;
	virtual bool GetScene(int nSceneID, Scene_Data& scene) = 0;
};
//----------------------------------------------------------------------------------------------
#endif //__DATA_LAYER_H__
