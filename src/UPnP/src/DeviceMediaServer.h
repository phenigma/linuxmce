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
