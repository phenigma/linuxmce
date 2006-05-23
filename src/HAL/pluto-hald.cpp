
#include "pluto-hald.h"
#include "HAL.h"

#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"
#include "DCE/DCEConfig.h"

#include "PlutoUtils/StringUtils.h"

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "Gen_Devices/AllCommandsRequests.h"

using namespace DCE;

using namespace std;

std::map<unsigned int, int> PlutoHalD::templatesMap;
HAL *PlutoHalD::halDevice = NULL;
long PlutoHalD::pnpDeviceID = 0;
GMainLoop * PlutoHalD::loop = NULL;
DBusError PlutoHalD::halError;

#define NEW_PNP

PlutoHalD::PlutoHalD()
{
}

void PlutoHalD::mainloop_integration (LibHalContext *ctx, DBusConnection * dbus_connection)
{
	dbus_connection_setup_with_g_main (dbus_connection, NULL);
}

void PlutoHalD::getPortIdentification(string portFromBus, string& portID)
{
	size_t startPos = portFromBus.find("usb");
	
	if( startPos != string::npos )
	{
		portID = portFromBus.substr(startPos);
		g_pPlutoLogger->Write(LV_DEBUG, "port ID = %s\n", portID.c_str());
	}
}

void PlutoHalD::myDeviceAdded(LibHalContext * ctx, const char * udi)
{
	if( ctx == NULL || udi == NULL )
	{
		// error
		return;
	}
	
	g_pPlutoLogger->Write(LV_DEBUG, "++++++++++++++++++ An usb device added");
	
	gchar *bus = libhal_device_get_property_string (ctx, udi, "info.bus", NULL);
	gchar *category = libhal_device_get_property_string (ctx, udi, "info.category", NULL);
	if( bus != NULL &&
		strcmp(bus, "usb_device") == 0 &&
		strlen(bus) == strlen("usb_device") )
	{
		int usb_device_product_id = libhal_device_get_property_int(ctx, udi, "usb_device.product_id", NULL);
		int usb_device_vendor_id = libhal_device_get_property_int(ctx, udi, "usb_device.vendor_id", NULL);

		g_pPlutoLogger->Write(LV_DEBUG, "An usb device added with vendor_id 0x%04x and product_id 0x%04x", 
										usb_device_vendor_id, 
										usb_device_product_id);
#ifdef NEW_PNP
// /MessageSend dcerouter -targetType category 999 159 2 65 52 4 51 "0403f850"
	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
	
	halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, udi, "");

/*	string response;
	if( !sendMessage(	"-targetType category " +
						StringUtils::itos( halDevice->m_dwPK_Device ) +
						+ " -1000 2 65 52 4 51 " + buffer,
						response ) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "ERROR: hald templates: %s", response.c_str());
	}*/
#else
		map<unsigned int, int>::iterator it;
		it = templatesMap.find(((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
		if(it != templatesMap.end())
		{
			gchar *info_udi = libhal_device_get_property_string (ctx, udi, "info.udi", NULL);
			
			// TODO
			// USB devices other than USB-Serial devices
			g_pPlutoLogger->Write(LV_DEBUG, "an known usb device added with vendor_id 0x%04x and product_id 0x%04x", 
											usb_device_vendor_id, 
											usb_device_product_id);
			
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "%08x", (*it).first);
			halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, udi, "");
			
			g_free (info_udi);
			info_udi = NULL;
		}
#endif
	}
	else if( category != NULL && 0 == strcmp(category, "serial") && strlen(category) == strlen("serial") )
	{
		gchar *parent = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "info.parent", NULL);
		gchar *info_udi = libhal_device_get_property_string (ctx, parent, "info.udi", NULL);
		int usb_device_product_id = libhal_device_get_property_int(ctx, parent, "usb_device.product_id", NULL);
		int usb_device_vendor_id = libhal_device_get_property_int(ctx, parent, "usb_device.vendor_id", NULL);
		
#ifdef NEW_PNP
// /MessageSend dcerouter -targetType category 999 159 2 65 55 "37|usb2/2-2/2-2:1.7" 52 4 51 "0403f850"
	gchar *serial_port = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "linux.sysfs_path", NULL);
	if(serial_port != NULL)
	{
		string portID;
		getPortIdentification(string(serial_port), portID);
		
		char buffer[64];
		snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
	
		halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, udi, "37|" + portID);
/*		string response;
		if( !sendMessage(	"-targetType category " +
							StringUtils::itos( halDevice->m_dwPK_Device ) +
							" -1000 2 65 55 \"37|" + portID +
							"\" 52 4 51 " + buffer,
							response ) )
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "ERROR: hald templates: %s", response.c_str());
		}*/
	}
	
	g_free (serial_port);
	serial_port = NULL;
