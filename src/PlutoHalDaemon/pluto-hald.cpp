#include <dbus/dbus.h>
#include <stdio.h>
#include <hal/libhal.h>
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"

#include "PlutoUtils/StringUtils.h"

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"



map<unsigned int, int> templatesMap;

/*
 
 CreateDevice createDevice(m_pRouter->iPK_Installation_get(),m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()); 
 createDevice.m_bInstallPackagesInBackground = true; 
 *iPK_Device = createDevice.DoIt(iPK_DHCPDevice,iPK_DeviceTemplate,sIP_Address,sMac_address,iPK_Device_ControlledVia,sData,iPK_Device_Related,iPK_Room > 0 ? iPK_Room : 0);

 */
// -DDBUS_API_SUBJECT_TO_CHANGE -I/usr/include/hal -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include

char *hostname = NULL;

using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger;
};


void mainloop_integration (LibHalContext *ctx, DBusConnection * dbus_connection)
{
	dbus_connection_setup_with_g_main (dbus_connection, NULL);
}

void sendMessage(char *params[], int count, string &returnValue)
{
	returnValue = "";
	
	printf("MessageSend %s", hostname);
	for(int i = 0; i < count; i++)
		printf(" %s", params[i]);
	printf("\n");
	Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, hostname);
	if(pEvent != NULL)
	{			
		for(int i=0;true;++i) // Wait up to 30 seconds
		{
			pEvent->m_pClientSocket->SendString("READY");
			string sResponse;
			if( !pEvent->m_pClientSocket->ReceiveString(sResponse,5) )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"Cannot communicate with router");
				throw(string("Cannot communicate with router"));
			}
			if( sResponse=="YES" )
				break;
			else if( sResponse=="NO" )
			{
				if( i>5 )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"Router not ready after 30 seconds.  Aborting....");
					throw(string("Router not ready after 30 seconds.  Aborting...."));
				}
				g_pPlutoLogger->Write(LV_STATUS,"DCERouter still loading.  Waiting 5 seconds");
				Sleep(5000);
			}
			else
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"Router gave unknown response to ready request %s",sResponse.c_str());
				throw(string("Router gave unknown response to ready request"));
			}
		}

		Message *pMsg=new Message(count, params);
		if(pMsg != NULL)
		{
			eExpectedResponse ExpectedResponse = pMsg->m_eExpectedResponse;
			if( ExpectedResponse == ER_ReplyMessage )
			{
				// There are out parameters, we need to get a message back in return
				Message *pResponse = pEvent->SendReceiveMessage( pMsg );
				if( !pResponse || pResponse->m_dwID != 0 )
				{
					if(pResponse)
					{
						delete pResponse;
						pResponse = NULL;
					}
					cout << "Failed to send message" << endl;
					g_pPlutoLogger->Write(LV_DEBUG, "Failed to send message" );
				}
				else
				{
					cout << "0" << endl;
					for( map<long, string>::iterator it=pResponse->m_mapParameters.begin();it!=pResponse->m_mapParameters.end();++it)
					{
						cout << (*it).first << ":" << (*it).second << endl;
						returnValue = (*it).second;
					}
				}
				delete pResponse;
				pResponse = NULL;
			}
			else
			{
				g_pPlutoLogger->Write(LV_DEBUG, "message should have out parameters (PK_Device (int))");
				cout << "message should have out parameters (PK_Device (int))" << ExpectedResponse << endl;
			}
		}
		else
		{
			throw(string("pMsg == NULL"));
		}
	}
	else
		throw(string("pEvent == NULL"));
}


