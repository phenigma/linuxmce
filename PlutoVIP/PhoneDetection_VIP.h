#ifndef PhoneDetection_VIP_h
#define PhoneDetection_VIP_h

#include "VIPShared/PhoneDetection_Bluetooth_Windows.h"
#include "VIPShared/PhoneDetection_Simulate.h"
#include "VIPShared/PhoneDevice.h"
#include "PlutoUtils/CommonIncludes.h"
#include "VIPShared/VR_IdentifyPhone.h"

#include "AppBar.h"
#include "Customer.h"

extern map<unsigned long, Customer*> m_mapCustomers;

class PhoneDetection_VIP : 
#ifdef BT_SOCKET	
	public PhoneDetection_Simulate
#else
	public PhoneDetection_Bluetooth_Windows
#endif
{
public:
	void AddCustomerImageToImageList(unsigned long Id, CString FileName);
	CString SaveCustomerImage(VR_IdentifyPhone* pIP);
	class pluto_pthread_mutex_t *m_pThreadMutex;
	pthread_mutexattr_t m_MutexAttr;
	
	//map<unsigned long, Customer*> m_mapCustomers;
	CAppBar* m_pNewAB;
	CAppBar* m_pActiveAB;
	CAppBar* m_pRecentAB;

	PhoneDetection_VIP();
	virtual ~PhoneDetection_VIP() { m_mapCustomers.clear(); }

	void AddCustomer(Customer *pCustomer);

	// The derived class must decide how to handle when new devices are detected or lost, or 
	// when the signal strength changes by more than 10 in either direction
	virtual void NewDeviceDetected(PhoneDevice *pDevice);
	virtual void LostDevice(PhoneDevice *pDevice);
	virtual void SignalStrengthChanged(PhoneDevice *pDevice);

	void SetAppBars(CAppBar* pNewAB,
					CAppBar* pActiveAB,
					CAppBar* pRecentAB);
	
	void SetupAppBars();
};

#endif
