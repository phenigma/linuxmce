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
			try
			{
				
				bool isNewDevice = true;
				string response;
				//find out if it is a new device by calling the command 
				//corresponding to the void General_Info_Plugin::Get_iPK_DeviceFromUID(string UID, string deviceType, string &returnValue)
				//function from General_Info_Plugin
				{
					char *params[5];
					
					params[0]  =	"-targetType";
					params[1]  =	"template";
					params[2]  =	"-o";
					params[3]  =	"0";
					params[4]  =	(char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(); // to: ;
					params[5]  =	"1";
					params[6]  =	(char*)StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ).c_str(); // Get_iPK_DeviceFromUID ;
					strcpy(buffer, info_udi);
					params[7]  =	(char *)buffer;

					
					sendMessage(params, 5, response);
					StringUtils::TrimSpaces(response);
					isNewDevice = response.empty();
				}
				if(isNewDevice)
				{
					char *params[15];
					
					char buffer2[20];
					snprintf(buffer, sizeof(buffer), "154|%s", info_udi);
					snprintf(buffer2, sizeof(buffer2), "%s", StringUtils::itos( (*it).second ).c_str());

					params[0]  =	"-targetType";
					params[1]  =	"template";
					params[2]  =	"-o";
					params[3]  =	"0"; // from:
					params[4]  =	"27";// to: DEVICETEMPLATE_General_Info_Plugin_CONST
					params[5]  =	"1"; //command
					params[6]  =	"718";
					params[7]  =	"44";
					params[8]  =	buffer2;
					params[9]  =	"156";
					params[10] =	"PLUTO_CORE_DEVICE_ID_STR";
					params[11] =	"57";
					params[12] =	"-1";
					params[13] =	"109"; // extra param for setting the data devices
					params[14] =	buffer;
				
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
			char *params[10];
			char buffer[20];
			strncmp(buffer, StringUtils::itos((*it).second).c_str(), sizeof(buffer));
			params[0]  =	"0"; // from:
			params[1]  =	"4";// to: DEVICETEMPLATE_General_Info_Plugin_CONST
			params[2]  =	"1"; //command
			params[3]  =	"246"; //set device data
			params[4]  =	"1"; //PK_Device
			params[5]  =	"xxx"; //get the device ID
			params[6]  =	"52"; //PK_DeviceData
			params[7]  =	"get the device data id";
			params[8]  =	"5"; //value to assign
			params[9]  =	serial_port;


			string response;
			//sendMessage(params, 15, response);
		}
		g_free (parent);
		parent = NULL;
	}
	g_free (serial_port);
	serial_port = NULL;
}



void myDeviceRemoved(LibHalContext * ctx, const char * udi)
{
	printf("removed device %s\n", udi);
}


/***/
void initialize(LibHalContext * ctx)
{
	int num_devices = 0;
	int i = 0;
	char **devices = hal_get_all_devices (ctx, &num_devices);
	gchar *bus = NULL;
	
//get all template IDs and producer_id vendor_id
	for(i = num_devices - 1; i >= 0 ; i--)
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
				char *params[9];
				params[0]  = "-targetType";
				params[1]  = "template";
				params[2]  = "-o";
				params[3]  = "0"; // from:
				params[4]  = (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(); // to:
				params[5]  = "1"; // command
				params[6]  = (char*)StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ).c_str(); // command ID
				params[7]  = (char*)StringUtils::itos( COMMANDPARAMETER_UID_CONST ).c_str(); // param ID
				params[8]  = info_udi; // UID
				
				string response;
				sendMessage(params, 9, response);
				
				if( !response.empty() )
				{
					// enable the device
					char *paramsEnable[9];
					paramsEnable[0]  = "-targetType";
					paramsEnable[1]  = "template";
					paramsEnable[2]  = "-o";
					paramsEnable[3]  = "0"; // from:
					paramsEnable[4]  = (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(); // to:
					paramsEnable[5]  = "1"; // command
					paramsEnable[6]  = (char*)StringUtils::itos( 1 ).c_str(); // command ID
					paramsEnable[7]  = (char*)StringUtils::itos( 1 ).c_str(); // param ID
					paramsEnable[8]  = info_udi; // UID
					
					string responseEnable;
					sendMessage(paramsEnable, 9, responseEnable);
				}
				else
				{
					// create a new device with this UID
					char buffer[2048];
					snprintf(buffer, sizeof(buffer), "%d|%s", DEVICEDATA_UID_CONST, info_udi);
					
					char *paramsCreate[15];
					
					paramsCreate[0]  = "-targetType";
					paramsCreate[1]  = "template";
					paramsCreate[2]  = "-o";
					paramsCreate[3]  = "0"; // from:
					paramsCreate[4]  = (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(); // to:
					paramsCreate[5]  = "1"; // command
					paramsCreate[6]  = (char*)StringUtils::itos( COMMAND_Create_Device_CONST ).c_str(); // command ID
					paramsCreate[7]  = (char*)StringUtils::itos( COMMANDPARAMETER_PK_DeviceTemplate_CONST ).c_str(); // 
					paramsCreate[8]  = (char*)StringUtils::itos( (*it).second ).c_str(); // template ID
					paramsCreate[9]  = (char*)StringUtils::itos( COMMANDPARAMETER_PK_Device_ControlledVia_CONST ).c_str();
					paramsCreate[10] = "PLUTO_CORE_DEVICE_ID_STR";
					paramsCreate[11] = (char*)StringUtils::itos( COMMANDPARAMETER_PK_Room_CONST ).c_str();
					paramsCreate[12] = "-1"; // ask user
					paramsCreate[13] = "109"; // extra param for setting the data devices
					paramsCreate[14] = buffer;
				
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
					fflush(stdout);
				}
				
				//got a device with a template in the database
				char *params[10];
				char buffer[20];
				strncmp(buffer, StringUtils::itos((*it).second).c_str(), sizeof(buffer));
				params[0]  = "-targetType";
				params[1]  = "template";
				params[2]  = "-o";
				params[3]  = "0"; // from:
				params[4]  = (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(); // to:
				params[5]  = "1"; //command
				params[6]  = "246"; //set device data
				params[7]  = "1"; //PK_Device
				params[8]  = "xxx"; //get the device ID
				params[9]  = "52"; //PK_DeviceData
				params[10] = "get the device data id";
				params[11] = "5"; //value to assign
				params[12] = serial_port;
			
				g_free (serial_port);
				serial_port = NULL;
			}
			
			g_free (parent);
			parent = NULL;
		}

		g_free(bus);
		bus = NULL;
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
	params[0]  = "-targetType";
	params[1]  = "template";
	params[2]  = "-o";
	params[3]  = "0";  // from:
	params[4]  = (char*)StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ).c_str(); // to: 
	params[5]  = "1";  // command
	params[6]  = (char*)StringUtils::itos( COMMAND_Get_All_HAL_Model_ID_CONST ).c_str(); // command ID
	

	try
	{	
		sendMessage(params, 7, response);
		printf("response %s\n", response.c_str());
	}
	catch(string ex)
	{
		printf("exception %s\n", ex.c_str());
		//return 1;
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
	return 0;
}