void myDeviceAdded(LibHalContext * ctx, const char * udi)
{

	static char last_udi[2048];

	gchar *bus = hal_device_get_property_string (ctx, udi, "info.bus");
	if(strcmp(bus, "usb_device") == 0 && strlen(bus) == strlen("usb_device"))
	{
		strcpy(last_udi, udi); 
		//hal_device_print (ctx, udi);
		int usb_device_product_id = hal_device_get_property_int(ctx, udi, "usb_device.product_id");
		int usb_device_vendor_id = hal_device_get_property_int(ctx, udi, "usb_device.vendor_id");

		map<unsigned int, int>::iterator it;
		it = templatesMap.find(((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xff) );
		if(it != templatesMap.end())
		{
			gchar *product = hal_device_get_property_string (ctx, udi, "info.product");
			gchar *vendor = hal_device_get_property_string (ctx, udi, "info.vendor");
			gchar *serial = hal_device_get_property_string (ctx, udi, "usb_device.serial");
			gchar *info_udi = hal_device_get_property_string (ctx, udi, "info.udi");
			gchar *sysfs_path = hal_device_get_property_string (ctx, udi, "usb_device.linux.sysfs_path");

			// TODO: check for null strings

			char buffer[4096];


			printf("%s | %s | %s | %s | %s | %s\n", bus, product, vendor, serial, info_udi, sysfs_path);
			if(hostname == NULL)
			{
				hostname = (char *)malloc(10);
				if(hostname != NULL)
					strcpy(hostname, "localhost");
			}
			/*./MessageSend 10.0.0.87 -targetType template -o 0 27 1 718 44 1754 156 1 57 -1
				0:OK
				2:30
			 * */
/*			snprintf( buffer, sizeof(buffer),
				"MessageSend %s -targetType template -o 0 27 1 718 44 %d 156 %d 57 -1 109 154|%s",
				hostname,
				PLUTO_ZWAVE_TEMPLATE_ID, 
				PLUTO_CORE_DEVICE_ID,
				info_udi);
				printf("%s\n", buffer);*/
//				system(buffer);
			char *params[15];
			for(int i = 0; i < 15; i++)
				params[15] = (char *)malloc(1024);
			try
			{
				
				bool isNewDevice = true;
				string response;
				//find out if it is a new device by calling the command 
				//corresponding to the void General_Info_Plugin::Get_iPK_DeviceFromUID(string UID, string deviceType, string &returnValue)
				//function from General_Info_Plugin
				{
					strncmp(params[0], "-targetType", 1024);
					strncmp(params[1], "template", 1024);
					strncmp(params[2], "-o", 1024);
					strncmp(params[3], "0", 1024);
					strncmp(params[4], (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(), 1024); // to: ;
					strncmp(params[5], "1", 1024);
					strncmp(params[6], (char*)StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ).c_str(), 1024); // Get_iPK_DeviceFromUID ;
					strncmp(params[7], (char *)info_udi, 1024);
			
					sendMessage(params, 8, response);
					StringUtils::TrimSpaces(response);
					isNewDevice = response.empty();
				}
				if(isNewDevice)
				{
					
					
					char buffer2[20];
					snprintf(buffer, sizeof(buffer), "154|%s", info_udi);
					snprintf(buffer2, sizeof(buffer2), "%s", StringUtils::itos( (*it).second ).c_str());

					strncmp(params[0], "-targetType", 1024);
					strncmp(params[1], "template", 1024);
					strncmp(params[2], "-o", 1024);
					strncmp(params[3], "0", 1024); // from:
					strncmp(params[4], "27", 1024);// to: DEVICETEMPLATE_General_Info_Plugin_CONST
					strncmp(params[5], "1", 1024); //command
					strncmp(params[6], "718", 1024);
					strncmp(params[7], "44", 1024);
					strncmp(params[8], buffer2, 1024);
					strncmp(params[9], "156", 1024);
					strncmp(params[10], "PLUTO_CORE_DEVICE_ID_STR", 1024);
					strncmp(params[11], "57", 1024);
					strncmp(params[12], "-1", 1024);
					strncmp(params[13], "109", 1024); // extra param for setting the data devices
					strncmp(params[14], buffer, 1024);
				
					sendMessage(params, 15, response);
				}
				else
				{
					//enable the device with ID = response
				}
			}
			catch(string ex)
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "exception thrown: %s", ex.c_str());
			}
			
			for(int i = 0; i < 15; i++)
			{
				free(params[i]);
				params[i] = NULL;
			}
			g_free (product);
			product = NULL;
			g_free (vendor);
			vendor = NULL;
			g_free (serial);
			serial = NULL;
			g_free (info_udi);
			info_udi = NULL;
			g_free (sysfs_path);
			sysfs_path = NULL;
		}
	}

		
//	if(/*strncmp(last_udi, udi, strlen(last_udi)) == 0 && */strncmp(&udi[strlen(udi) - 10], "usb-serial", 10) == 0)
//	{
//	}
	g_free (bus);
}


