#include "DCE/Logger.h"
#include "../Orbiter.h"

#define HAVE_STDBOOL_H
#include <hid.h>
#include <usb.h>
#include <stdio.h>
#include <string.h>

void *ProcessHIDEvents(void *p)
{
	Orbiter *pOrbiter = (Orbiter *) p;
	struct usb_bus *busses;
	usb_set_debug(255);

	usb_init();
	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();

	struct usb_bus *bus;
	int c, i, a;

	/* ... */

	for (bus = busses; bus; bus = bus->next) {
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next) {

			g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents %04x:%04x\n", dev->descriptor.idVendor, dev->descriptor.idProduct);

			if ( (dev->descriptor.idVendor==0x0c16) && (dev->descriptor.idProduct==0x0006) )
			{
				g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents device found!");

				usb_dev_handle * handle = usb_open(dev);

				int res = 0;

				/*                                    res = usb_set_configuration(handle, 1);
				g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents set configuration: %i\n", res);
				if (res<0)
				return 1;
				*/                          /*
				res = usb_detach_kernel_driver_np(handle, 1);
				g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents detach interface: %i\n", res);
				if (res<0){ perror("error: "); /*return 1;*//*}
				*/
				res = usb_claim_interface(handle, 1);
				g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents claim interface: %i\n", res);
				if (res<0){ perror("error: ");
				return NULL;}

				char outPacket[4] = { 0x08, 0x40, 0x00, 0x00 };
				char inPacket[6];

				res = usb_control_msg(handle, 0x21, 9, 8+(0x03<<8) /*int value*/, 1 /* int index */, outPacket, 4, 250);
				//                                                      res = usb_interrupt_write(handle, 0x01, outPacket, 4, 250);
				if (res<0) perror("err: ");                             g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents write result: %i\n", res);
				//                                                      if (res<0) return 1;

				int cnt=0;
				while(1)
				{
					res = usb_interrupt_read(handle, 0x82, inPacket, 6, 250);
					//g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents read result: %i\n", res);
					if (res<0&&res!=-110) break;
					if (res<=0)
					{
						if (cnt%100==0) g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents .", cnt++);
						usleep(10000);
						cnt++;
					}
					else
					{
						g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents \n[READER] %04i.%03i: read bytes: ", cnt/100, cnt%100);
						int i;
						for (i=0; i<res; i++)
							g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents %02x ", (int)(unsigned char) inPacket[i]);
						g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents \n");

						if( inPacket[0]==8 && inPacket[1]==0x20 )
						{
							g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents ProcessHIDEvents got a bind request.  Donig it.\n");
							char write_packet[5];
							write_packet[0]=8;
							write_packet[1]=0x20;
							write_packet[2]=0x02;  // The remote ID
							write_packet[3]=0;
							int ctrl = usb_control_msg(handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
							g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents ProcessHIDEvents  usb_control_msg %d\n",(int) ctrl);
							if (ctrl<0) perror("error: ");
						}

						//                                                    break;
					}

				}

				usb_release_interface(handle, 1);


				usb_close(handle);
			}
		}
	}


	return 0;
}

/*
	Kirill's note:
	short note:
	usb_control_msg(handle, 0x21, 9, 8+(0x03<<8) , 1 , outPacket, 4, 250);
	you see here the combination:
	8+(0x03<<8) , 1

	this is from hid sources : reportID + (HID_RT_FEATURE<<8),   interfaceID
	seems, this comes directly from USB HID specs, so I decided to keep it
	its in sources of libhid , src/hid_exchange.c   - in function function hid_set_output_report
	just for reference
	yep, so now we know everything 
	the 0x21: USB_ENDPOINT_OUT + USB_TYPE_CLASS + USB_RECIP_INTERFACE
	09: HID_REPORT_SET
*/
