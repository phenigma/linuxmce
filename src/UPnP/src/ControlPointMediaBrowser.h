#ifndef CONTROLPOINTMEDIABROWSER_H_
#define CONTROLPOINTMEDIABROWSER_H_

#include "main.h"

class ControlPointMediaBrowser
{
public:
	ControlPointMediaBrowser();
	virtual ~ControlPointMediaBrowser();

	static int MediaBrowserCallbackEventHandler(Upnp_EventType eventType, void* event, void *cookie);
};

#endif /*CONTROLPOINTMEDIABROWSER_H_*/