#else
		map<unsigned int, int>::iterator it =
			templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
		if( it != templatesMap.end() )
		{
			gchar *serial_port = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "linux.sysfs_path", NULL);
			if(serial_port != NULL)
			{
				string portID;
				getPortIdentification(string(serial_port), portID);
				g_pPlutoLogger->Write(LV_DEBUG, "udi = %s serial port = %s port id = \n", udi, serial_port, portID.c_str());
				
				char buffer[64];
				snprintf(buffer, sizeof(buffer), "%08x", (*it).first);
				halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, udi, "37|" + portID);
			}
			
			g_free (serial_port);
			serial_port = NULL;
		}
#endif
		
		g_free (parent);
		parent = NULL;
		g_free (info_udi);
		info_udi = NULL;
	}

	g_free (bus);
	bus = NULL;
	g_free (category);
	category = NULL;
}

void PlutoHalD::myDeviceNewCapability(LibHalContext * ctx, const char * udi, const char *capability)
{
	if( udi == NULL || ctx == NULL || capability == NULL )
	{
		// error
		return;
	}
	
	gchar *serial_port = libhal_device_get_property_string (ctx, udi, "linux.sysfs_path", NULL);
	if(serial_port != NULL)
	{
		gchar *parent = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "info.parent", NULL);
		gchar *info_udi = libhal_device_get_property_string (ctx, parent, "info.udi", NULL);
		int usb_device_product_id = libhal_device_get_property_int(ctx, parent, "usb_device.product_id", NULL);
		int usb_device_vendor_id = libhal_device_get_property_int(ctx, parent, "usb_device.vendor_id", NULL);
		
		g_pPlutoLogger->Write(LV_DEBUG, "udi = %s parent = %s capability = %s serial port = %s\n", udi, parent, capability, serial_port);
		
		map<unsigned int, int>::iterator it;
		it = templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
		if( it != templatesMap.end() )
		{
			string portID;
			getPortIdentification(string(serial_port), portID);
			
			g_pPlutoLogger->Write(LV_DEBUG, "NewCapability udi = %s serial port = %s port id = \n", udi, serial_port, portID.c_str());
			string responseCreate;
			
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "%08x", (*it).first);
			halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, info_udi, "37|" + portID);
		}
		
		g_free (parent);
		parent = NULL;
		g_free (info_udi);
		info_udi = NULL;
	}
	
	g_free (serial_port);
	serial_port = NULL;
}

void PlutoHalD::myDeviceRemoved(LibHalContext * ctx, const char * udi)
{
	if( ctx != NULL && udi != NULL )
	{
		g_pPlutoLogger->Write(LV_DEBUG, "--------------- Removed = %s", udi);
		
		halDevice->EVENT_Device_Removed("", "", 0, "", 0, "", 4, 0, udi, "");
	}
	else
	{
		// error
		g_pPlutoLogger->Write(LV_WARNING, "error on HAL passing null arguments to remove");
	}
}

/***/
void PlutoHalD::initialize(LibHalContext * ctx)
{
	int num_devices = 0;
	char **devices = libhal_get_all_devices (ctx, &num_devices, NULL);
	gchar *bus = NULL;
	gchar *category = NULL;

	//get all template IDs and producer_id vendor_id
	for(int i = num_devices - 1; i >= 0 ; i--)
	{
		char *udi = devices[i];
		g_pPlutoLogger->Write(LV_DEBUG, "init udi = %s\n", udi);
		bus = libhal_device_get_property_string (ctx, udi, "info.bus", NULL);
		category = libhal_device_get_property_string (ctx, udi, "info.category", NULL);
		if( bus != NULL && 0 == strcmp(bus, "usb_device") && strlen(bus) == strlen("usb_device") )
		{
			int usb_device_product_id = libhal_device_get_property_int(ctx, udi, "usb_device.product_id", NULL);
			int usb_device_vendor_id = libhal_device_get_property_int(ctx, udi, "usb_device.vendor_id", NULL);
			
#ifdef NEW_PNP
// /MessageSend dcerouter -targetType category 999 159 2 65 52 4 51 "0403f850"
	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
	
	halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, udi, "");
#else
			map<unsigned int, int>::iterator it =
				templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
			g_pPlutoLogger->Write(LV_DEBUG, "searching for %08x \n", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
			if( it != templatesMap.end() )
			{
//					gchar *product = libhal_device_get_property_string (ctx, udi, "info.product");
//					gchar *vendor = libhal_device_get_property_string (ctx, udi, "info.vendor");
//					gchar *serial = libhal_device_get_property_string (ctx, udi, "usb_device.serial");
					gchar *info_udi = libhal_device_get_property_string (ctx, udi, "info.udi", NULL);
//					gchar *sysfs_path = libhal_device_get_property_string (ctx, udi, "usb_device.linux.sysfs_path");
					
					// TODO
					// USB devices other than USB-Serial devices
					g_pPlutoLogger->Write(LV_DEBUG, "an known usb device added with vendor_id 0x%04x and product_id 0x%04x", 
													usb_device_vendor_id, 
													usb_device_product_id);
					
					char buffer[64];
					snprintf(buffer, sizeof(buffer), "%08x", (*it).first);
					halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, udi, "");
					
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
#endif
		}
		else if( category != NULL && 0 == strcmp(category, "serial") && strlen(category) == strlen("serial") )
		{
			gchar *parent = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "info.parent", NULL);
			gchar *info_udi = libhal_device_get_property_string (ctx, parent, "info.udi", NULL);
			int usb_device_product_id = libhal_device_get_property_int(ctx, parent, "usb_device.product_id", NULL);
			int usb_device_vendor_id = libhal_device_get_property_int(ctx, parent, "usb_device.vendor_id", NULL);
			
#ifdef NEW_PNP
// /MessageSend dcerouter -targetType category 999 159 2 65 55 "37|usb2/2-2/2-2:1.7" 52 4 51 "0403f850"
			gchar *serial_port = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "linux.sysfs_path", NULL);
			if(serial_port != NULL)
			{
				string portID;
				getPortIdentification(string(serial_port), portID);
				
				char buffer[64];
				snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
			
				halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, udi, "37|" + portID);
			}
			
			g_free (serial_port);
			serial_port = NULL;
