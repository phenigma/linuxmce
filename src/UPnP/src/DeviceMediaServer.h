#ifndef DEVICEMEDIASERVER_H_
#define DEVICEMEDIASERVER_H_

#include "Device.h"

class DeviceMediaServer : public Device
{
public:
	DeviceMediaServer();
	virtual ~DeviceMediaServer();
	static const char MediaServerDeviceType[];
};

#endif /*DEVICEMEDIASERVER_H_*/
