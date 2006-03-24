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
#include "DCE/DCEConfig.h"

#include "PlutoUtils/StringUtils.h"

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#define DEBUG 1

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

//void sendMessage(char *params[], int count, string &returnValue)
//{
//	returnValue = "";
//	
//	printf("MessageSend %s", hostname);
//	for(int i = 0; i < count; i++)
//		printf(" %s", params[i]);
//	printf("\n");
//	Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, hostname);
//	if(pEvent != NULL)
//	{			
//		for(int i=0;true;++i) // Wait up to 30 seconds
//		{
//			pEvent->m_pClientSocket->SendString("READY");
//			string sResponse;
//			if( !pEvent->m_pClientSocket->ReceiveString(sResponse,5) )
//			{
//				g_pPlutoLogger->Write(LV_CRITICAL,"Cannot communicate with router");
//				throw(string("Cannot communicate with router"));
//			}
//			if( sResponse=="YES" )
//				break;
//			else if( sResponse=="NO" )
//			{
//				if( i>5 )
//				{
//					g_pPlutoLogger->Write(LV_CRITICAL,"Router not ready after 30 seconds.  Aborting....");
//					throw(string("Router not ready after 30 seconds.  Aborting...."));
//				}
//				g_pPlutoLogger->Write(LV_STATUS,"DCERouter still loading.  Waiting 5 seconds");
//				Sleep(5000);
//			}
//			else
//			{
//				g_pPlutoLogger->Write(LV_CRITICAL,"Router gave unknown response to ready request %s",sResponse.c_str());
//				throw(string("Router gave unknown response to ready request"));
//			}
//		}
//
//		Message *pMsg=new Message(count, params);
//		if(pMsg != NULL)
//		{
//			eExpectedResponse ExpectedResponse = pMsg->m_eExpectedResponse;
//			if( ExpectedResponse == ER_ReplyMessage )
//			{
//				// There are out parameters, we need to get a message back in return
//				Message *pResponse = pEvent->SendReceiveMessage( pMsg );
//				if( !pResponse || pResponse->m_dwID != 0 )
//				{
//					if(pResponse)
//					{
//						delete pResponse;
//						pResponse = NULL;
//					}
//					cout << "Failed to send message" << endl;
//					g_pPlutoLogger->Write(LV_DEBUG, "Failed to send message" );
//				}
//				else
//				{
//					cout << "0" << endl;
//					for( map<long, string>::iterator it=pResponse->m_mapParameters.begin();it!=pResponse->m_mapParameters.end();++it)
//					{
//						cout << (*it).first << ":" << (*it).second << endl;
//						returnValue = (*it).second;
//					}
//				}
//				delete pResponse;
//				pResponse = NULL;
//			}
//			else
//			{
//				g_pPlutoLogger->Write(LV_DEBUG, "message should have out parameters (PK_Device (int))");
//				cout << "message should have out parameters (PK_Device (int))" << ExpectedResponse << endl;
//			}
//		}
//		else
//		{
//			throw(string("pMsg == NULL"));
//		}
//	}
//	else
//		throw(string("pEvent == NULL"));
//}

