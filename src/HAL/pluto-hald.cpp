/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

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

#define UNKNOWN_COMM_METHOD  11
#define USB_COMM_METHOD       4
#define PCI_COMM_METHOD       8

using namespace DCE;

using namespace std;

std::map<unsigned int, int> PlutoHalD::templatesMap;
HAL *PlutoHalD::halDevice = NULL;
long PlutoHalD::pnpDeviceID = 0;
GMainLoop * PlutoHalD::loop = NULL;
DBusError PlutoHalD::halError;
bool PlutoHalD::threadShutdown = false;

PlutoHalD::PlutoHalD()
{
}

void PlutoHalD::mainloop_integration (LibHalContext *ctx, DBusConnection * dbus_connection)
{
	dbus_connection_setup_with_g_main (dbus_connection, NULL);
}

void PlutoHalD::getPortIdentification(string portFromBus, string& portID)
{
	size_t startPos = portFromBus.find("pci");
	size_t usbPos = portFromBus.find("/usb");
	size_t minus = portFromBus.rfind("-");
	size_t colon = portFromBus.rfind(":");
	
	if( startPos != string::npos && usbPos != string::npos && usbPos > startPos &&
		minus    != string::npos && colon  != string::npos && colon > minus )
	{
		// Razvan G. 
		// /sys/devices/pci0000:00/0000:00:02.0/usb1/1-4/1-4.5/1-4.5:1.0 => pci0000:00/0000:00:02.0+4.5
		portID = portFromBus.substr(startPos, usbPos - startPos);
		portID += "+";
		portID += portFromBus.substr(minus + 1, colon - minus - 1);
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "port ID = %s\n", portID.c_str());
	}
}

