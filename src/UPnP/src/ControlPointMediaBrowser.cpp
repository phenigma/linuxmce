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
			Upnp_Discovery *d_event = (Upnp_Discovery *)event;
			IXML_Document *descDoc = NULL;
			int ret = UpnpDownloadXmlDoc(d_event->Location, &descDoc);
			if(ret != UPNP_E_SUCCESS)
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "unable to get the xml file from device");
			}
			else
			{
				g_pPlutoLogger->Write(LV_DEBUG, "got the XML document from device");
				g_pPlutoLogger->Write(LV_DEBUG, "friendly name %s", GetFirstDocumentItem(descDoc, "friendlyName"));
				
			}
			if(NULL != descDoc)
			{
				ixmlDocument_free(descDoc);
			}
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
