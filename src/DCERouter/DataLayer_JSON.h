#ifndef __DATA_LAYER_JSON_H__
#define __DATA_LAYER_JSON_H__

#include "IDataLayer.h"
//----------------------------------------------------------------------------------------------
class DataLayer_JSON : public IDataLayer
{
	int m_dwPK_Device_Largest;

public:
	DataLayer_JSON(void);
	~DataLayer_JSON(void);
	bool GetDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router);
	int GetLargestDeviceNumber();

private:

	void ParseDevicesList(std::map<int, DeviceData_Router *>& mapDeviceData_Router, struct json_object *json_obj);
	void ParseDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router, struct json_object *json_obj);
	void ParseDeviceDataList(std::map<int, string>& mapDeviceData, struct json_object *json_obj);
	void ParseDeviceParameters(std::map<string, string>& mapDeviceParams, struct json_object *json_obj);
	void AssignParametersToDevice(const std::map<int, DeviceData_Router *>& mapDeviceData_Router,
		DeviceData_Router *pDevice, const std::map<string, string>& mapDeviceParams);
};
//----------------------------------------------------------------------------------------------
#endif //__DATA_LAYER_JSON_H__

