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
#ifndef PLUTOHALD_H_
#define PLUTOHALD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <string>

#include <dbus/dbus.h>
#include <hal/libhal.h>
#include <glib-2.0/glib.h>
#include <dbus/dbus-glib-lowlevel.h>

namespace DCE
{
	class HAL;
	class Message;
}

using namespace DCE;

class PlutoHalD
{
	private:
	
		static std::map<unsigned int, int> templatesMap;
		static DCE::HAL * halDevice;
		static DBusError halError;
		static long pnpDeviceID;
		static GMainLoop * loop;
		static bool threadShutdown;
		static std::string moreInfo;
	
	public:
	
		PlutoHalD();
		
		/***/
		static void* startUp(void * device);
		
		/***/
		static void shutDown();
		
		/***/
		static void mainloop_integration (LibHalContext *ctx, DBusConnection * dbus_connection);
		
		/***/
		static void myDeviceAdded(LibHalContext * ctx, const char * udi);
		
		/***/
		static void myDeviceNewCapability(LibHalContext * ctx, const char * udi, const char *capability);
		
		/***/
		static void myDeviceRemoved(LibHalContext * ctx, const char * udi);
		
		/***/
		static void getPortIdentification(std::string portFromBus, std::string& portID);
		
		/***/
		static void initialize(LibHalContext * ctx);
		
		/***/
		static void getProductVendorId(
			LibHalContext * ctx, const char * udi,
			int * prodId, int * vendorId,
			int * subsysProdId, int * subsysVendorId,
			int * busType , std::string & sysfsPath);
		
		/***/
		static void getParentId(LibHalContext * ctx, const char * udi,
								const char * tag, const char * value,
								std::string & parentId);
		
		/***/
		static void getChildId(LibHalContext * ctx, const char * udi,
								const char * tag, const char * value,
								std::string & childId);
		
		static void getSerialParent(const char * sysfs, std::string & parentSysfs);
				
		/***/
		static gboolean timeoutHandler (DBusConnection *bus);
};

#endif /*PLUTOHALD_H_*/
