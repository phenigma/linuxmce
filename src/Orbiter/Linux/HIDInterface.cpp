#include "DCE/Logger.h"
#include "../Orbiter.h"
#include "HIDInterface.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include <stdio.h>
#include <string.h>

void *ProcessHIDEvents(void *p)
{
	PlutoHIDInterface *pPlutoHIDInterface = (PlutoHIDInterface *) p;
	pPlutoHIDInterface->ProcessHIDEvents();
	return NULL;
}

PlutoHIDInterface::PlutoHIDInterface(Orbiter *pOrbiter) : m_HIDMutex("HID")
{
	m_pOrbiter=pOrbiter; 
	m_p_usb_dev_handle=NULL; 
	m_bRunning=false;
	m_HIDMutex.Init(NULL);
	m_iRemoteID=m_iPK_Device_Remote=0;
	m_iHoldingDownButton=0;
}

void PlutoHIDInterface::ProcessHIDEvents()
{
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

				m_p_usb_dev_handle = usb_open(dev);

				int res = 0;
				res = usb_claim_interface(m_p_usb_dev_handle, 1);
				if (res<0)
				{ 
					g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents claim interface: %i\n", res);
					perror("error: ");
					return;
				}

				char outPacket[4] = { 0x08, 0x40, 0x00, 0x00 };
				char inPacket[6];

				res = usb_control_msg(m_p_usb_dev_handle, 0x21, 9, 8+(0x03<<8) /*int value*/, 1 /* int index */, outPacket, 4, 250);

				//                                                      res = usb_interrupt_write(m_p_usb_dev_handle, 0x01, outPacket, 4, 250);
				if (res<0)
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents first usb_control_msg: %i\n", res);
					perror("err: ");
				}

				int cnt=0;
				while(!m_pOrbiter->m_bQuit)
				{
					PLUTO_SAFETY_LOCK(hm,m_HIDMutex);
					m_bRunning=true;
					res = usb_interrupt_read(m_p_usb_dev_handle, 0x82, inPacket, 6, 250);
					if (res<0&&res!=-110) break;
					if (res<=0)
					{
#ifdef DEBUG
						if (cnt%100==0) 
							g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents .", cnt++);
#endif
						hm.Release();  
						usleep(10000);  // Give somebody else a chance to use this
						hm.Relock();
						cnt++;
					}
					else
					{
						g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents \n[READER] %04i.%03i: read bytes: %d", cnt/100, cnt%100,res);

						if( res==6 && inPacket[0]==8 )  // It's for us
						{
							if( inPacket[1]==0x20 )  // A bind request
								ProcessBindRequest(inPacket);
							else if( inPacket[1]==0x25 )  // A button
								ProcessHIDButton(inPacket);
						}
					}
				}

				m_bRunning=false;
				usb_release_interface(m_p_usb_dev_handle, 1);
				usb_close(m_p_usb_dev_handle);
			}
		}
	}

	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents Exiting");
}

