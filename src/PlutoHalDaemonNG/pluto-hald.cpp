#include <dbus/dbus.h>
#include <stdio.h>
#include <hal/libhal.h>
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <dbus/dbus-glib-lowlevel.h>


void mainloop_integration (LibHalContext *ctx, DBusConnection * dbus_connection) {
	dbus_connection_setup_with_g_main (dbus_connection, NULL);
}

void myDeviceAdded(LibHalContext * ctx, const char * udi) {
}

void myDeviceNewCapability(LibHalContext * ctx, const char * udi, const char *capability) {
	gchar *plutoDeviceTemplate = hal_device_get_property_string (ctx, udi, "pluto.deviceTemplate");

	printf("New device with template %s found", plutoDeviceTemplate);
}

void myDeviceRemoved(LibHalContext * ctx, const char * udi) {
}

void initialize(LibHalContext * ctx) {
	int 	  num_devices = 0;
	char 	**devices = hal_get_all_devices (ctx, &num_devices);
	gchar 	 *bus = NULL;

}

int main(int argc, char* argv[]) {
	LibHalFunctions  funcs;
	LibHalContext 	*ctx = NULL;
	GMainLoop 	*loop = NULL;
	
	FILE * lockFile = NULL;

	
	loop = g_main_loop_new (NULL, FALSE);
	
	funcs.main_loop_integration 	= mainloop_integration;
	funcs.device_added 		= myDeviceAdded;
	funcs.device_removed 		= myDeviceRemoved;
	funcs.device_new_capability 	= myDeviceNewCapability;
	funcs.device_lost_capability 	= NULL;
	funcs.device_property_modified 	= NULL;
	funcs.device_condition 		= NULL;
	
	ctx = hal_initialize(&funcs, 0);
	if ( ctx == NULL ) 
		return 1;
	}
	initialize(ctx);
	
	hal_device_property_watch_all(ctx);

	g_main_loop_run(loop);
	
	hal_shutdown(ctx);
	
	return 0;
}
