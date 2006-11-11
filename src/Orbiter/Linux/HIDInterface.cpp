#include "DCE/Logger.h"
#include "../Orbiter.h"

#define HAVE_STDBOOL_H
#include <hid.h>
#include <usb.h>
#include <stdio.h>
#include <string.h>

bool ProcessBindRequest(Orbiter *pOrbiter,usb_dev_handle *handle,char *inPacket,char *write_packet);
bool ProcessHIDButton(Orbiter *pOrbiter,char *inPacket);

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

	for (bus = busses; bus; bus = bus->next) 
	{
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next) {

#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents %04x:%04x\n", dev->descriptor.idVendor, dev->descriptor.idProduct);
#endif
			if ( (dev->descriptor.idVendor==0x0c16) && (dev->descriptor.idProduct==0x0006) )  // The gyration remote
			{
				g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents device found!");

				usb_dev_handle * handle = usb_open(dev);

				int res = 0;
				res = usb_claim_interface(handle, 1);
				if (res<0)
				{ 
					g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents claim interface: %i\n", res);
					perror("error: ");
					return NULL;
				}

				char outPacket[4] = { 0x08, 0x40, 0x00, 0x00 };
				char inPacket[6];

				res = usb_control_msg(handle, 0x21, 9, 8+(0x03<<8) /*int value*/, 1 /* int index */, outPacket, 4, 250);

				//                                                      res = usb_interrupt_write(handle, 0x01, outPacket, 4, 250);
				if (res<0)
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents first usb_control_msg: %i\n", res);
					perror("err: ");
				}

				int cnt=0;
				while(1)
				{
					res = usb_interrupt_read(handle, 0x82, inPacket, 6, 250);
					if (res<0&&res!=-110) break;
					if (res<=0)
					{
#ifdef DEBUG
						if (cnt%100==0) 
							g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents .", cnt++);
#endif
						usleep(10000);
						cnt++;
					}
					else
					{
						g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents \n[READER] %04i.%03i: read bytes: %d", cnt/100, cnt%100,res);

						if( res==6 && inPacket[0]==8 )  // It's for us
						{
							if( inPacket[1]==0x20 )  // A bind request
							{
								char write_packet[5];
								ProcessBindRequest(pOrbiter,handle,inPacket,write_packet);
								int ctrl = usb_control_msg(handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
								if (ctrl<0)
								{
									g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents ProcessBindRequest  usb_control_msg %d\n",(int) ctrl);
									perror("error: ");
								}
							}
							else if( inPacket[1]==0x25 )  // A button
								ProcessHIDButton(pOrbiter,inPacket);
						}
					}

				}

				usb_release_interface(handle, 1);


				usb_close(handle);
			}
		}
	}

	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents Exiting");

	return 0;
}

bool ProcessBindRequest(Orbiter *pOrbiter,usb_dev_handle *handle,char *inPacket,char *write_packet)
{
	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents ProcessBindRequest got a bind request for %d %d %d %d",
		(int) inPacket[2],(int) inPacket[3],(int) inPacket[4],(int) inPacket[5]);

	write_packet[0]=8;
	write_packet[1]=0x20;
	write_packet[2]=0x02;  // The remote ID
	write_packet[3]=0;
	int ctrl = usb_control_msg(handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
	if (ctrl<0)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents ProcessBindRequest  usb_control_msg %d\n",(int) ctrl);
		perror("error: ");
	}
	return true;
}


bool ProcessHIDButton(Orbiter *pOrbiter,char *inPacket)
{
	static int iRemoteID=0;
	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents ProcessHIDButton for %d %d %d %d",
		(int) inPacket[2],(int) inPacket[3],(int) inPacket[4],(int) inPacket[5]);
	
	Orbiter::Event *pEvent = new Orbiter::Event;
	pEvent->type=Orbiter::Event::HID;

	pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false );
	return true;
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
