/*
  PhoneDetectionEngine
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include <iostream>
#include <fstream>
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "PhoneDetectionEngine.h"
#include "BD/PhoneDevice.h"

class EnginePlusDevice
{
public:
	PhoneDetectionEngine *m_pEngine;
	PhoneDevice *m_pDevice;
	EnginePlusDevice(PhoneDetectionEngine *pEngine,PhoneDevice *pDevice)
	{
		m_pEngine=pEngine;
		m_pDevice=pDevice;
	}
};

void *HandleDetectionThread(void *p)
{
	PhoneDetectionEngine *pe = (PhoneDetectionEngine *)p;
	pe->m_bInScanLoop=true;
	
	while( !pe->m_bAbortScanLoop && pe->ScanningLoop() )
		pe->DetectionLogic();

	pe->m_bInScanLoop=false;
	return NULL;
}

void *NewDeviceThread(void *p)
{
	EnginePlusDevice *ed = (EnginePlusDevice *)p;
	ed->m_pEngine->NewDeviceDetected(ed->m_pDevice);
	delete ed;
	return NULL;
}

void *LostDeviceThread(void *p)
{
	EnginePlusDevice *ed = (EnginePlusDevice *)p;

	if(!ed->m_pEngine->m_bAbortScanLoop)
		ed->m_pEngine->LostDevice(ed->m_pDevice);

	delete ed->m_pDevice;
	ed->m_pDevice = NULL;

	delete ed;
	ed = NULL;

	return NULL;
}

void *SignalStrengthThread(void *p)
{
	EnginePlusDevice *ed = (EnginePlusDevice *)p;
	ed->m_pEngine->SignalStrengthChanged(ed->m_pDevice);

	delete ed->m_pDevice;
	ed->m_pDevice = NULL;

	delete ed;
	ed = NULL;

	return NULL;
}

void PhoneDetectionEngine::DetectionLogic()
{
	//TODO: put here the logic of detection; use:
	//map<u_int64_t,class PhoneDevice *> m_mapPhoneDevice_Detected;
	//map<u_int64_t,class PhoneDevice *> m_mapDevicesDetectedThisScan;

	g_pPlutoLogger->Write(LV_STATUS, "Devices detected so far: %d", m_mapPhoneDevice_Detected.size());
	g_pPlutoLogger->Write(LV_STATUS, "Devices detected this loop: %d", m_mapDevicesDetectedThisScan.size());

	//new devices
	map<u_int64_t,class PhoneDevice *>::iterator it;
	for(it = m_mapDevicesDetectedThisScan.begin(); it != m_mapDevicesDetectedThisScan.end(); it++)
	{
		PhoneDevice *pPhone = (*it).second;

		//g_pPlutoLogger->Write(LV_STATUS, "Analyzing phone detected this scan: mac %s, %s",
		//	pPhone->m_sIPAddress.c_str(), pPhone->m_sID.c_str());

		PhoneDevice *pDExisting = m_mapPhoneDevice_Detected_Find(pPhone->m_iMacAddress);
		if( pDExisting && abs(pDExisting->m_iLinkQuality - pPhone->m_iLinkQuality) < 10 )
		{
			// nothing to do
			delete pPhone;
			continue;
		}

		if( !pDExisting )
		{
			//g_pPlutoLogger->Write(LV_WARNING, "New device detected: mac %s, %s",
			//	pPhone->m_sMacAddress.c_str(), pPhone->m_sID.c_str());

			Intern_NewDeviceDetected(pPhone);
		}
		else
		{
			//g_pPlutoLogger->Write(LV_WARNING, "Signal strength changed for device: mac %s, %s, signal strength is now %d",
			//	pPhone->m_sMacAddress.c_str(), pPhone->m_sID.c_str(), pPhone->m_iLinkQuality);

			Intern_SignalStrengthChanged(pPhone);
		}
	}

	//list devices?
	list<PhoneDevice *> listDevicesLost;
	map<u_int64_t,class PhoneDevice *>::iterator itDevice;
	for(itDevice=m_mapPhoneDevice_Detected.begin();itDevice!=m_mapPhoneDevice_Detected.end();)
	{
		PhoneDevice *pDeviceDetected = (*itDevice).second;

		//g_pPlutoLogger->Write(LV_STATUS, "Analyzing phone detected last scan: mac %s, %s",
		//	pDeviceDetected->m_sIPAddress.c_str(), pDeviceDetected->m_sID.c_str());

		PhoneDevice *pDeviceDetectedThisScan = m_mapDevicesDetectedThisScan_Find(pDeviceDetected->m_iMacAddress);
		if( !pDeviceDetectedThisScan ) //not found this loop
		{
			PhoneDevice *pPhoneDevice = (*itDevice).second;
			g_pPlutoLogger->Write(LV_STATUS,"Deleting lost device from map: %s size: %d",pPhoneDevice->m_sID.c_str(),(int) m_mapPhoneDevice_Detected.size());

			listDevicesLost.push_back( (*itDevice).second );
			m_mapPhoneDevice_Detected.erase(itDevice++);
		}
		else
			itDevice++;
	}

	list<PhoneDevice *>::iterator itLost;
	for(itLost = listDevicesLost.begin();itLost != listDevicesLost.end();++itLost)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Lost connection to device: mac %s, id %s", 
			(*itLost)->m_sMacAddress.c_str(), (*itLost)->m_sID.c_str());
		Intern_LostDevice(*itLost);
	}
}

void PhoneDetectionEngine::StartScanning()
{
g_pPlutoLogger->Write(LV_STATUS,"Start scanning size m_mapPhoneDevice_Detected size: %d",(int) m_mapPhoneDevice_Detected.size());
	m_bAbortScanLoop=false;
	pthread_create(&m_ThreadID, NULL, HandleDetectionThread, (void*)this);
}

void PhoneDetectionEngine::StopScanning()
{
	g_pPlutoLogger->Write(LV_STATUS, "Stopping scan loop...");
	m_bAbortScanLoop=true;
	RequestStopScanning();

	Sleep(100); // Just to be sure we are in the body of the scan loop

	while( m_bInScanLoop )
		Sleep(100);  // Just wait until the thread exits
	g_pPlutoLogger->Write(LV_STATUS, "Scan loop ended");
}


void PhoneDetectionEngine::Intern_NewDeviceDetected(class PhoneDevice *pDevice)
{
	PLUTO_SAFETY_LOCK(mm,m_MapMutex);
	m_mapPhoneDevice_Detected[pDevice->m_iMacAddress]=pDevice;
g_pPlutoLogger->Write(LV_STATUS,"Adding device to map1: %s m_mapPhoneDevice_Detected size is now: %d",pDevice->m_sID.c_str(),(int) m_mapPhoneDevice_Detected.size());

	pthread_t t;
	EnginePlusDevice *pED = new EnginePlusDevice(this,pDevice);
	pthread_create(&t, NULL, NewDeviceThread, (void*)pED);
}

void PhoneDetectionEngine::Intern_LostDevice(class PhoneDevice *pDevice)
{
g_pPlutoLogger->Write(LV_STATUS,"lost dev 1 m_mapPhoneDevice_Detected size: %d",(int) m_mapPhoneDevice_Detected.size());
	pthread_t t;
	EnginePlusDevice *pED = new EnginePlusDevice(this,pDevice);
	pthread_create(&t, NULL, LostDeviceThread, (void*)pED);
g_pPlutoLogger->Write(LV_STATUS,"lost dev 2 m_mapPhoneDevice_Detected size: %d",(int) m_mapPhoneDevice_Detected.size());
}

void PhoneDetectionEngine::Intern_SignalStrengthChanged(class PhoneDevice *pDevice)
{
	PLUTO_SAFETY_LOCK(mm,m_MapMutex);
	m_mapPhoneDevice_Detected[pDevice->m_iMacAddress]->m_iLinkQuality = pDevice->m_iLinkQuality;

g_pPlutoLogger->Write(LV_STATUS,"ssc1 m_mapPhoneDevice_Detected size: %d",(int) m_mapPhoneDevice_Detected.size());
	pthread_t t;
	EnginePlusDevice *pED = new EnginePlusDevice(this,pDevice);
	pthread_create(&t, NULL, SignalStrengthThread, (void*)pED);
g_pPlutoLogger->Write(LV_STATUS,"ssc 2 m_mapPhoneDevice_Detected size: %d",(int) m_mapPhoneDevice_Detected.size());
}