void myDeviceNewCapability(LibHalContext * ctx, const char * udi, const char *capability)
{
	char *params[10];
	for(int i= 0; i < 10; i++)
	{
		params[i] = (char *)malloc(1024);
	}
		
	gchar *serial_port = hal_device_get_property_string (ctx, udi, "serial.device");
	if(serial_port != NULL)
	{
		gchar *parent = hal_device_get_property_string (ctx, hal_device_get_property_string(ctx, udi, "info.parent"), "info.parent");
		int usb_device_product_id = hal_device_get_property_int(ctx, parent, "usb_device.product_id");
		int usb_device_vendor_id = hal_device_get_property_int(ctx, parent, "usb_device.vendor_id");
		printf("udi = %s parent = %s capability = %s serial port = %s\n", udi, parent, capability, serial_port);
		map<unsigned int, int>::iterator it;
		it = templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xff) );
		if(it != templatesMap.end())
		{
			//got a device with a template in the database
			
			
			char buffer[20];
			strncmp(buffer, StringUtils::itos((*it).second).c_str(), 20);
			strncmp(params[0]  ,	"0", 1024); // from:
			strncmp(params[1]  ,	"4", 1024);// to: DEVICETEMPLATE_General_Info_Plugin_CONST
			strncmp(params[2]  ,	"1", 1024); //command
			strncmp(params[3]  ,	"246", 1024); //set device data
			strncmp(params[4]  ,	"1", 1024); //PK_Device
			strncmp(params[5]  ,	"xxx", 1024); //get the device ID
			strncmp(params[6]  ,	"52", 1024); //PK_DeviceData
			strncmp(params[7]  ,	"get the device data id", 1024);
			strncmp(params[8]  ,	"5", 1024); //value to assign
			strncmp(params[9]  ,	serial_port, 1024);


			string response;
			sendMessage(params, 10, response);
		}
		g_free (parent);
		parent = NULL;
	}
	g_free (serial_port);
	serial_port = NULL;

	for(int i= 0; i < 10; i++)
	{
		free(params[i]);
		params[i] = NULL;
	}
}



void myDeviceRemoved(LibHalContext * ctx, const char * udi)
{
	printf("removed device %s\n", udi);
}


