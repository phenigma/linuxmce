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
	/*
	map<int,int> m_mapRemoteID_Device; // Map of known remote id's to device id's
	map<string,int> m_mapSerialNumber_RemoteID; // Map of known serial number's to remote id's
	int m_mapRemoteID_Device_Find(int RemoteID)	{ map<int,int>::iterator it = m_mapRemoteID_Device.find(RemoteID); return it==m_mapRemoteID_Device.end() ? NULL : (*it).second; }
	int m_mapSerialNumber_RemoteID_Find(string sSerialNumber) { map<string,int>::iterator it = m_mapSerialNumber_RemoteID.find(sSerialNumber); return it==m_mapSerialNumber_RemoteID.end() ? NULL : (*it).second; }
	*/
	int m_iHoldingDownButton; // The button currently being held down
	Orbiter *m_pOrbiter;

public:
	PlutoHIDInterface(Orbiter *pOrbiter);
	void ProcessHIDEvents();

	bool ProcessBindRequest(char *inPacket);
	bool ProcessHIDButton(char *inPacket);
	bool SetActiveRemote(int iRemoteID,bool bFollowMe);
	bool StartMouse();
	bool StopMouse();
};

#endif // PlutoHIDInterface_h
