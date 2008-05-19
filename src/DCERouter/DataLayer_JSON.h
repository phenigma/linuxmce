#ifndef __DATA_LAYER_JSON_H__
#define __DATA_LAYER_JSON_H__

#include "IDataLayer.h"
#include "DeviceTemplate_Data.h"
#include "DeviceCategory_Data.h"
#include <map>
//----------------------------------------------------------------------------------------------
class DataLayer_JSON : public IDataLayer
{
	int m_dwPK_Device_Largest;

	std::map<int, DeviceTemplate_Data> m_mapDeviceTemplate_Data;
	std::map<int, DeviceCategory_Data> m_mapDeviceCategory_Data;

public:
	DataLayer_JSON(void);
	~DataLayer_JSON(void);
	bool GetDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router);
	bool ReadStaticConfiguration(std::map<int, DeviceData_Router *>& mapDeviceData_Router);
	int GetLargestDeviceNumber();

private:

	void ParseDevicesList(std::map<int, DeviceData_Router *>& mapDeviceData_Router, struct json_object *json_obj);
	void ParseDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router, struct json_object *json_obj);
	void ParseDeviceDataList(std::map<int, string>& mapDeviceData, struct json_object *json_obj);
	void ParseDeviceParameters(std::map<string, string>& mapDeviceParams, struct json_object *json_obj);
	void AssignParametersToDevice(const std::map<int, DeviceData_Router *>& mapDeviceData_Router,
		DeviceData_Router *pDevice, const std::map<string, string>& mapDeviceParams);

	void ParseDeviceTemplates(struct json_object *json_obj);
	void ParseDeviceCategories(struct json_object *json_obj);

	void UpdateDevicesTree(std::map<int, DeviceData_Router *>& mapDeviceData_Router);
};
//----------------------------------------------------------------------------------------------
#endif //__DATA_LAYER_JSON_H__