/***/
void initialize(LibHalContext * ctx)
{
	int num_devices = 0;
	char **devices = hal_get_all_devices (ctx, &num_devices);
	gchar *bus = NULL;

	char *params[15];
	for(int i = 0; i < 15; i++)
		params[i] = (char *)malloc(1024);
	
//get all template IDs and producer_id vendor_id
	for(int i = num_devices - 1; i >= 0 ; i--)
	{
		char *udi = devices[i];
		bus = hal_device_get_property_string (ctx, udi, "info.bus");
		if( 0 == strcmp(bus, "usb_device") )
		{
			int usb_device_product_id = hal_device_get_property_int(ctx, udi, "usb_device.product_id");
			int usb_device_vendor_id = hal_device_get_property_int(ctx, udi, "usb_device.vendor_id");
			
			map<unsigned int, int>::iterator it =
				templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xff) );
			if( it != templatesMap.end() )
			{
//					gchar *product = hal_device_get_property_string (ctx, udi, "info.product");
//					gchar *vendor = hal_device_get_property_string (ctx, udi, "info.vendor");
//					gchar *serial = hal_device_get_property_string (ctx, udi, "usb_device.serial");
					gchar *info_udi = hal_device_get_property_string (ctx, udi, "info.udi");
//					gchar *sysfs_path = hal_device_get_property_string (ctx, udi, "usb_device.linux.sysfs_path");
					
				// check if there is a device with this UID

				strncpy(params[0], "-targetType", 1024);
				strncpy(params[1], "template", 1024);
				strncpy(params[2], "-o", 1024);
				strncpy(params[3], "0", 1024); // from:
				strncpy(params[4], (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(), 1024); // to:
				strncpy(params[5], "1", 1024); // command
				strncpy(params[6], (char*)StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ).c_str(), 1024); // command ID
				strncpy(params[7], (char*)StringUtils::itos( COMMANDPARAMETER_UID_CONST ).c_str(), 1024); // param ID
				strncpy(params[8], info_udi, 1024); // UID
				
				string response;
				sendMessage(params, 9, response);
				
				if( !response.empty() )
				{
					// enable the device

					strncpy(params[0], "-targetType", 1024);
					strncpy(params[1], "template", 1024);
					strncpy(params[2], "-o", 1024);
					strncpy(params[3], "0", 1024); // from:
					strncpy(params[4], (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(), 1024); // to:
					strncpy(params[5], "1", 1024); // command
					strncpy(params[6], (char*)StringUtils::itos( COMMAND_Set_Enable_Status_CONST ).c_str(), 1024); // command ID
					strncpy(params[7], (char*)StringUtils::itos( COMMANDPARAMETER_PK_Device_CONST ).c_str(), 1024); // param ID
					strncpy(params[8], (char*)response.c_str(), 1024); // device ID
					strncpy(params[9], (char*)StringUtils::itos( COMMANDPARAMETER_Enable_CONST ).c_str(), 1024); // param ID
					strncpy(params[10], "1", 1024); // true
					
					string responseEnable;
					sendMessage(params, 11, responseEnable);
				}
				else
				{
					// create a new device with this UID
					char buffer[2048];
					snprintf(buffer, sizeof(buffer), "%d|%s", DEVICEDATA_UID_CONST, info_udi);
					
					char *paramsCreate[15];
					
					strncpy(params[0]  , "-targetType", 1024);
					strncpy(params[1]  , "template", 1024);
					strncpy(params[2]  , "-o", 1024);
					strncpy(params[3]  , "0", 1024); // from:
					strncpy(params[4]  , (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(), 1024); // to:
					strncpy(params[5]  , "1", 1024); // command
					strncpy(params[6]  , (char*)StringUtils::itos( COMMAND_Create_Device_CONST ).c_str(), 1024); // command ID
					strncpy(params[7]  , (char*)StringUtils::itos( COMMANDPARAMETER_PK_DeviceTemplate_CONST ).c_str(), 1024); // 
					strncpy(params[8]  , (char*)StringUtils::itos( (*it).second ).c_str(), 1024); // template ID
					strncpy(params[9]  , (char*)StringUtils::itos( COMMANDPARAMETER_PK_Device_ControlledVia_CONST ).c_str(), 1024);
					strncpy(params[10] , "PLUTO_CORE_DEVICE_ID_STR", 1024);
					strncpy(params[11] , (char*)StringUtils::itos( COMMANDPARAMETER_PK_Room_CONST ).c_str(), 1024);
					strncpy(params[12] , "-1", 1024); // ask user
					strncpy(params[13] , "109", 1024); // extra param for setting the data devices
					strncpy(params[14] , buffer, 1024);
				
					string responseCreate;
					sendMessage(paramsCreate, 15, responseCreate);
				}
					
//					g_free (product);
//					product = NULL;
//					g_free (vendor);
//					vendor = NULL;
//					g_free (serial);
//					serial = NULL;
					g_free (info_udi);
					info_udi = NULL;
//					g_free(sysfs_path);
//					sysfs_path = NULL;
			}
		}
		else if( 0 == strcmp(bus, "usb-serial") )
		{
			gchar *parent = hal_device_get_property_string (ctx, hal_device_get_property_string(ctx, udi, "info.parent"), "info.parent");
			gchar *info_udi = hal_device_get_property_string (ctx, parent, "info.udi");
			int usb_device_product_id = hal_device_get_property_int(ctx, parent, "usb_device.product_id");
			int usb_device_vendor_id = hal_device_get_property_int(ctx, parent, "usb_device.vendor_id");
			
			map<unsigned int, int>::iterator it =
				templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xff) );
			if( it != templatesMap.end() )
			{
				gchar *serial_port = hal_device_get_property_string (ctx, udi, "serial.device");
				if(serial_port != NULL)
				{
					printf("udi = %s serial port = %s\n", udi, serial_port);
					
					// check if there is a device with this UID
				
					strncpy(params[0]  , "-targetType", 1024);
					strncpy(params[1]  , "template", 1024);
					strncpy(params[2]  , "-o", 1024);
					strncpy(params[3]  , "0", 1024); // from:
					strncpy(params[4]  , (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(), 1024); // to:
					strncpy(params[5]  , "1", 1024); // command
					strncpy(params[6]  , (char*)StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ).c_str(), 1024); // command ID
					strncpy(params[7]  , (char*)StringUtils::itos( COMMANDPARAMETER_UID_CONST ).c_str(), 1024); // param ID
					strncpy(params[8]  , info_udi, 1024); // UID
					
					string response;
					sendMessage(params, 9, response);
					
					if( !response.empty() )
					{
						// set the serial port for the device
						char *paramsSerial[13];
						
						strncpy(params[0]  , "-targetType", 1024);
						strncpy(params[1]  , "template", 1024);
						strncpy(params[2]  , "-o", 1024);
						strncpy(params[3]  , "0", 1024); // from:
						strncpy(params[4]  , (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(), 1024); // to:
						strncpy(params[5]  , "1", 1024); //command
						strncpy(params[6]  , (char*)StringUtils::itos( COMMAND_Set_Device_Data_CONST ).c_str(), 1024); //set device data
						strncpy(params[7]  , (char*)StringUtils::itos( COMMANDPARAMETER_PK_Device_CONST ).c_str(), 1024); //PK_Device
						strncpy(params[8]  , (char*)response.c_str(), 1024); //get the device ID
						strncpy(params[9]  , (char*)StringUtils::itos( COMMANDPARAMETER_PK_DeviceData_CONST ).c_str(), 1024); //PK_DeviceData
						strncpy(params[10] , (char*)StringUtils::itos( DEVICEDATA_COM_Port_on_PC_CONST ).c_str(), 1024);
						strncpy(params[11] , (char*)StringUtils::itos( COMMANDPARAMETER_Value_To_Assign_CONST ).c_str(), 1024); //value to assign
						strncpy(params[12] , serial_port, 1024);
						
						string responseSerial;
						sendMessage(paramsSerial, 13, responseSerial);
					}
				}
				
				g_free (serial_port);
				serial_port = NULL;
			}
			
			g_free (parent);
			parent = NULL;
			g_free (info_udi);
			info_udi = NULL;
		}

		g_free(bus);
		bus = NULL;
	}
	for(int i = 0; i < 15; i++)
	{
		free(params[i]);
		params[i] = NULL;
	}
}

