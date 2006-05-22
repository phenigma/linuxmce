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
	
	public:
	
		PlutoHalD();
		
		/***/
		static void* startUp(void * HAL);
		
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
};

#endif /*PLUTOHALD_H_*/
