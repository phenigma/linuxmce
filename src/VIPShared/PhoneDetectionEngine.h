#ifndef PhoneDetectionEngine_h
#define PhoneDetectionEngine_h

#include "PlutoUtils/MultiThreadIncludes.h"
#include "Logger.h"

using namespace DCE;

class PhoneDetectionEngine
{
public:
	// This will always have the currently detected devices.  You must grab
	// the m_MapMutex before accessing it.  The Intern_ methods add and remove
	// to this map
	map<u_int64_t,class PhoneDevice *> m_mapPhoneDevice_Detected;
	map<u_int64_t,class PhoneDevice *> m_mapDevicesDetectedThisScan;

	bool m_bInScanLoop; // True if it's in the scanning loop
	bool m_bAbortScanLoop;  // True when we are in the process of Aborting the scan
	pthread_t m_ThreadID;
	pluto_pthread_mutex_t m_MapMutex,m_StartStopMutex;
	pthread_mutexattr_t m_MutexAttr;

	PhoneDetectionEngine() 
		: 	m_MapMutex("detect map mutex"),m_StartStopMutex("detect ss mutex")	{ 

		pthread_mutexattr_init( &m_MutexAttr );
		pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
		m_MapMutex.Init(&m_MutexAttr);
		m_StartStopMutex.Init(NULL);
		m_bInScanLoop=false;
		m_bAbortScanLoop=false;
		m_ThreadID=NULL;
	}
	virtual ~PhoneDetectionEngine() 
	{ 
		g_pPlutoLogger->Write(LV_STATUS,"Phone Detection Engine terminating");
		StopScanning(); 
	}

	PhoneDevice *m_mapPhoneDevice_Detected_Find(u_int64_t MacAddress)
	{
		PLUTO_SAFETY_LOCK(mm,m_MapMutex);
		map<u_int64_t,class PhoneDevice *>::iterator it = m_mapPhoneDevice_Detected.find(MacAddress);
		return it==m_mapPhoneDevice_Detected.end() ? NULL : (*it).second;
	}

	PhoneDevice *m_mapDevicesDetectedThisScan_Find(u_int64_t MacAddress)
	{
		PLUTO_SAFETY_LOCK(mm,m_MapMutex);
		map<u_int64_t,class PhoneDevice *>::iterator it = m_mapDevicesDetectedThisScan.find(MacAddress);
		return it == m_mapDevicesDetectedThisScan.end() ? NULL : (*it).second;
	}

	// StartScanning should spawn a new worker thread that starts scanning
	// and return immediately
	virtual void StartScanning();

	// StopScanning will kill the thread, and return only when it has safely
	// stopped the scan
	virtual void StopScanning();

	// RequestStopScanning will kill the thread, but will return immediately.
	// The calling class can later call StopScanning to be sure it has stopped
	virtual void RequestStopScanning() { m_bAbortScanLoop=true; };

	// This is the actual scan loop that gets called by the worker thread
	// The worker thread will set m_bInScanLoop to true, and keep calling it until
	// the thread returns false.  This loop will add new devices to the map, and call
	// Intern_ methods
	virtual bool ScanningLoop()=0;

	// The derived class must decide how to handle when new devices are detected or lost, or 
	// when the signal strength changes by more than 10 in either direction
	virtual void NewDeviceDetected(class PhoneDevice *pDevice)=0;
	virtual void LostDevice(class PhoneDevice *pDevice)=0;
	virtual void SignalStrengthChanged(class PhoneDevice *pDevice)=0;

	virtual void AddDeviceToDetectionList(class PhoneDevice *pDevice);
	virtual void RemoveDeviceFromDetectionList(class PhoneDevice *pDevice);

	//this method should not be overrided in derived classes
	virtual void DetectionLogic(); 
	
	// Since the scanning routine can be sensitive, rather than handling everything on the 
	// same thread, we'll just span a new thread to handle the events.  These Intern_
	// functions spawn a new thread which calls the event handlers above
protected:

	// This will also add the phone to the map
	virtual void Intern_NewDeviceDetected(class PhoneDevice *pDevice);

	// This will also remove the phone from the map
	virtual void Intern_LostDevice(class PhoneDevice *pDevice);
	virtual void Intern_SignalStrengthChanged(class PhoneDevice *pDevice);
};
#endif