void sendMessage(string params, string &returnValue)
{
	returnValue = "";
	
	g_pPlutoLogger->Write(LV_DEBUG, "MessageSend %s %s\n", hostname, params.c_str());
	
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
					g_pPlutoLogger->Write(LV_DEBUG,"Router not ready after 30 seconds.  Aborting....");
					throw(string("Router not ready after 30 seconds.  Aborting...."));
				}
				g_pPlutoLogger->Write(LV_DEBUG,"DCERouter still loading.  Waiting 5 seconds");
				Sleep(5000);
			}
			else
			{
				g_pPlutoLogger->Write(LV_DEBUG,"Router gave unknown response to ready request %s",sResponse.c_str());
				throw(string("Router gave unknown response to ready request"));
			}
		}

		Message *pMsg=new Message(params);
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
					g_pPlutoLogger->Write(LV_DEBUG, "Failed to send message" );
				}
				else
				{
					for( map<long, string>::iterator it=pResponse->m_mapParameters.begin();it!=pResponse->m_mapParameters.end();++it)
					{
						g_pPlutoLogger->Write(LV_DEBUG, "%ld : %s",  (*it).first , (*it).second.c_str() );
						returnValue = (*it).second;
					}
				}
				delete pResponse;
				pResponse = NULL;
			}
			else
			{
				g_pPlutoLogger->Write(LV_DEBUG, "message should have out parameters (PK_Device (int)) ");
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
//	static char last_udi[2048];

	gchar *bus = hal_device_get_property_string (ctx, udi, "info.bus");
	if( bus != NULL &&
		strcmp(bus, "usb_device") == 0 &&
		strlen(bus) == strlen("usb_device") )
	{
//		strcpy(last_udi, udi);
		//hal_device_print (ctx, udi);
		int usb_device_product_id = hal_device_get_property_int(ctx, udi, "usb_device.product_id");
		int usb_device_vendor_id = hal_device_get_property_int(ctx, udi, "usb_device.vendor_id");

		map<unsigned int, int>::iterator it;
		it = templatesMap.find(((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
		if(it != templatesMap.end())
		{
			gchar *info_udi = hal_device_get_property_string (ctx, udi, "info.udi");
			
			try
			{
				// check if there is a device with this UID
				string response;
				sendMessage(	"-targetType template -o 0 " + 
								StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
								" 1 " +
								StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ) + " " + 
								StringUtils::itos( COMMANDPARAMETER_UID_CONST ) + " " +
								info_udi, 
								response );
				g_pPlutoLogger->Write(LV_DEBUG, "response myDeviceAdded: DeviceFromUID = %s\n", response.c_str());
				
				if( !response.empty() )
				{
					// enable the device
					string responseEnable;
					sendMessage(	"-targetType template -o 0 " +
									StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) +
									" 1 " +
									StringUtils::itos( COMMAND_Set_Enable_Status_CONST ) + " " +
									StringUtils::itos( COMMANDPARAMETER_PK_Device_CONST ) + " " +
									response + " " +
									StringUtils::itos( COMMANDPARAMETER_Enable_CONST ) + " " +
									"1", 
									responseEnable );
					g_pPlutoLogger->Write(LV_DEBUG, "responseEnable myDeviceAdded: %s\n", responseEnable.c_str());
				}
				else
				{
					// create a new device with this UID
					DCEConfig dceConfig;
					
					char buffer[2048];
					snprintf(buffer, sizeof(buffer), "%d|%s", DEVICEDATA_UID_CONST, info_udi);

					string responseCreate;
					sendMessage(	"-targetType template -o 0 " + 
									StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
									" 1 " + 
									StringUtils::itos( COMMAND_Create_Device_CONST ) + " " + 
									StringUtils::itos( COMMANDPARAMETER_PK_DeviceTemplate_CONST ) + " " + 
									StringUtils::itos( (*it).second ) + " " +
									StringUtils::itos( COMMANDPARAMETER_PK_Device_Related_CONST ) + " " +
									StringUtils::itos( dceConfig.m_iPK_Device_Computer ) + " " +
									StringUtils::itos( COMMANDPARAMETER_PK_Room_CONST ) + 
									" -1 " + 
									"109 " + 
									buffer,
									responseCreate );
					g_pPlutoLogger->Write(LV_DEBUG, "responseCreate myDeviceAdded: %s\n", responseCreate.c_str());
					
/*					string responseRestart;
					sendMessage(string("-targetType template -o 0 ") + 
							StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
							" 1 " + 
							StringUtils::itos( COMMAND_Restart_DCERouter_CONST ),
						responseRestart );
					g_pPlutoLogger->Write(LV_DEBUG, "responseRestart myDeviceAdded: %s\n", responseRestart.c_str());*/
					
				}
			}
			catch(string ex)
			{
				g_pPlutoLogger->Write(LV_WARNING, "exception thrown: %s", ex.c_str());
			}
			
			g_free (info_udi);
			info_udi = NULL;
		}
	}

//	if(/*strncmp(last_udi, udi, strlen(last_udi)) == 0 && */strncmp(&udi[strlen(udi) - 10], "usb-serial", 10) == 0)
//	{
//	}

	g_free (bus);
	bus = NULL;
}

void myDeviceNewCapability(LibHalContext * ctx, const char * udi, const char *capability)
{
	gchar *serial_port = hal_device_get_property_string (ctx, udi, "serial.device");
	if(serial_port != NULL)
	{
		gchar *parent = hal_device_get_property_string (ctx, hal_device_get_property_string(ctx, udi, "info.parent"), "info.parent");
		gchar *info_udi = hal_device_get_property_string (ctx, parent, "info.udi");
		int usb_device_product_id = hal_device_get_property_int(ctx, parent, "usb_device.product_id");
		int usb_device_vendor_id = hal_device_get_property_int(ctx, parent, "usb_device.vendor_id");
		
		g_pPlutoLogger->Write(LV_DEBUG, "udi = %s parent = %s capability = %s serial port = %s\n", udi, parent, capability, serial_port);
		
		map<unsigned int, int>::iterator it;
		it = templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
		if( it != templatesMap.end() )
		{
			g_pPlutoLogger->Write(LV_DEBUG, "NewCapability: udi = %s serial port = %s\n", udi, serial_port);
			
			try
			{
				// check if there is a device with this UID
				string response;
				sendMessage(	"-targetType template -o 0 " + 
								StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
								" 1 " + 
								StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ) + " " +
								StringUtils::itos( COMMANDPARAMETER_UID_CONST ) + " " +
								info_udi,
								response );
				g_pPlutoLogger->Write(LV_DEBUG, "response NewCapability: DeviceFromUID = %s\n", response.c_str());
				
				if( !response.empty() )
				{
					// set the serial port for the device
					string responseSerial;
					sendMessage(	"-targetType template -o 0 " + 
									StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
									" 1 " + 
									StringUtils::itos( COMMAND_Set_Device_Data_CONST ) + " " +
									StringUtils::itos( COMMANDPARAMETER_PK_Device_CONST ) + " " +
									response + " " +
									StringUtils::itos( COMMANDPARAMETER_PK_DeviceData_CONST ) + " " +
									StringUtils::itos( DEVICEDATA_COM_Port_on_PC_CONST ) + " " +
									StringUtils::itos( COMMANDPARAMETER_Value_To_Assign_CONST ) + " " +
									serial_port,
									responseSerial );
					g_pPlutoLogger->Write(LV_DEBUG, "responseSerial NewCapability: %s\n", responseSerial.c_str());
				}
			}
			catch(string ex)
			{
				g_pPlutoLogger->Write(LV_WARNING, "exception thrown: %s", ex.c_str());
			}
		}
		
		g_free (parent);
		parent = NULL;
		g_free (info_udi);
		info_udi = NULL;
	}
	
	g_free (serial_port);
	serial_port = NULL;
}

