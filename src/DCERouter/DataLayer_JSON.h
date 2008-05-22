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

	std::map<int, DeviceData_Router *> m_mapDeviceData_Router;
	std::map<int, DeviceTemplate_Data> m_mapDeviceTemplate_Data;
	std::map<int, DeviceCategory_Data> m_mapDeviceCategory_Data;
	std::map<int, Scene_Data> m_mapScene_Data;

	struct json_object *m_root_json_obj;

public:
	DataLayer_JSON(void);
	~DataLayer_JSON(void);

	//load/save data
	bool Load();
	bool Save();

	//get devices
	std::map<int, DeviceData_Router *>& Devices();

	//queries for scenes and device templates
	Scene_Data* Scene(int nSceneID);
	DeviceTemplate_Data* DeviceTemplate(int nPK_DeviceTemplate);

	//queries for devices
	int LargestDeviceNumber();
	DeviceData_Router *ChildMatchingDeviceData(int nPK_Device_Parent, int nFK_DeviceData, string sValue);
	DeviceData_Router *Device(int nPK_Device);
	void ChildrenDevices(int nPK_Device_Parent, std::vector<DeviceData_Router *>& vectDevice_Children);

	//operations on devices
	void SetDeviceData(int nPK_Device, int nFK_DeviceData, string sValue);
	void SetDeviceData(DeviceData_Router *pDeviceData_Router, int nFK_DeviceData, string sValue);

private:

	bool LoadDynamicConfiguration();
	bool LoadStaticConfiguration();
	void UpdateDevicesTree();

	void ParseDevicesList(struct json_object *json_obj);
	void ParseDevices(struct json_object *json_obj);
	void ParseDeviceDataList(std::map<int, string>& mapDeviceData, struct json_object *json_obj);
	void ParseDeviceParameters(std::map<string, string>& mapDeviceParams, struct json_object *json_obj);
	void ParseScenes(struct json_object *json_obj);
	void ParseCommands(map<int, Command_Data>& mapCommands, struct json_object *json_obj);
	void ParseDeviceTemplates(struct json_object *json_obj);
	void ParseDeviceCategories(struct json_object *json_obj);
	void ParseCommandParameters(std::map<int, string>& mapParams, struct json_object *json_obj);

	void SaveDevicesList();
	void SaveDevices();

	void AssignParametersToDevice(DeviceData_Router *pDevice, const std::map<string, string>& mapDeviceParams);

	//utils
	char *GetUncompressedDataFromFile(string sFileName);
};
//----------------------------------------------------------------------------------------------
#endif //__DATA_LAYER_JSON_H__

