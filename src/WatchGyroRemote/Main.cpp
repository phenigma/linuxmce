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
#include "DCEConfig.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;
DCEConfig dceConfig;

#define HAVE_STDBOOL_H
extern "C" 
{
		#include <hid.h>
		#include <usb.h>
}

int main(int argc, char *argv[])
{
	LoggerWrapper::SetType(LT_LOGGER_FILE,"/var/log/pluto/WatchGyroRemote.log");

	usb_dev_handle *p_usb_dev_handle;
	struct usb_bus *busses;
	usb_set_debug(255);

	usb_init();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote starting");

	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();

	struct usb_bus *bus;
	//int c, i, a;

	/* ... */

	for (bus = busses; bus; bus = bus->next) 
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote bus %s",bus->dirname);
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next) {

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote %04x:%04x\n", dev->descriptor.idVendor, dev->descriptor.idProduct);
			if ( (dev->descriptor.idVendor==0x0c16) && (dev->descriptor.idProduct==0x0006) )  // The gyration remote
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote device found!");

				p_usb_dev_handle = usb_open(dev);

				int res = 0;
				res = usb_claim_interface(p_usb_dev_handle, 1);
				if (res<0)
				{ 
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WatchGyroRemote claim interface: %i\n", res);
					perror("error: ");
					break;
				}

				char inPacket[6];
				int cnt=0;
				while(true)
				{
					res = usb_interrupt_read(p_usb_dev_handle, 0x82, inPacket, 6, 250);
					if (res<0&&res!=-110) break;
					if (res<=0)
					{
						if (cnt%100==20) 
							LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote %d .", cnt++);
					}
					else
					{
						unsigned char *pPtr = (unsigned char *) inPacket;
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote [READER] %04i.%03i: read bytes: %d %x.%x.%x.%x.%x.%x", 
							cnt/100, cnt%100, res, (int) pPtr[0],(int) pPtr[1],(int) pPtr[2],(int) pPtr[3],(int) pPtr[4],(int) pPtr[5]);

						if( res==6 && inPacket[0]==8 )  // It's for us
						{
							if( inPacket[1]==0x25 )  // A button
							{
								LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote button %d %d %d %d",
									(int) inPacket[2],(int) inPacket[3],(int) inPacket[4],(int) inPacket[5]);
							}
						}
					}
				}

				LoggerWrapper::GetInstance()->Write(LV_STATUS,"WatchGyroRemote");
				usb_release_interface(p_usb_dev_handle, 1);
				usb_close(p_usb_dev_handle);
			}
		}
	}

	return 0;
}