void myDeviceRemoved(LibHalContext * ctx, const char * udi)
{
	g_pPlutoLogger->Write(LV_DEBUG, "removed device %s\n", udi);
	
	try
	{
		// check if there is a device with this UID
		string response;
		sendMessage(	"-targetType template -o 0 " + 
						StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
						" 1 " +
						StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ) + " " + 
						StringUtils::itos( COMMANDPARAMETER_UID_CONST ) + " " +
						udi, 
						response );
		g_pPlutoLogger->Write(LV_DEBUG, "response myDeviceRemoved: DeviceFromUID = %s\n", response.c_str());
		
		if( !response.empty() )
		{
			// disable the device
			string responseEnable;
			sendMessage(	"-targetType template -o 0 " +
							StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) +
							" 1 " +
							StringUtils::itos( COMMAND_Set_Enable_Status_CONST ) + " " +
							StringUtils::itos( COMMANDPARAMETER_PK_Device_CONST ) + " " +
							response + " " +
							StringUtils::itos( COMMANDPARAMETER_Enable_CONST ) + " " +
							"0", // false == disable
							responseEnable );
			g_pPlutoLogger->Write(LV_DEBUG, "responseEnable myDeviceRemoved: %s\n", responseEnable.c_str());
		}
	}
	catch(string ex)
	{
		g_pPlutoLogger->Write(LV_WARNING, "exception thrown: %s", ex.c_str());
	}
}

