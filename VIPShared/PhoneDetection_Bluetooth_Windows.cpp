#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "PhoneDetection_Bluetooth_Windows.h"
#include "BD/PhoneDevice.h"
#include <math.h>

#define INQUIRY_TIMEDOUT 30 //seconds

using namespace DCE;

bool PhoneDetection_Bluetooth_Windows::ScanningLoop()
{
	if( m_bAbortScanLoop )
		return false;

	m_bWaitingForInquiry=true;
	m_mapDevicesDetectedThisScan.clear();
	if( !StartInquiry() )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to start inquiry");
		Sleep(1000); // Sleep a second before we try again
		m_bWaitingForInquiry=false;
		return true;  // This loop will get called again
	}
	g_pPlutoLogger->Write(LV_STATUS,"Inquiry started");
	pthread_mutex_lock(&m_InquiryMutex.mutex);

	timespec abstime;
	abstime.tv_sec = (long) (time(NULL) + INQUIRY_TIMEDOUT);
	abstime.tv_nsec = 0;
	if(ETIMEDOUT == pthread_cond_timedwait(&m_InquiryCond,&m_InquiryMutex.mutex, &abstime))
	{
		StopInquiry();
		g_pPlutoLogger->Write(LV_STATUS,"Inquiry stopped - timed out %d seconds", INQUIRY_TIMEDOUT);
	}

	pthread_mutex_unlock(&m_InquiryMutex.mutex);
	list<PhoneDevice *> listDevicesLost;

	// Make a list of all the devices that were lost this scan
	PLUTO_SAFETY_LOCK(mm, m_MapMutex);

	map<u_int64_t,class PhoneDevice *>::iterator itDevice;
	for(itDevice=m_mapPhoneDevice_Detected.begin();itDevice!=m_mapPhoneDevice_Detected.end();)
	{
		class PhoneDevice *pD = (*itDevice).second;
		map<u_int64_t,class PhoneDevice *>::iterator itDeviceNew = m_mapDevicesDetectedThisScan.find(pD->m_iMacAddress);
		if( itDeviceNew==m_mapDevicesDetectedThisScan.end() )
		{
			listDevicesLost.push_back( (*itDevice).second );
			m_mapPhoneDevice_Detected.erase(itDevice++);
		}
		else
			itDevice++;
	}

	mm.Release();

	list<PhoneDevice *>::iterator itLost;
	for(itLost = listDevicesLost.begin();itLost != listDevicesLost.end();++itLost)
	{
		Intern_LostDevice(*itLost);
		g_pPlutoLogger->Write(LV_STATUS, "Lost connection to %s device", (*itLost)->m_sID.c_str());
	}

	return true;
}

void PhoneDetection_Bluetooth_Windows::OnDeviceResponded(BD_ADDR bda,
										DEV_CLASS dev_class,
										BD_NAME bd_name,
										BOOL b_connected)
{
	char name[100];
	strcpy(name,(const char *)bd_name);

	u_int64_t iMacAddress=0;
	for(int i=0;i<6;++i)
	{
		u_int64_t power = (u_int64_t) pow(256., 5-i);
		int Digit = bda[i];
		iMacAddress += (power * Digit);
	}

	g_pPlutoLogger->Write(LV_STATUS,"Device %s responded.", bd_name);

	PhoneDevice *pDNew = new PhoneDevice(name,iMacAddress,255);
	PLUTO_SAFETY_LOCK(mm,m_MapMutex);
	m_mapDevicesDetectedThisScan[iMacAddress] = pDNew;

	PhoneDevice *pDExisting = m_mapPhoneDevice_Detected_Find(pDNew->m_iMacAddress);
	if( pDExisting && abs(pDExisting->m_iLinkQuality-pDNew->m_iLinkQuality)<10 )
	{
		// nothing to do
		delete pDNew;
		return;
	}
	mm.Release();

	if( !pDExisting )
		Intern_NewDeviceDetected(pDNew);
	else
		Intern_SignalStrengthChanged(pDNew);
}


void PhoneDetection_Bluetooth_Windows::OnInquiryComplete(BOOL /*success*/, short	/*num_responses*/)
{
	g_pPlutoLogger->Write(LV_STATUS,"Inquiry complete");
	pthread_cond_broadcast(&m_InquiryCond);
}

