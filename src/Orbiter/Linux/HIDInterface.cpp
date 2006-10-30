#include "DCE/Logger.h"
#include "../Orbiter.h"

#define HAVE_STDBOOL_H
#include <hid.h>
#include <stdio.h>
#include <string.h>

void *ProcessHIDEvents(void *p)
{
	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents starting");

	Orbiter *pOrbiter = (Orbiter *) p;
	HIDInterface* hid;
	hid_return ret;

	HIDInterfaceMatcher matcher = { 0x0c16, 0x0006, NULL, NULL, 0 };

	/* see include/debug.h for possible values */
	//hid_set_debug(HID_DEBUG_ALL);
	hid_set_debug(HID_DEBUG_NONE);
	hid_set_debug_stream(stderr);
	/* passed directly to libusb */
	hid_set_usb_debug(0);
	
	ret = hid_init();
	if (ret != HID_RET_SUCCESS)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents hid_init failed with return code %d", ret);
		return NULL;
	}

	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents initialized");

	hid = hid_new_HIDInterface();
	if (hid == 0)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents hid_new_HIDInterface() failed, out of memory?");
		return NULL;
	}

	ret = hid_force_open(hid, 0, &matcher, 3);
	if (ret != HID_RET_SUCCESS)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents hid_force_open failed with return code %d\n", ret);
		return NULL;
	}

	ret = hid_write_identification(stdout, hid);
	if (ret != HID_RET_SUCCESS) {
		g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents hid_write_identification failed with return code %d\n", ret);
		return NULL;
	}

	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents wrote identification");

	const unsigned char RECV_PACKET_LEN = 6;
	unsigned char packet[RECV_PACKET_LEN];
	do
	{
		ret = hid_interrupt_read(hid, 0x82, (char *) packet, RECV_PACKET_LEN, 250);
		int i;
		if (ret != HID_RET_SUCCESS && ret != HID_RET_FAIL_INT_READ)
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents hid_get_input_report failed with return code %d\n", ret);
		}
		else if (ret != HID_RET_FAIL_INT_READ)
		{
g_pPlutoLogger->Write(LV_WARNING,"ProcessHIDEvents hid_get_input_report ret %d\n", ret);

			Orbiter::Event *pEvent = new Orbiter::Event;
			pEvent->type=Orbiter::Event::HID;

			printf("HID packet: ");
			for (i = 0; i < RECV_PACKET_LEN; i++)
			{
				pEvent->data.hid.m_pbHid[i] = packet[i];
				printf("%02x ", packet[i]);
			}
			printf(".\n");

			if( packet[0]==8 && packet[1]==0x20 )
			{
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents got a bind request.  Donig it.");
#endif
				char write_packet[5];
				write_packet[0]=8;
				write_packet[1]=0x20;
				write_packet[2]=0x01;
				write_packet[3]=0;
				hid_return ret_write = hid_interrupt_write(hid, 0x80, (char *) write_packet, 4, 250);
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents hid_return was %d",(int) ret_write);
#endif
			}

#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents queueing to orbiter   rrr");
#endif
			pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false );
		}
	} while (!pOrbiter->m_bQuit);

	ret = hid_close(hid);
	if (ret != HID_RET_SUCCESS)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents hid_close failed with return code %d\n", ret);
		return NULL;
	}

	hid_delete_HIDInterface(&hid);

	ret = hid_cleanup();
	if (ret != HID_RET_SUCCESS)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents hid_cleanup failed with return code %d\n", ret);
		return NULL;
	}
	
	g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents exiting");

	return NULL;
}
