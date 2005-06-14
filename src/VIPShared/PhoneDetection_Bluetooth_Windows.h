#ifndef PhoneDetection_Bluetooth_Windows_h
#define PhoneDetection_Bluetooth_Windows_h

#include "PhoneDetectionEngine.h"
#include "WinBluetooth/inc/BtIfDefinitions.h" 
#include "WinBluetooth/inc/BtIfClasses.h"

class PhoneDetection_Bluetooth_Windows : public PhoneDetectionEngine, public CBtIf
{
	bool m_bWaitingForInquiry;
	pluto_pthread_mutex_t m_InquiryMutex;
	pthread_cond_t m_InquiryCond;

public:

	PhoneDetection_Bluetooth_Windows() : m_InquiryMutex("inq mutex") { 
		m_bWaitingForInquiry=false; 
		pthread_cond_init(&m_InquiryCond, NULL);
		m_InquiryMutex.Init(NULL,&m_InquiryCond);
	}

	virtual ~PhoneDetection_Bluetooth_Windows();

	virtual bool ScanningLoop();

	virtual void RequestStopScanning();

    virtual void SuspendScanning();

	//from BtIf
	virtual void OnDeviceResponded(BD_ADDR	 bda,
								   DEV_CLASS dev_class,
								   BD_NAME	bd_name,
								   BOOL b_connected);
	//from BtIf
	virtual void OnInquiryComplete(BOOL success, short	num_responses);
};
#endif

