#ifndef __DATA_LAYER_JSON_H__
#define __DATA_LAYER_JSON_H__

#include "IDataLayer.h"
#include "DeviceTemplate_Data.h"
#include "DeviceCategory_Data.h"
#include "Scene_Data.h"
#include <map>
//----------------------------------------------------------------------------------------------
class DataLayer_JSON : public IDataLayer
{
	int m_dwPK_Device_Largest;

	std::map<int, DeviceTemplate_Data> m_mapDeviceTemplate_Data;
	std::map<int, DeviceCategory_Data> m_mapDeviceCategory_Data;
	std::map<int, Scene_Data> m_mapScene_Data;

public:
	DataLayer_JSON(void);
	~DataLayer_JSON(void);
	bool GetDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router);
	Scene_Data* GetScene(int nSceneID);
	bool ReadStaticConfiguration(std::map<int, DeviceData_Router *>& mapDeviceData_Router);
	int GetLargestDeviceNumber();

	DeviceTemplate_Data* GetDeviceTemplate(int nPK_DeviceTemplate);

private:

	void ParseDevicesList(std::map<int, DeviceData_Router *>& mapDeviceData_Router, struct json_object *json_obj);
	void ParseDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router, struct json_object *json_obj);
	void ParseDeviceDataList(std::map<int, string>& mapDeviceData, struct json_object *json_obj);
	void ParseDeviceParameters(std::map<string, string>& mapDeviceParams, struct json_object *json_obj);
	void ParseScenes(struct json_object *json_obj);
	void ParseCommands(map<int, Command_Data>& mapCommands, struct json_object *json_obj);
	void ParseDeviceTemplates(struct json_object *json_obj);
	void ParseDeviceCategories(struct json_object *json_obj);
	void ParseCommandParameters(std::map<int, string>& mapParams, struct json_object *json_obj);

	void AssignParametersToDevice(const std::map<int, DeviceData_Router *>& mapDeviceData_Router,
		DeviceData_Router *pDevice, const std::map<string, string>& mapDeviceParams);
	void UpdateDevicesTree(std::map<int, DeviceData_Router *>& mapDeviceData_Router);

	//utils
	char *GetUncompressedDataFromFile(string sFileName);
};
//----------------------------------------------------------------------------------------------
#endif //__DATA_LAYER_JSON_H__

