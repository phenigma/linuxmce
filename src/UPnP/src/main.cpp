/**
* The entry point in the application
*/

#include "main.h"


#include "ControlPointMediaBrowser.h"
#include "DeviceMediaServer.h"

#include <string>

using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger = NULL;
}

int main()
{
	char *ipAddress = NULL;
	int port = 0;
	g_pPlutoLogger = new FileLogger("upnp.log");
	g_pPlutoLogger->Write(LV_DEBUG, "========start==========");
	UpnpClient_Handle handle = -1;
			
//starting the UPNP lib
	int err = UpnpInit(ipAddress, port);
	if(err != UPNP_E_SUCCESS)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "init failed %d", err);
		goto exit_point;
	}
	
	if(NULL == ipAddress)
	{
		ipAddress = UpnpGetServerIpAddress();
	}
	if(0 == port)
	{
		port = UpnpGetServerPort();
	}
	g_pPlutoLogger->Write(LV_DEBUG, "Upnp initialized with ip %s:%d", ipAddress, port);

	//register control point (aka client)
	err = UpnpRegisterClient(ControlPointMediaBrowser::MediaBrowserCallbackEventHandler,
							&handle, &handle);
	if(UPNP_E_SUCCESS != err)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "register failed %d", err);
		goto exit_point;
	}
		
	//try to find the media servers on the net
	err = UpnpSearchAsync(handle, 5, DeviceMediaServer::MediaServerDeviceType, NULL);
	if(UPNP_E_SUCCESS != err)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "search failed %d", err);
		goto exit_point;
	}
	
	UpnpUnRegisterClient(handle);
			
exit_point:
	UpnpFinish();
	g_pPlutoLogger->Write(LV_DEBUG, "Upnp closed");

	g_pPlutoLogger->Flush();
	delete(g_pPlutoLogger);
	return 0;
}
