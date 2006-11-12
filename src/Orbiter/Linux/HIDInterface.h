#ifndef PlutoHIDInterface_h
#define PlutoHIDInterface_h

#include "../Orbiter.h"
#define HAVE_STDBOOL_H
extern "C" 
{
		#include <hid.h>
		#include <usb.h>
}

void *ProcessHIDEvents(void *p);

class PlutoHIDInterface
{
private:
    usb_dev_handle *m_p_usb_dev_handle;
	bool m_bRunning;
	pluto_pthread_mutex_t m_HIDMutex;  // This will also protect the callback map
	int m_iRemoteID,m_iPK_Device_Remote;
	Orbiter *m_pOrbiter;

public:
	PlutoHIDInterface(Orbiter *pOrbiter);
	void ProcessHIDEvents();

	bool ProcessBindRequest(char *inPacket,char *write_packet);
	bool ProcessHIDButton(char *inPacket);
	int GetDeviceForRemoteID();
};

#endif // PlutoHIDInterface_h
