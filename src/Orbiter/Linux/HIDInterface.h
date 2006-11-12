#ifndef HIDInterface_h
#define HIDInterface_h

#include "../Orbiter.h"

void *ProcessHIDEvents(void *p);

class HIDInterface
{
private:
    usb_dev_handle *m_p_usb_dev_handle;
	bool m_bRunning;
	pluto_pthread_mutex_t m_HIDMutex;  // This will also protect the callback map
	int m_iRemoteID,m_iPK_Device_Remote;

public:
	HIDInterface(Orbiter *pOrbiter);
	m_HIDMutex{ m_pOrbiter=pOrbiter; m_p_usb_dev_handle=NULL; m_bRunning=false; }
	void ProcessHIDEvents();

	bool ProcessBindRequest(usb_dev_handle *handle,char *inPacket,char *write_packet);
	bool ProcessHIDButton(char *inPacket);
};

#endif // HIDInterface_h
