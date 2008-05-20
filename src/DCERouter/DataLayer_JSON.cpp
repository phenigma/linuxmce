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

#include "PlutoUtils/minilzo.h"
#include "PlutoUtils/PlutoDefs.h"
//----------------------------------------------------------------------------------------------
#define HEAP_ALLOC(var,size) \
	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]
static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);
//----------------------------------------------------------------------------------------------
#define JSON_DYNAMIC_CONFIG_FILE "pluto.json.lzo"
#define JSON_STATIC_CONFIG_FILE "pluto_main.json.lzo"
//----------------------------------------------------------------------------------------------
DataLayer_JSON::DataLayer_JSON(void)
{
	m_dwPK_Device_Largest = 0;
}
//----------------------------------------------------------------------------------------------
DataLayer_JSON::~DataLayer_JSON(void)
{
}
//----------------------------------------------------------------------------------------------
bool DataLayer_JSON::GetDevices(std::map<int, DeviceData_Router *>& mapDeviceData_Router)
{
	struct json_object *json_obj = NULL;
	
	char *pData = GetUncompressedDataFromFile(JSON_DYNAMIC_CONFIG_FILE);

	if(NULL != pData)
	{
		json_obj = json_tokener_parse(pData);

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

		PLUTO_SAFE_DELETE_ARRAY(pData);

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "DataLayer: Got %d devices", mapDeviceData_Router.size());

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------------------
int DataLayer_JSON::GetLargestDeviceNumber()
{
	return m_dwPK_Device_Largest;
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
				else if(sValue == "device_template_id" && iter_device.val->o_type == json_type_string)
					PK_DeviceTemplate = atoi(json_object_get_string(iter_device.val));
				else if(sValue == "room_id" && iter_device.val->o_type == json_type_string)
					PK_Room = atoi(json_object_get_string(iter_device.val));
				else if(sValue == "device_controlled_via_id" && iter_device.val->o_type == json_type_string)
					PK_Device_ControlledVia = atoi(json_object_get_string(iter_device.val));
				else if(sValue == "params")
					ParseDeviceParameters(mapDeviceParams, iter_device.val);
				else if(sValue == "device_data" && iter_device.val->o_type == json_type_object)
					ParseDeviceDataList(mapDeviceData, iter_device.val);
			}

			pDevice = new DeviceData_Router(nDeviceID, PK_DeviceTemplate, PK_Installation, PK_Device_ControlledVia);
			pDevice->m_sDescription = sDescription;
			pDevice->m_mapParameters = mapDeviceData;
			mapDeviceData_Router[pDevice->m_dwPK_Device] = pDevice;

			if(m_dwPK_Device_Largest < nDeviceID)
				m_dwPK_Device_Largest = nDeviceID;

			AssignParametersToDevice(mapDeviceData_Router, pDevice, mapDeviceParams);
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
void DataLayer_JSON::AssignParametersToDevice(const std::map<int, DeviceData_Router *>& mapDeviceData_Router, 
	DeviceData_Router *pDevice, const std::map<string, string>& mapDeviceParams)
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
		else if(it->first == "FK_Device_RouteTo")
		{
			int FK_Device_RouterTo = atoi(it->second.c_str());

			std::map<int, DeviceData_Router *>::const_iterator it_device = mapDeviceData_Router.find(FK_Device_RouterTo);
			if(it_device != mapDeviceData_Router.end())
			{
				if(NULL != it_device->second)
				{
					pDevice->m_pDevice_RouteTo = it_device->second;
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Message to device %d routed to device %d",
						pDevice->m_dwPK_Device, FK_Device_RouterTo);
				}
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot find device %d to route messages from %d",
					FK_Device_RouterTo, pDevice->m_dwPK_Device);
			}
		}
	}
}
//----------------------------------------------------------------------------------------------
bool DataLayer_JSON::ReadStaticConfiguration(std::map<int, DeviceData_Router *>& mapDeviceData_Router)
{
	struct json_object *json_obj = NULL;

	char *pData = GetUncompressedDataFromFile(JSON_STATIC_CONFIG_FILE);

	if(NULL != pData)
	{
		json_obj = json_tokener_parse(pData);

		struct json_object_iter iter;
		json_object_object_foreachC(json_obj, iter) 
		{
			string sValue = iter.key;

			if(sValue == "DeviceTemplate")
				ParseDeviceTemplates(iter.val);
			else if(sValue == "DeviceCategory")
				ParseDeviceCategories(iter.val);
		}

		json_object_put(json_obj); 

		PLUTO_SAFE_DELETE_ARRAY(pData);

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "DataLayer: Got %d device templates", m_mapDeviceTemplate_Data.size());
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "DataLayer: Got %d device categories", m_mapDeviceCategory_Data.size());

		UpdateDevicesTree(mapDeviceData_Router);
		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDeviceTemplates(struct json_object *json_obj)
{
	struct json_object *obj_devices = json_obj;
	struct json_object_iter iter_devices;
	json_object_object_foreachC(obj_devices, iter_devices) 
	{
		string sDeviceTemplateID = iter_devices.key;

		StringUtils::Replace(&sDeviceTemplateID, "PK_DeviceTemplate_", "");
		int nDeviceTemplateID = atoi(sDeviceTemplateID.c_str());

		DeviceTemplate_Data aDeviceTemplate_Data(nDeviceTemplateID);

		struct json_object_iter iter_deviceparams;
		json_object_object_foreachC(iter_devices.val, iter_deviceparams)
		{
			string sKey = iter_deviceparams.key;

			if(iter_deviceparams.val->o_type == json_type_string)
			{
				string sValue = json_object_get_string(iter_deviceparams.val);

				if(sKey == "Description")
					aDeviceTemplate_Data.Description(sValue);
				if(sKey == "FK_DeviceCategory")
					aDeviceTemplate_Data.DeviceCategory(atoi(sValue.c_str()));
				if(sKey == "ImplementsDCE")
					aDeviceTemplate_Data.ImplementsDCE(sValue == "1");
				if(sKey == "IsEmbedded")
					aDeviceTemplate_Data.IsEmbedded(sValue == "1");
				if(sKey == "CommandLine")
					aDeviceTemplate_Data.CommandLine(sValue);
			}
		}	

		m_mapDeviceTemplate_Data[nDeviceTemplateID] = aDeviceTemplate_Data;
	}
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::ParseDeviceCategories(struct json_object *json_obj)
{
	struct json_object *obj_devices = json_obj;
	struct json_object_iter iter_categories;
	json_object_object_foreachC(obj_devices, iter_categories) 
	{
		string sDeviceCategoryID = iter_categories.key;

		StringUtils::Replace(&sDeviceCategoryID, "PK_DeviceCategory_", "");
		int nDeviceCategoryID = atoi(sDeviceCategoryID.c_str());

		DeviceCategory_Data aDeviceCategory_Data(nDeviceCategoryID);

		struct json_object_iter iter_deviceparams;
		json_object_object_foreachC(iter_categories.val, iter_deviceparams)
		{
			string sKey = iter_deviceparams.key;

			if(iter_deviceparams.val->o_type == json_type_string)
			{
				string sValue = json_object_get_string(iter_deviceparams.val);

				if(sKey == "Description")
					aDeviceCategory_Data.Description(sValue);
				if(sKey == "FK_DeviceCategory_Parent")
					aDeviceCategory_Data.DeviceCategoryParent(atoi(sValue.c_str()));
			}
		}	

		m_mapDeviceCategory_Data[nDeviceCategoryID] = aDeviceCategory_Data;
	}	
}
//----------------------------------------------------------------------------------------------
void DataLayer_JSON::UpdateDevicesTree(std::map<int, DeviceData_Router *>& mapDeviceData_Router)
{
	for(map<int, DeviceData_Router *>::iterator it = mapDeviceData_Router.begin(); it != mapDeviceData_Router.end(); ++it)
	{
		DeviceData_Router *pDevice = it->second;

		std::map<int, DeviceTemplate_Data>::iterator it_devtemplate = m_mapDeviceTemplate_Data.find(pDevice->m_dwPK_DeviceTemplate);
		if(it_devtemplate != m_mapDeviceTemplate_Data.end())
		{
			pDevice->m_bImplementsDCE = it_devtemplate->second.ImplementsDCE();
			pDevice->m_bIsEmbedded = it_devtemplate->second.IsEmbedded();
			pDevice->m_dwPK_DeviceCategory = it_devtemplate->second.DeviceCategory();
			pDevice->m_sCommandLine = it_devtemplate->second.CommandLine();
			pDevice->m_sDescription = it_devtemplate->second.Description();
		}
	}
}
//----------------------------------------------------------------------------------------------
char *DataLayer_JSON::GetUncompressedDataFromFile(string sFileName)
{
	if(!FileUtils::FileExists(sFileName))
	{
		string sPlainFileName = sFileName;
		StringUtils::Replace(&sPlainFileName, ".lzo", "");

		if(FileUtils::FileExists(sPlainFileName))
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "FILE NOT FOUND! Generating lzo file %s from plain json file", sFileName.c_str());

			string sData;
			FileUtils::ReadTextFile(sPlainFileName, sData);

			//compress pixels data
			char *pCompressedData = new char[sData.size() * 3];
			int iCompressedDataSize = 0;
			long iSize = 0;
			lzo1x_1_compress((lzo_byte *)sData.c_str(), (lzo_uint)sData.size(), (lzo_byte *)(pCompressedData + 4), (lzo_uint *)(&iSize), wrkmem);
			*((int *)pCompressedData) = (int)sData.size();
			
			FileUtils::WriteBufferIntoFile(sFileName, pCompressedData, iSize + sizeof(int));
			delete [] pCompressedData;
		}
	}	

	size_t nCompressedDataSize = 0;
	char *pCompressedData = FileUtils::ReadFileIntoBuffer(sFileName, nCompressedDataSize);

	if(NULL == pCompressedData || nCompressedDataSize < 5)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unabled to find/parse json file: %s", sFileName.c_str());
		return NULL;
	}

	int nUncompressDataSize = *(reinterpret_cast<int *>(pCompressedData));
	char *pUncompressedData = new char[nUncompressDataSize + 1];
	memset(pUncompressedData, 0, nUncompressDataSize + 1); 

	long size = 0;
	lzo1x_decompress((lzo_byte *)(pCompressedData + sizeof(int)), (lzo_uint)(nCompressedDataSize - sizeof(int)), 
		(lzo_byte *)pUncompressedData, (lzo_uint *)(&size), wrkmem);

	PLUTO_SAFE_DELETE_ARRAY(pCompressedData);

	return pUncompressedData;
}
//----------------------------------------------------------------------------------------------
