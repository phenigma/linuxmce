#ifndef __DATA_LAYER_H__
#define __DATA_LAYER_H__

#include <map>
#include "DeviceData_Router.h"

using namespace DCE;

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
	virtual int GetLargestDeviceNumber() = 0;
};

#endif //__DATA_LAYER_H__