bool PlutoHIDInterface::ProcessBindRequest(char *inPacket)
{
	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents ProcessBindRequest got a bind request for %d %d %d %d",
		(int) inPacket[2],(int) inPacket[3],(int) inPacket[4],(int) inPacket[5]);


	unsigned char *pSerialNumber = (unsigned char *) inPacket; // Unsigned so it's not negative numbers
	char sSerialNumber[30];
	sprintf(sSerialNumber,"%x.%x.%x.%x",(int) pSerialNumber[2],(int) pSerialNumber[3],(int) pSerialNumber[4],(int) pSerialNumber[5]);

	int PK_Device=0,RemoteID=m_mapSerialNumber_RemoteID_Find(sSerialNumber);
	if( RemoteID==0 )
	{
		DCE::CMD_Get_Remote_ID CMD_Get_Remote_ID(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_OrbiterPlugIn,sSerialNumber,&PK_Device,&RemoteID);
		if( !m_pOrbiter->SendCommand(CMD_Get_Remote_ID) || PK_Device==0 || RemoteID==0 )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"PlutoHIDInterface::ProcessBindRequest failed to set RemoteID");
			RemoteID=255; // A bogus number
		}
		m_mapRemoteID_Device[ RemoteID ] = PK_Device;
		m_mapSerialNumber_RemoteID[ sSerialNumber ] = RemoteID;
	}
	else
		PK_Device=m_mapRemoteID_Device_Find(RemoteID);

	char write_packet[5];
	write_packet[0]=8;
	write_packet[1]=0x20;
	write_packet[2]=(char) RemoteID;
	write_packet[3]=0;
	int ctrl = usb_control_msg(m_p_usb_dev_handle, 0x21, 0x9, 8+(0x03<<8) /*int value*/, 1 /* int index */, write_packet, 4, 250);
	if (ctrl<0)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents ProcessBindRequest  usb_control_msg %d\n",(int) ctrl);
		perror("error: ");
		return false;
	}
	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents ProcessBindRequest remote %d PK_Device %d",RemoteID,PK_Device);
	return true;
}


bool PlutoHIDInterface::ProcessHIDButton(char *inPacket)
{
	unsigned char *p_Packet = (unsigned char *) inPacket;

	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents ProcessHIDButton for %d %d %d %d",
		(int) p_Packet[2],(int) p_Packet[3],(int) p_Packet[4],(int) p_Packet[5]);

	int iRemoteID = p_Packet[2];
	if( iRemoteID!=m_iRemoteID )
	{
		m_iPK_Device_Remote = m_mapRemoteID_Device_Find(iRemoteID);
		if( !m_iPK_Device_Remote )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"PlutoHIDInterface::ProcessHIDButton Remote ID %d is unknown",iRemoteID);
			return false;
		}
		m_iRemoteID = iRemoteID;
		DCE::CMD_Set_Active_Remote CMD_Set_Active_Remote(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_OrbiterPlugIn,m_iPK_Device_Remote,m_pOrbiter->m_dwPK_Device);
		m_pOrbiter->SendCommand(CMD_Set_Active_Remote);
		g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents ProcessHIDButton new remote %d device %d",m_iRemoteID,m_iPK_Device_Remote );
	}
	
	// If p_Packet[3]==0 then this is notifying us that the button was released.  If it's not, it's notifying us that a button was pressed
	// if m_iHoldingDownButton is !0, then we're already reporting a button as being depressed, so we need to fire a button up if the 
	// user is pressing another button
	if( m_iHoldingDownButton )
	{
		Orbiter::Event *pEvent = new Orbiter::Event;
		pEvent->type=Orbiter::Event::BUTTON_UP;
		pEvent->data.button.m_iPK_Button = m_iHoldingDownButton + 100000;
		m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false );
		if( p_Packet[3]==0 )
		{
#ifdef DEBUG
			g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents ProcessHIDButton button %d was released",m_iHoldingDownButton);
#endif
			m_iHoldingDownButton=0;  // we're not holding down any buttons anymore
			return true;
		}
	}

	if( p_Packet[3]==0 )
	{
		g_pPlutoLogger->Write(LV_WARNING,"ProcessHIDEvents ProcessHIDButton got a button up with nothing down");
		return true; // Shouldn't get here because m_iHoldingDownButton should be non-null if we're getting a button up
	}

	m_iHoldingDownButton=p_Packet[3];
	Orbiter::Event *pEvent = new Orbiter::Event;
	pEvent->type=Orbiter::Event::BUTTON_DOWN;
	pEvent->data.button.m_iPK_Button = m_iHoldingDownButton + 100000; // We're using especially high numbers, > 100000 for these special buttons

	m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false );
	return true;
}


/*
	Kirill's note:
	short note:
	usb_control_msg(m_p_usb_dev_handle, 0x21, 9, 8+(0x03<<8) , 1 , outPacket, 4, 250);
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