#else
			map<unsigned int, int>::iterator it =
				templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
			if( it != templatesMap.end() )
			{
				gchar *serial_port = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "linux.sysfs_path", NULL);
				if(serial_port != NULL)
				{
					string portID;
					getPortIdentification(string(serial_port), portID);
					g_pPlutoLogger->Write(LV_DEBUG, "udi = %s serial port = %s port id = \n", udi, serial_port, portID.c_str());
					
					char buffer[64];
					snprintf(buffer, sizeof(buffer), "%08x", (*it).first);
					halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, udi, "37|" + portID);
				}
				
				g_free (serial_port);
				serial_port = NULL;
			}
#endif
			
			g_free (parent);
			parent = NULL;
			g_free (info_udi);
			info_udi = NULL;
		}
		
		g_free(bus);
		bus = NULL;
		g_free(category);
		category = NULL;
	}
}

void* PlutoHalD::startUp(void *pnp)
{
	g_pPlutoLogger->Write(LV_DEBUG, "############ Start ");
	
	if( pnp == NULL )
	{
		// error
		return NULL;
	}
	
	LibHalContext * ctx = NULL;
	
	halDevice = (HAL *)pnp;
	if( halDevice->m_pData == NULL )
	{
		// error
		return NULL;
	}

#ifndef NEW_PNP
	pnpDeviceID = halDevice->m_pData->m_dwPK_Device_ControlledVia;
	if( !pnpDeviceID )
	{
		// error
		return NULL;
	}

	//get the list of the templates and their corresponding product_id / vendor_id
	string response;
	if( !sendMessage(	"-targetType template -o 0 " + 
		StringUtils::itos( DEVICETEMPLATE_General_Info_Plugin_CONST ) + 
		" 1 " +
		StringUtils::itos( COMMAND_Get_All_HAL_Model_ID_CONST ),
		response ) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "ERROR: hald templates: %s", response.c_str());
		return NULL;
	}
	g_pPlutoLogger->Write(LV_DEBUG, "template response %s\n", response.c_str());

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
#endif
	
	g_pPlutoLogger->Write(LV_DEBUG, "############ Start 1");
	
	loop = g_main_loop_new (NULL, FALSE);
	
	DBusConnection * halConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &halError);
	if( halConnection == NULL )
	{
		g_pPlutoLogger->Write(LV_DEBUG, "DBusConnection is NULL!\n");
		return NULL;
	}
	
	ctx = libhal_ctx_new();
	if ( ctx == NULL )
	{
		g_pPlutoLogger->Write(LV_DEBUG, "CTX is NULL!\n");
		return NULL;
	}
	
	g_pPlutoLogger->Write(LV_DEBUG, "############ Start 2");
	
	libhal_ctx_set_device_added(ctx, myDeviceAdded);
	libhal_ctx_set_device_removed(ctx, myDeviceRemoved);

// we don't need capability now
//	libhal_ctx_set_device_new_capability(ctx, myDeviceNewCapability);
	
	libhal_ctx_set_dbus_connection(ctx, halConnection);
	mainloop_integration(ctx, libhal_ctx_get_dbus_connection(ctx));
	
	g_pPlutoLogger->Write(LV_DEBUG, "############ Start 3");
	
	libhal_ctx_init(ctx, NULL);
	
	initialize(ctx);
	
	libhal_device_property_watch_all(ctx, &halError);

	g_pPlutoLogger->Write(LV_DEBUG, "############ Start 4");
	
	g_main_loop_run(loop);
	
	libhal_ctx_shutdown(ctx, NULL);
	libhal_ctx_free(ctx);
	
	g_pPlutoLogger->Write(LV_DEBUG, "############ END ----------- ");
	
	return NULL;
}

void PlutoHalD::shutDown()
{
	if( loop != NULL )
		g_main_loop_quit(loop);
}