void PlutoHalD::myDeviceAdded(LibHalContext * ctx, const char * udi)
{
	if( ctx == NULL || udi == NULL )
	{
		// error
		return;
	}
	
	gchar *bus = libhal_device_get_property_string (ctx, udi, "info.bus", NULL);
	gchar *category = libhal_device_get_property_string (ctx, udi, "info.category", NULL);
	if( bus != NULL &&
		strcmp(bus, "usb_device") == 0 &&
		strlen(bus) == strlen("usb_device") )
	{
		int usb_device_product_id = libhal_device_get_property_int(ctx, udi, "usb_device.product_id", NULL);
		int usb_device_vendor_id = libhal_device_get_property_int(ctx, udi, "usb_device.vendor_id", NULL);

		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "An usb device added with vendor_id 0x%04x and product_id 0x%04x", 
										usb_device_vendor_id, 
										usb_device_product_id);
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++ New device added = %s", udi);
		
		char buffer[64];
		snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
		
		halDevice->EVENT_Device_Detected("", "", "", 0, buffer, USB_COMM_METHOD, 0, udi, "", "");
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
	}
	else if( category != NULL )
	{
		if(  0 == strcmp(category, "serial") && strlen(category) == strlen("serial") )
		{
			gchar *parent = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "info.parent", NULL);
			gchar *info_udi = libhal_device_get_property_string (ctx, parent, "info.udi", NULL);
			int usb_device_product_id = libhal_device_get_property_int(ctx, parent, "usb_device.product_id", NULL);
			int usb_device_vendor_id = libhal_device_get_property_int(ctx, parent, "usb_device.vendor_id", NULL);
			
		// /MessageSend dcerouter -targetType category 999 159 2 65 55 "37|usb2/2-2/2-2:1.7" 52 4 51 "0403f850"
			gchar *serial_port = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "linux.sysfs_path", NULL);
			if(serial_port != NULL)
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++ Serial device added = %s", udi);
				
				string portID;
				getPortIdentification(string(serial_port), portID);
				
				char buffer[64];
				snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
			
				halDevice->EVENT_Device_Detected("", "", "", 0, buffer, USB_COMM_METHOD, 0, info_udi, "37|" + portID, category);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
			}
		
			g_free (serial_port);
			serial_port = NULL;
			
			g_free (parent);
			parent = NULL;
			g_free (info_udi);
			info_udi = NULL;
		}
		else if( 0 == strcmp(category, "bluetooth_hci") && strlen(category) == strlen("bluetooth_hci") )
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++ Bluetooth device added = %s", udi);
			
			gchar *parent = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "info.parent", NULL);
			gchar *info_udi = libhal_device_get_property_string (ctx, parent, "info.udi", NULL);
			int usb_device_product_id = libhal_device_get_property_int(ctx, parent, "usb_device.product_id", NULL);
			int usb_device_vendor_id = libhal_device_get_property_int(ctx, parent, "usb_device.vendor_id", NULL);
			
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
		
			halDevice->EVENT_Device_Detected("", "", "", 0, buffer, USB_COMM_METHOD, 0, info_udi, "", category);
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
			
			g_free (parent);
			parent = NULL;
			g_free (info_udi);
			info_udi = NULL;
		}
		else if( 0 == strcmp(category, "volume") && strlen(category) == strlen("volume") )
		{
			// let's see if it's an usb volume first
			gchar *parent = libhal_device_get_property_string(ctx, udi, "info.parent", NULL);
			if( parent != NULL )
			{
				gchar *storage = libhal_device_get_property_string(ctx, parent, "storage.bus", NULL);
				if( storage != NULL && 0 == strcmp(storage, "usb") && strlen(storage) == strlen("usb") )
				{
					// search for prod_id and vendor_id
					int usb_device_product_id = 0;
					int usb_device_vendor_id = 0;
					int subsys_usb_device_product_id = 0;
					int subsys_usb_device_vendor_id = 0;
					int iBusType = USB_COMM_METHOD;
					string sysfsPath;
					getProductVendorId(
						ctx, udi,
						&usb_device_product_id, &usb_device_vendor_id,
						&subsys_usb_device_product_id, &subsys_usb_device_vendor_id,
						&iBusType , sysfsPath);
					
					char buffer[64];
					snprintf(buffer, sizeof(buffer), "%08x%08x",
						(unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff),
						(unsigned int) ((subsys_usb_device_vendor_id & 0xffff) << 16) | (subsys_usb_device_product_id & 0xffff));
					
					if( usb_device_product_id && usb_device_vendor_id )
					{
						LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ USB Volume device added = %s", udi);
						
						// command parameters
						string deviceData;
						
						// TODO is_disc ??
						
						gchar *fstype = libhal_device_get_property_string(ctx, udi, "volume.fstype", NULL);
						if( fstype != NULL )
						{
							deviceData += StringUtils::itos( DEVICEDATA_Filesystem_CONST );
							deviceData += "|";
							deviceData += fstype;
						}
						
						gchar *blockdevice = libhal_device_get_property_string(ctx, udi, "block.device", NULL);
						if( blockdevice != NULL )
						{
							if( !deviceData.empty() )
							{
								deviceData += "|";
							}
							deviceData += StringUtils::itos( DEVICEDATA_Block_Device_CONST );
							deviceData += "|";
							deviceData += blockdevice;
						}
						
						halDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), category);
						
						LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
			
						g_free(blockdevice);
						blockdevice = NULL;
						g_free(fstype);
						fstype = NULL;
					}
				}
			}
		}
		// general category
		else
		{
			// search for prod_id and vendor_id
			int device_product_id = 0;
			int device_vendor_id = 0;
			int subsys_device_product_id = 0;
			int subsys_device_vendor_id = 0;
			int iBusType = UNKNOWN_COMM_METHOD;
			string sysfsPath;
			getProductVendorId(
				ctx, udi,
				&device_product_id, &device_vendor_id,
				&subsys_device_product_id, &subsys_device_vendor_id,
				&iBusType , sysfsPath );
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "%08x%08x",
				(unsigned int) ((device_vendor_id & 0xffff) << 16) | (device_product_id & 0xffff),
				(unsigned int) ((subsys_device_vendor_id & 0xffff) << 16) | (subsys_device_product_id & 0xffff));
			
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ General category = %s || UID = %s", category, udi);
			
			string categoryDeviceKey = category;
			categoryDeviceKey += ".device";
			
			gchar *categoryDevice = libhal_device_get_property_string(ctx, udi, categoryDeviceKey.c_str(), NULL);
			if( categoryDevice == NULL )
			{
				string categoryDeviceKey = category;
				categoryDeviceKey += ".device_file";
				categoryDevice = libhal_device_get_property_string(ctx, udi, categoryDeviceKey.c_str(), NULL);
			}
			
			// command parameters
			string deviceData;
			if( categoryDevice != NULL )
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ Device category = %s", categoryDevice);
				
				deviceData += StringUtils::itos( DEVICEDATA_Block_Device_CONST );
				deviceData += "|";
				deviceData += categoryDevice;
			}
			
			if( !sysfsPath.empty() )
			{
				if( !deviceData.empty() )
				{
					deviceData += "|";
				}
				
				deviceData += StringUtils::itos( DEVICEDATA_Location_on_PCI_bus_CONST );
				deviceData += "|";
				deviceData += sysfsPath.c_str();
			}
			
			halDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), category);

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
			
			g_free(categoryDevice);
			categoryDevice = NULL;
		}
	}
	else if( bus != NULL &&
			strcmp(bus, "pci") == 0 &&
			strlen(bus) == strlen("pci") )
	{
			int device_product_id = libhal_device_get_property_int(ctx, udi, "pci.product_id", NULL);
			int device_vendor_id = libhal_device_get_property_int(ctx, udi, "pci.vendor_id", NULL);
			int subsys_device_product_id = libhal_device_get_property_int(ctx, udi, "pci.subsys_product_id", NULL);
			int subsys_device_vendor_id = libhal_device_get_property_int(ctx, udi, "pci.subsys_vendor_id", NULL);
			
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "%08x%08x",
				(unsigned int) ((device_vendor_id & 0xffff) << 16) | (device_product_id & 0xffff),
				(unsigned int) ((subsys_device_vendor_id & 0xffff) << 16) | (subsys_device_product_id & 0xffff));
			
			halDevice->EVENT_Device_Detected("", "", "", 0, buffer, PCI_COMM_METHOD, 0, udi, "", "");

			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Not processing bus: %s category: %s",bus ? bus : "*none*", category ? category : "*none");
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
		
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "udi = %s parent = %s capability = %s serial port = %s\n", udi, parent, capability, serial_port);
		
		map<unsigned int, int>::iterator it;
		it = templatesMap.find( (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff) );
		if( it != templatesMap.end() )
		{
			string portID;
			getPortIdentification(string(serial_port), portID);
			
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "NewCapability udi = %s serial port = %s port id = \n", udi, serial_port, portID.c_str());
			string responseCreate;
			
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "%08x", (*it).first);
			halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, info_udi, "37|" + portID, "");
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
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
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "--------------- Removed = %s", udi);
		
		halDevice->EVENT_Device_Removed("", "", 0, "", 0, "", 4, 0, udi, "", "");
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event");
	}
	else
	{
		// error
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "error on HAL passing null arguments to remove");
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
		
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "init udi = %s\n", udi);
		
		bus = libhal_device_get_property_string (ctx, udi, "info.bus", NULL);
		category = libhal_device_get_property_string (ctx, udi, "info.category", NULL);
		if( bus != NULL && 0 == strcmp(bus, "usb_device") && strlen(bus) == strlen("usb_device") )
		{
			int usb_device_product_id = libhal_device_get_property_int(ctx, udi, "usb_device.product_id", NULL);
			int usb_device_vendor_id = libhal_device_get_property_int(ctx, udi, "usb_device.vendor_id", NULL);
			
// /MessageSend dcerouter -targetType category 999 159 2 65 52 4 51 "0403f850"
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
			
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ New device added = %s", udi);
			
			halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, udi, "", "");
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
		}
		else if( category != NULL )
		{
			if( 0 == strcmp(category, "serial") && strlen(category) == strlen("serial") )
			{
				gchar *parent = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "info.parent", NULL);
				gchar *info_udi = libhal_device_get_property_string (ctx, parent, "info.udi", NULL);
				int usb_device_product_id = libhal_device_get_property_int(ctx, parent, "usb_device.product_id", NULL);
				int usb_device_vendor_id = libhal_device_get_property_int(ctx, parent, "usb_device.vendor_id", NULL);
				
	// /MessageSend dcerouter -targetType category 999 159 2 65 55 "37|usb2/2-2/2-2:1.7" 52 4 51 "0403f850"
				gchar *serial_port = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "linux.sysfs_path", NULL);
				if(serial_port != NULL)
				{
					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ Serial device added = %s", udi);
					
					string portID;
					getPortIdentification(string(serial_port), portID);
					
					char buffer[64];
					snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
				
					halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, info_udi, "37|" + portID, category);
					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
				}
				
				g_free (serial_port);
				serial_port = NULL;
				
				g_free (parent);
				parent = NULL;
				g_free (info_udi);
				info_udi = NULL;
			}
			else if( 0 == strcmp(category, "bluetooth_hci") && strlen(category) == strlen("bluetooth_hci") )
			{
				gchar *parent = libhal_device_get_property_string (ctx, libhal_device_get_property_string(ctx, udi, "info.parent", NULL), "info.parent", NULL);
				gchar *info_udi = libhal_device_get_property_string (ctx, parent, "info.udi", NULL);
				
				int usb_device_product_id = libhal_device_get_property_int(ctx, parent, "usb_device.product_id", NULL);
				int usb_device_vendor_id = libhal_device_get_property_int(ctx, parent, "usb_device.vendor_id", NULL);
				
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ Bluetooth device added = %s", udi);
				
				char buffer[64];
				snprintf(buffer, sizeof(buffer), "%08x", (unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff));
				
				halDevice->EVENT_Device_Detected("", "", "", 0, buffer, 4, 0, info_udi, "", category);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
				
				g_free (parent);
				parent = NULL;
				g_free (info_udi);
				info_udi = NULL;
			}
			else if( 0 == strcmp(category, "volume") && strlen(category) == strlen("volume") )
			{
				// let's see if it's an usb volume first
				gchar *parent = libhal_device_get_property_string(ctx, udi, "info.parent", NULL);
				if( parent != NULL )
				{
					gchar *storage = libhal_device_get_property_string(ctx, parent, "storage.bus", NULL);
					if( storage != NULL && 0 == strcmp(storage, "usb") && strlen(storage) == strlen("usb") )
					{
						// search for prod_id and vendor_id
						int usb_device_product_id = 0;
						int usb_device_vendor_id = 0;
						int subsys_usb_device_product_id = 0;
						int subsys_usb_device_vendor_id = 0;
						int iBusType = USB_COMM_METHOD;
						string sysfsPath;
						getProductVendorId(
							ctx, udi,
							&usb_device_product_id, &usb_device_vendor_id,
							&subsys_usb_device_product_id, &subsys_usb_device_vendor_id,
							&iBusType, sysfsPath );
						
						char buffer[64];
						snprintf(buffer, sizeof(buffer), "%08x%08x",
							(unsigned int) ((usb_device_vendor_id & 0xffff) << 16) | (usb_device_product_id & 0xffff),
							(unsigned int) ((subsys_usb_device_vendor_id & 0xffff) << 16) | (subsys_usb_device_product_id & 0xffff));
						
						if( usb_device_product_id && usb_device_vendor_id )
						{
							LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ USB Volume device added = %s", udi);
							
							// command parameters
							string deviceData;
							
							// TODO is_disc ??
							
							gchar *fstype = libhal_device_get_property_string(ctx, udi, "volume.fstype", NULL);
							if( fstype != NULL )
							{
								deviceData += StringUtils::itos( DEVICEDATA_Filesystem_CONST );
								deviceData += "|";
								deviceData += fstype;
							}
							
							gchar *blockdevice = libhal_device_get_property_string(ctx, udi, "block.device", NULL);
							if( blockdevice != NULL )
							{
								if( !deviceData.empty() )
								{
									deviceData += "|";
								}
								deviceData += StringUtils::itos( DEVICEDATA_Block_Device_CONST );
								deviceData += "|";
								deviceData += blockdevice;
							}
							
							halDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), category);
							
							LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
				
							g_free(blockdevice);
							blockdevice = NULL;
							g_free(fstype);
							fstype = NULL;
						}
					}
				}
			}
			// general category
			else
			{
				// search for prod_id and vendor_id
				int device_product_id = 0;
				int device_vendor_id = 0;
				int subsys_device_product_id = 0;
				int subsys_device_vendor_id = 0;
				int iBusType = UNKNOWN_COMM_METHOD;
				string sysfsPath;
				getProductVendorId(
					ctx, udi,
					&device_product_id, &device_vendor_id,
					&subsys_device_product_id, &subsys_device_vendor_id,
					&iBusType, sysfsPath );
				char buffer[64];
				snprintf(buffer, sizeof(buffer), "%08x%08x",
					(unsigned int) ((device_vendor_id & 0xffff) << 16) | (device_product_id & 0xffff),
					(unsigned int) ((subsys_device_vendor_id & 0xffff) << 16) | (subsys_device_product_id & 0xffff));
				
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ General category = %s || UID = %s", category, udi);
				
				string categoryDeviceKey = category;
				categoryDeviceKey += ".device";
				
				gchar *categoryDevice = libhal_device_get_property_string(ctx, udi, categoryDeviceKey.c_str(), NULL);
				if( categoryDevice == NULL )
				{
					string categoryDeviceKey = category;
					categoryDeviceKey += ".device_file";
					categoryDevice = libhal_device_get_property_string(ctx, udi, categoryDeviceKey.c_str(), NULL);
				}
				
				// command parameters
				string deviceData;
				if( categoryDevice != NULL )
				{
					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "+++++++ Device category = %s", categoryDevice);
					
					deviceData += StringUtils::itos( DEVICEDATA_Block_Device_CONST );
					deviceData += "|";
					deviceData += categoryDevice;
				}
				
				if( !sysfsPath.empty() )
				{
					if( !deviceData.empty() )
					{
						deviceData += "|";
					}
					
					deviceData += StringUtils::itos( DEVICEDATA_Location_on_PCI_bus_CONST );
					deviceData += "|";
					deviceData += sysfsPath.c_str();
				}
			
				halDevice->EVENT_Device_Detected("", "", "", 0, buffer, iBusType, 0, udi, deviceData.c_str(), category);

				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
				
				g_free(categoryDevice);
				categoryDevice = NULL;
			}
		}
		else if( bus != NULL &&
				strcmp(bus, "pci") == 0 &&
				strlen(bus) == strlen("pci") )
		{
				int device_product_id = libhal_device_get_property_int(ctx, udi, "pci.product_id", NULL);
				int device_vendor_id = libhal_device_get_property_int(ctx, udi, "pci.vendor_id", NULL);
				int subsys_device_product_id = libhal_device_get_property_int(ctx, udi, "pci.subsys_product_id", NULL);
				int subsys_device_vendor_id = libhal_device_get_property_int(ctx, udi, "pci.subsys_vendor_id", NULL);
				
				char buffer[64];
				snprintf(buffer, sizeof(buffer), "%08x%08x",
					(unsigned int) ((device_vendor_id & 0xffff) << 16) | (device_product_id & 0xffff),
					(unsigned int) ((subsys_device_vendor_id & 0xffff) << 16) | (subsys_device_product_id & 0xffff));
				
				halDevice->EVENT_Device_Detected("", "", "", 0, buffer, PCI_COMM_METHOD, 0, udi, "", "");
	
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Finished firing event for %s",buffer);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Not processing bus: %s category: %s",bus ? bus : "*none*", category ? category : "*none");
		}
		
		g_free(bus);
		bus = NULL;
		g_free(category);
		category = NULL;
	}
}