/***/
void initialize(LibHalContext * ctx)
{
	int num_devices = 0;
	char **devices = hal_get_all_devices (ctx, &num_devices);
	gchar *bus = NULL;

	//get all template IDs and producer_id vendor_id
	for(int i = num_devices - 1; i >= 0 ; i--)
	{
		char *udi = devices[i];
		g_pPlutoLogger->Write(LV_DEBUG, "init udi = %s\n", udi);
		bus = hal_device_get_property_string (ctx, udi, "info.bus");
		if( 0 == strcmp(bus, "usb_device") )
		{
			int usb_device_product_id = hal_device_get_property_int(ctx, udi, "usb_device.product_id");
			int usb_device_vendor_id = hal_device_get_property_int(ctx, udi, "usb_device.vendor_id");
			
			map<unsigned int, int>::iterator it =
				templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
			g_pPlutoLogger->Write(LV_DEBUG, "searching for %08x \n", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
			if( it != templatesMap.end() )
			{
//					gchar *product = hal_device_get_property_string (ctx, udi, "info.product");
//					gchar *vendor = hal_device_get_property_string (ctx, udi, "info.vendor");
//					gchar *serial = hal_device_get_property_string (ctx, udi, "usb_device.serial");
					gchar *info_udi = hal_device_get_property_string (ctx, udi, "info.udi");
//					gchar *sysfs_path = hal_device_get_property_string (ctx, udi, "usb_device.linux.sysfs_path");
					
				try
				{
					// check if there is a device with this UID
					string response;
					sendMessage(	"-targetType template -o 0 " + 
									StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
									" 1 " +
									StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ) + " " + 
									StringUtils::itos( COMMANDPARAMETER_UID_CONST ) + " " +
									info_udi, 
									response );
					g_pPlutoLogger->Write(LV_DEBUG, "response DeviceFromUID_ = %s\n", response.c_str());
					
					if( !response.empty() )
					{
						// enable the device
						string responseEnable;
						sendMessage(	"-targetType template -o 0 " +
										StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) +
										" 1 " +
										StringUtils::itos( COMMAND_Set_Enable_Status_CONST ) + " " +
										StringUtils::itos( COMMANDPARAMETER_PK_Device_CONST ) + " " +
										response + " " +
										StringUtils::itos( COMMANDPARAMETER_Enable_CONST ) + " " +
										"1", 
										responseEnable );
						g_pPlutoLogger->Write(LV_DEBUG, "responseEnable = %s\n", responseEnable.c_str());
					}
					else
					{
						// create a new device with this UID
						DCEConfig dceConfig;
						
						char buffer[2048];
						snprintf(buffer, sizeof(buffer), "%d|%s", DEVICEDATA_UID_CONST, info_udi);
						//TODO: hack to be removed
						if(dceConfig.m_iPK_Device_Computer == 0)
							dceConfig.m_iPK_Device_Computer = 1;
							
						string responseCreate;
						sendMessage(	"-targetType  template  -o 0 " + 
										StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
										" 1 " + 
										StringUtils::itos( COMMAND_Create_Device_CONST ) + " " + 
										StringUtils::itos( COMMANDPARAMETER_PK_DeviceTemplate_CONST ) + " " + 
										StringUtils::itos( (*it).second ) + " " +
										StringUtils::itos( COMMANDPARAMETER_PK_Device_Related_CONST ) + " " +
										StringUtils::itos( dceConfig.m_iPK_Device_Computer ) + " " +
										StringUtils::itos( COMMANDPARAMETER_PK_Room_CONST ) + 
										" -1 " + 
										"109 " + 
										buffer,
										responseCreate );
						g_pPlutoLogger->Write(LV_DEBUG, "responseCreate = %s\n", responseCreate.c_str());
					}
				}
				catch(string ex)
				{
					g_pPlutoLogger->Write(LV_WARNING, "exception thrown: %s", ex.c_str());
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
				templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
			if( it != templatesMap.end() )
			{
				gchar *serial_port = hal_device_get_property_string (ctx, udi, "serial.device");
				if(serial_port != NULL)
				{
					g_pPlutoLogger->Write(LV_DEBUG, "udi = %s serial port = %s\n", udi, serial_port);
					
					try
					{
						// check if there is a device with this UID
						string response;
						sendMessage(	"-targetType template -o 0 " + 
										StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
										" 1 " + 
										StringUtils::itos( COMMAND_Get_iPK_DeviceFromUID_CONST ) + " " +
										StringUtils::itos( COMMANDPARAMETER_UID_CONST ) + " " +
										info_udi,
										response );
						g_pPlutoLogger->Write(LV_DEBUG, "response DeviceFromUID = %s\n", response.c_str());
						
						if( !response.empty() )
						{
							// set the serial port for the device
							string responseSerial;
							sendMessage(	"-targetType template -o 0 " + 
											StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
											" 1 " + 
											StringUtils::itos( COMMAND_Set_Device_Data_CONST ) + " " +
											StringUtils::itos( COMMANDPARAMETER_PK_Device_CONST ) + " " +
											response + " " +
											StringUtils::itos( COMMANDPARAMETER_PK_DeviceData_CONST ) + " " +
											StringUtils::itos( DEVICEDATA_COM_Port_on_PC_CONST ) + " " +
											StringUtils::itos( COMMANDPARAMETER_Value_To_Assign_CONST ) + " " +
											serial_port, 
											responseSerial );
							g_pPlutoLogger->Write(LV_DEBUG, "responseSerial %s\n", responseSerial.c_str());
						}
					}
					catch(string ex)
					{
						g_pPlutoLogger->Write(LV_WARNING, "exception thrown: %s", ex.c_str());
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
// 	try
// 	{
// 		string responseRestart;
// 		sendMessage(string("-targetType template -o 0 ") + 
// 				StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
// 				" 1 " + 
// 				StringUtils::itos( COMMAND_Restart_DCERouter_CONST ),
// 			responseRestart );
// 		g_pPlutoLogger->Write(LV_DEBUG, "responseRestart %s\n", responseRestart.c_str());
// 	}
// 	catch(string ex)
// 	{
// 		g_pPlutoLogger->Write(LV_WARNING, "exception thrown: %s", ex.c_str());
// 	}

}

int main(int argc, char* argv[])
{
	LibHalFunctions funcs;
	LibHalContext * ctx = NULL;
	GMainLoop * loop = NULL;
	
	g_pPlutoLogger = new FileLogger("hal.log");
	
	if(argc >= 2)
	{
		size_t len = strlen(argv[1]);
		hostname = (char *)malloc(len + 1);
		if(hostname != NULL)
		{
			strncpy(hostname, argv[1], len);
		}
	}
	else
	{
		size_t len = strlen("localhost");
		hostname = (char *)malloc(len + 1);
		if(hostname != NULL)
		{
			strncpy(hostname, "localhost", len);
		}
	}

	//get the list of the templates and their corresponding product_id / vendor_id
	string response;
	try
	{
		sendMessage(	"-targetType template -o 0 " + 
						StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
						" 1 " +
						StringUtils::itos( COMMAND_Get_All_HAL_Model_ID_CONST ),
						response );
		g_pPlutoLogger->Write(LV_DEBUG, "response %s\n", response.c_str());
	}
	catch(string ex)
	{
		g_pPlutoLogger->Write(LV_WARNING, "exception %s\n", ex.c_str());
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
		g_pPlutoLogger->Write(LV_DEBUG, "%s", row.c_str());
		
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
	
	for(map<unsigned int, int>::iterator it = templatesMap.begin(); it != templatesMap.end(); ++it)
	{
		g_pPlutoLogger->Write(LV_DEBUG, "%08x --------- %d", (*it).first, (*it).second);
		g_pPlutoLogger->Write(LV_DEBUG, "%08x --------- %d\n", (*it).first, (*it).second);
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
	if ( ctx == NULL )
	{
		g_pPlutoLogger->Write(LV_DEBUG, "CTX is NULL!\n");
		return 1;
	}
	initialize(ctx);
	
	hal_device_property_watch_all(ctx);

	g_main_loop_run(loop);
	
	hal_shutdown(ctx);
	free(hostname);
	
	return 0;
}
