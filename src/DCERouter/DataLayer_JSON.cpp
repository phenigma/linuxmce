#include "DataLayer_JSON.h"
//----------------------------------------------------------------------------------------------
extern "C"
{
	#include "json.h" 
	#include "bits.h"
	#include "debug.h"
	#include "linkhash.h"
	#include "arraylist.h"
	#include "json_util.h"
	#include "json_object.h"
	#include "json_object_private.h"
	#include "json_tokener.h" 
}
//----------------------------------------------------------------------------------------------
#define JSON_CONFIG_FILE "pluto.json"
//----------------------------------------------------------------------------------------------
DataLayer_JSON::DataLayer_JSON(void)
{					
}
//----------------------------------------------------------------------------------------------
DataLayer_JSON::~DataLayer_JSON(void)
{
}
//----------------------------------------------------------------------------------------------
bool DataLayer_JSON::GetDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router)
{
	struct json_object *json_obj = NULL;
	
	string sData;
	FileUtils::ReadTextFile(JSON_CONFIG_FILE, sData);
	json_obj = json_tokener_parse(const_cast<char *>(sData.c_str()));

	struct json_object_iter iter;
	json_object_object_foreachC(json_obj, iter) 
	{
		string sValue = iter.key;

		if(sValue == "devices_list")
			ParseDevicesList(mapDeviceData_Router, iter.val);
		else if(sValue == "devices")
			ParseDevices(mapDeviceData_Router, iter.val);
	}

	json_object_put(json_obj); 
	return true;
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDevicesList(std::map<int, DeviceData_Router *>& mapDeviceData_Router, struct json_object *json_obj)
{
	if(json_obj != NULL && json_obj->o_type == json_type_array)
	{
		for(int i = 0; i < json_object_array_length(json_obj); i++) 
		{
			struct json_object *obj_device_id = json_object_array_get_idx(json_obj, i);

			if(obj_device_id->o_type == json_type_int)
			{
				int nDeviceID = json_object_get_int(obj_device_id);
				mapDeviceData_Router[nDeviceID] = NULL;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Device id from array must be an int!");
			}
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Not an array : devices_list!");
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router, struct json_object *json_obj)
{
	struct json_object *obj_devices = json_obj;
	struct json_object_iter iter_devices;
	json_object_object_foreachC(obj_devices, iter_devices) 
	{
		string sDeviceID = iter_devices.key;

		StringUtils::Replace(&sDeviceID, "device_", "");
		int nDeviceID = atoi(sDeviceID.c_str());

		if(mapDeviceData_Router.find(nDeviceID) != mapDeviceData_Router.end())
		{
			DeviceData_Router *pDevice = NULL;
			int PK_DeviceTemplate = 0;
			int PK_Installation = 1; //Hardcoded!
			int PK_Device_ControlledVia = 0;
			int PK_Room = 0;
			string sDescription;
			map<int, string> mapDeviceData;
			map<string, string> mapDeviceParams;

			struct json_object *obj_device = iter_devices.val;
			struct json_object_iter iter_device;
			json_object_object_foreachC(obj_device, iter_device) 
			{
				string sValue = iter_device.key;

				if(sValue == "description" && iter_device.val->o_type == json_type_string)
					sDescription = json_object_get_string(iter_device.val);
				else if(sValue == "device_template_id" && iter_device.val->o_type == json_type_int)
					PK_DeviceTemplate = json_object_get_int(iter_device.val);
				else if(sValue == "room_id" && iter_device.val->o_type == json_type_int)
					PK_Room = json_object_get_int(iter_device.val);
				else if(sValue == "device_controlled_via_id" && iter_device.val->o_type == json_type_int)
					PK_Device_ControlledVia = json_object_get_int(iter_device.val);
				else if(sValue == "params")
					ParseDeviceParameters(mapDeviceParams, iter_device.val);
				else if(sValue == "device_data" && iter_device.val->o_type == json_type_object)
					ParseDeviceDataList(mapDeviceData, iter_device.val);
			}

			pDevice = new DeviceData_Router(nDeviceID, PK_DeviceTemplate, PK_Installation, PK_Device_ControlledVia);
			pDevice->m_sDescription = sDescription;
			pDevice->m_mapParameters = mapDeviceData;
			mapDeviceData_Router[pDevice->m_dwPK_Device] = pDevice;

			AssignParametersToDevice(pDevice, mapDeviceParams);
		}
 		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got a device, but it's not in the device id list: '%s'",
				iter_devices.key);
		}
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDeviceDataList(std::map<int, string>& mapDeviceData, struct json_object *json_obj)
{
	struct json_object_iter iter_devicedata;
	json_object_object_foreachC(json_obj, iter_devicedata)
	{
		string sValue = iter_devicedata.key;
		int PK_DeviceData = atoi(StringUtils::Replace(&sValue, "dd_", "").c_str());

		if(iter_devicedata.val->o_type == json_type_string)
			mapDeviceData[PK_DeviceData] = json_object_get_string(iter_devicedata.val);
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDeviceParameters(std::map<string, string>& mapDeviceParams, struct json_object *json_obj)
{
	struct json_object_iter iter_devicedata;
	json_object_object_foreachC(json_obj, iter_devicedata)
	{
		string sKey = iter_devicedata.key;

		if(iter_devicedata.val->o_type == json_type_string)
			mapDeviceParams[sKey] = json_object_get_string(iter_devicedata.val);
	}	
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::AssignParametersToDevice(DeviceData_Router *pDevice, const std::map<string, string>& mapDeviceParams)
{
	std::map<string, string>::const_iterator it = mapDeviceParams.begin();
	
	for(; it != mapDeviceParams.end(); ++it)
	{
		if(it->first == "IPaddress")
			pDevice->m_sIPAddress = it->second;
		else if(it->first == "MACaddress")
			pDevice->m_sMacAddress = it->second;
		else if(it->first == "IgnoreOnOff")
			pDevice->m_bIgnoreOnOff = it->second == "1";
		//else if(it->first == "NeedConfigure")
		//	pDevice-> = it->second;
		else if(it->first == "State")
			pDevice->m_sState_set(it->second);
		else if(it->first == "Status")
			pDevice->m_sStatus_set(it->second);
		else if(it->first == "Disabled")
			pDevice->m_bDisabled = it->second == "1";
	}
}
//----------------------------------------------------------------------------------------------