void PlutoHalD::getProductVendorId(	LibHalContext * ctx, const char * udi,
									int * prodId, int * vendorId,
									int * subsysProdId, int * subsysVendorId,
									int * busType , string & sysfsPath )
{
	gchar *bus = libhal_device_get_property_string (ctx, udi, "info.bus", NULL);
	if( bus != NULL )
	{
		string prodIdKey = bus;
		string vendorIdKey = bus;
		string subsysProdIdKey = bus;
		string subsysVendorIdKey = bus;
		prodIdKey += ".product_id";
		vendorIdKey += ".vendor_id";
		subsysProdIdKey += ".subsys_product_id";
		subsysVendorIdKey += ".subsys_vendor_id";
		
		*prodId = libhal_device_get_property_int(ctx, udi, prodIdKey.c_str(), NULL);
		*vendorId = libhal_device_get_property_int(ctx, udi, vendorIdKey.c_str(), NULL);
		*subsysProdId = libhal_device_get_property_int(ctx, udi, subsysProdIdKey.c_str(), NULL);
		*subsysVendorId = libhal_device_get_property_int(ctx, udi, subsysVendorIdKey.c_str(), NULL);
		
		gchar *sysfs_path = libhal_device_get_property_string(ctx, udi, "linux.sysfs_path", NULL);
		if( sysfs_path != NULL )
		{
			sysfsPath = sysfs_path;
			size_t startPos = sysfsPath.find("pci");
			if( startPos != string::npos )
			{
				sysfsPath = sysfsPath.substr(startPos);
			}
		}
		g_free(sysfs_path);
		sysfs_path = NULL;
		
		if( *prodId == -1 || *vendorId == -1 )
		{
			gchar *parent = libhal_device_get_property_string(ctx, udi, "info.parent", NULL);
			if( parent != NULL )
			{
				getProductVendorId(ctx, parent, prodId, vendorId, subsysProdId, subsysVendorId, busType, sysfsPath);
			}
			g_free(parent);
			parent = NULL;
		}
		
		if( 0 == strcmp(bus, "pci") && strlen(bus) == strlen("pci") )
		{
			*busType = 8;
		}
		else if( 0 == strcmp(bus, "usb") && strlen(bus) == strlen("usb") )
		{
			*busType = 4;
		}
		else if( 0 == strcmp(bus, "usb_device") && strlen(bus) == strlen("usb_device") )
		{
			*busType = 4;
		}
		else if( 0 == strcmp(bus, "ide") && strlen(bus) == strlen("ide") )
		{
			*busType = 9;
		}
		else if( 0 == strcmp(bus, "scsi") && strlen(bus) == strlen("scsi") )
		{
			*busType = 10;
		}
		else // unknown
		{
			*busType = 11;
		}
	}
	else
	{
		gchar *parent = libhal_device_get_property_string(ctx, udi, "info.parent", NULL);
		if( parent != NULL )
		{
			getProductVendorId(ctx, parent, prodId, vendorId, subsysProdId, subsysVendorId, busType, sysfsPath);
		}
		g_free(parent);
		parent = NULL;
	}
	g_free(bus);
	bus = NULL;
}

