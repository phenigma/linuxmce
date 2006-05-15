#ifndef PLUTOHALD_H_
#define PLUTOHALD_H_
#include <dbus/dbus.h>
#include <stdio.h>
#include <hal/libhal.h>
#include <stdlib.h>
#include <string.h>
#include <map.h>
#include <glib-2.0/glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "Gen_Devices/Plug_And_PlayBase.h"

namespace DCE
{
	class Plug_And_Play;
}

using namespace DCE;

class PlutoHalD
{
private:
	static std::map<unsigned int, int> templatesMap;
	static DCE::Plug_And_Play *pnpdevice;
public:
	PlutoHalD();
	static void* startUp(void * pnpdevice);
	static bool sendMessage(Message * pMsg, string &returnValue);
	static bool sendMessage(std::string params, std::string &returnValue);
	static void mainloop_integration (LibHalContext *ctx, DBusConnection * dbus_connection);
	static void getPortIdentification(std::string portFromBus, std::string& portID);
	static void myDeviceAdded(LibHalContext * ctx, const char * udi);
	static void myDeviceNewCapability(LibHalContext * ctx, const char * udi, const char *capability);
	static void myDeviceRemoved(LibHalContext * ctx, const char * udi);
	static void initialize(LibHalContext * ctx);
};

#endif /*PLUTOHALD_H_*/
