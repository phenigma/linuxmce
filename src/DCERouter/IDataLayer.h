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

	virtual bool GetDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router) = 0;
	virtual bool ReadStaticConfiguration(std::map<int, DeviceData_Router *>& mapDeviceData_Router) = 0;
	virtual int GetLargestDeviceNumber() = 0;
	virtual Scene_Data* GetScene(int nSceneID) = 0;

	virtual DeviceTemplate_Data* GetDeviceTemplate(int nPK_DeviceTemplate) = 0;
};
//----------------------------------------------------------------------------------------------
#endif //__DATA_LAYER_H__
