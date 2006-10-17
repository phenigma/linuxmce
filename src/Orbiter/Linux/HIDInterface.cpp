#include "DCE/Logger.h"
#include "../Orbiter.h"

#include <hid.h>
#include <stdio.h>
#include <string.h>

void *ProcessHIDEvents(void *p)
{
	Orbiter *pOrbiter = (Orbiter *) p;
	HIDInterface* hid;
	hid_return ret;

	HIDInterfaceMatcher matcher = { 0x0c16, 0x0002, NULL, NULL, 0 };

	/* see include/debug.h for possible values */
	//hid_set_debug(HID_DEBUG_ALL);
	hid_set_debug(0);
	hid_set_debug_stream(stderr);
	/* passed directly to libusb */
	hid_set_usb_debug(0);
	
	ret = hid_init();
	if (ret != HID_RET_SUCCESS)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents hid_init failed with return code %d", ret);
		return NULL;
	}

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

	const unsigned char RECV_PACKET_LEN = 6;
	unsigned char packet[RECV_PACKET_LEN];
	do
	{
		ret = hid_interrupt_read(hid, 0x81, packet, RECV_PACKET_LEN, 250);
		int i;
		if (ret != HID_RET_SUCCESS && ret != HID_RET_FAIL_INT_READ)
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"ProcessHIDEvents hid_get_input_report failed with return code %d\n", ret);
		}
		else if (ret != HID_RET_FAIL_INT_READ)
		{
g_pPlutoLogger->Write(LV_WARNING,"ProcessHIDEvents hid_get_input_report ret %d\n", ret);
			for (i = 0; i < RECV_PACKET_LEN; i++)
			{
g_pPlutoLogger->Write(LV_WARNING,"ProcessHIDEvents %02x ", (unsigned char) packet[i);
				Orbiter::Event *pEvent = new Orbiter::Event;

				pEvent->type=Orbiter::Event::HID;
				pEvent->data.hid.m_iHid = packet[i);
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"ProcessHIDEvents queueing to orbiter   rrr");
#endif
				pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false );
			}
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
}