int main(int argc, char* argv[])
{
	LibHalFunctions funcs;
	LibHalContext * ctx;
	GMainLoop *loop = NULL;
	
	g_pPlutoLogger = new FileLogger("hal.log");
	
	if(argc >= 2)
	{
		hostname = (char *)malloc(strlen(argv[1]) + 1);
		if(hostname != NULL)
		{
			strcpy(hostname, argv[1]);
		}
	}
	else
	{
		hostname = (char *)malloc(strlen("localhost") + 1);
		if(hostname != NULL)
		{
			strcpy(hostname, "localhost");
		}
	}



	//get the list of the templates and their corresponding product_id / vendor_id
	string response;
	char *params[7];
	
	for(int w = 0; w < 7; w++)
	{
		params[w] = (char *)malloc(1024);
	}
	strncpy(params[0], "-targetType", 1024);
	strncpy(params[1], "template", 1024);
	strncpy(params[2], "-o", 1024);
	strncpy(params[3], "0", 1024);
	strncpy(params[4], StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(), 1024);
	strncpy(params[5], "1", 1024);
	strncpy(params[6], StringUtils::itos( COMMAND_Get_All_HAL_Model_ID_CONST ).c_str(), 1024);

	try
	{
		sendMessage((char **)params, 7, response);
		printf("response %s\n", response.c_str());
	}
	catch(string ex)
	{
		printf("exception %s\n", ex.c_str());
		//return 1;
	}

	for(int w = 0; w < 7; w++)
	{
		free( params[w] );
		params[w] = NULL;
	}

	vector<string> strings;
	StringUtils::Tokenize(response, string("\n"), strings);
	
	unsigned int vendor_product = 0;
	int templateID = -1;
	for(vector<string>::const_iterator it = strings.begin(); it != strings.end(); ++it)
	{
		string row = (*it);
		StringUtils::TrimSpaces(row);
		printf("%s", row.c_str());
		
		//should be "template_id vendorproduct:vendorproduct:vendorproduct\n"
		size_t space = row.find(' ');
		if( space != string::npos )
		{
			// the first word is the Template ID
			sscanf(row.substr(0, space).c_str(), "%d", &templateID);
			
			vector<string> products;
			row = row.substr(space+1);
			StringUtils::Tokenize(row, string(":"), products);
			
			for(vector<string>::const_iterator itProd = products.begin(); itProd != products.end(); ++itProd)
			{
				sscanf((*itProd).c_str(), "%x", &vendor_product);
				
				templatesMap[vendor_product] = templateID;
			}
		}
	}
	loop = g_main_loop_new (NULL, FALSE);
	
	funcs.main_loop_integration = mainloop_integration;
	funcs.device_added = myDeviceAdded;
	funcs.device_removed = myDeviceRemoved;
	funcs.device_new_capability = myDeviceNewCapability;
	funcs.device_lost_capability = NULL;
	funcs.device_property_modified = NULL;
	funcs.device_condition = NULL;
	
	ctx = hal_initialize(&funcs, 0);
	if (!ctx)
	{
		printf("NULL!\n");
		return 1;
	}
	initialize(ctx);
	
	hal_device_property_watch_all(ctx);

	g_main_loop_run(loop);
	
	hal_shutdown(ctx);
	free(hostname);
	return 0;
}
