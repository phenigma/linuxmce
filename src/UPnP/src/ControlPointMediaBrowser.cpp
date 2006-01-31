#include "ControlPointMediaBrowser.h"
#include "main.h"

using namespace DCE;

ControlPointMediaBrowser::ControlPointMediaBrowser()
{
}

ControlPointMediaBrowser::~ControlPointMediaBrowser()
{
}

int ControlPointMediaBrowser::MediaBrowserCallbackEventHandler(Upnp_EventType eventType, 
		void* event, 
		void *cookie)
{
	switch(eventType)
	{
		case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
		case UPNP_DISCOVERY_SEARCH_RESULT:
		{
			g_pPlutoLogger->Write(LV_DEBUG, "some discovery is on the way");
			break;
		}

		case UPNP_DISCOVERY_SEARCH_TIMEOUT:
			g_pPlutoLogger->Write(LV_DEBUG, "search timed out");
			break;
		
		case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
			g_pPlutoLogger->Write(LV_DEBUG, "discovery buh-bye");
			break;
		
		default:
			g_pPlutoLogger->Write(LV_DEBUG, "invalid event type");
			break;
	}
	return 0;
}