void* PlutoHalD::startUp(void *device)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoHalD::startUp  Waiting 10 seconds to let GSD devices start first and disable any invalid ports");
	Sleep(10000);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "############ Start ");
	
	if( device == NULL )
	{
		// error
		return NULL;
	}
	
	LibHalContext * ctx = NULL;
	
	halDevice = (HAL *)device;
	if( halDevice->m_pData == NULL )
	{
		// error
		return NULL;
	}

	dbus_error_init(&halError);
	
	loop = g_main_loop_new (NULL, FALSE);
	
	DBusConnection * halConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &halError);
	if( halConnection == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "DBusConnection is NULL!\n");
		return NULL;
	}
	
	ctx = libhal_ctx_new();
	if ( ctx == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "CTX is NULL!\n");
		return NULL;
	}
	
	libhal_ctx_set_device_added(ctx, myDeviceAdded);
	libhal_ctx_set_device_removed(ctx, myDeviceRemoved);

// we don't need capability now
//	libhal_ctx_set_device_new_capability(ctx, myDeviceNewCapability);
	
	libhal_ctx_set_dbus_connection(ctx, halConnection);
	mainloop_integration(ctx, libhal_ctx_get_dbus_connection(ctx));
	
	if( !libhal_ctx_init(ctx, &halError) )
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "CTX initialization failed!\n");
		dbus_error_free(&halError);
		libhal_ctx_free(ctx);
		return NULL;
	}
	
	initialize(ctx);
	
	libhal_device_property_watch_all(ctx, &halError);

	if( dbus_error_is_set (&halError) )
		dbus_error_free (&halError);
	
	dbus_error_init(&halError);
	
	/* Every second call send_ping() with the bus as an argument*/
	g_timeout_add(500, (GSourceFunc)timeoutHandler, halConnection);
	
	g_main_loop_run(loop);
	
	if( dbus_error_is_set (&halError) )
		dbus_error_free (&halError);
	
	dbus_error_init(&halError);
	
	libhal_ctx_shutdown(ctx, &halError);
	libhal_ctx_free(ctx);
	
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "############ END ----------- ");
	
	threadShutdown = false;
	return NULL;
}

gboolean PlutoHalD::timeoutHandler(DBusConnection *bus)
{
	if( loop != NULL && threadShutdown )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "############ QUIT ++++++++++++++");
		g_main_loop_quit(loop);
	}
	return TRUE;
}

void PlutoHalD::shutDown()
{
	threadShutdown = true;
	
	while( threadShutdown )
	{
		usleep(10);
	}
}
