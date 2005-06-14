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

	//let's wait for Orbiters to load
	//we don't want to detect a phone and send a message to orbiters if they are not registered yet
	//because we'll lost that message
	Sleep(5000);
	
	while(!pe->m_bAbortScanLoop)
    {
        if(!pe->m_bScanningSuspended)
        {
            if(pe->ScanningLoop())
	    	    pe->DetectionLogic();
            else
                break;
        }
        else
            Sleep(100); //TODO: use condwait + broadcast
    }

	pe->m_bInScanLoop=false;
	return NULL;
}

void *NewDeviceThread(void *p)
{
	EnginePlusDevice *ed = (EnginePlusDevice *)p;
	
	if( !ed->m_pEngine->m_bAbortScanLoop )
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

	if(!ed->m_pEngine->m_bAbortScanLoop)
		ed->m_pEngine->SignalStrengthChanged(ed->m_pDevice);

	delete ed->m_pDevice;
	ed->m_pDevice = NULL;

	delete ed;
	ed = NULL;

	return NULL;
}

void PhoneDetectionEngine::DetectionLogic()
{
	PLUTO_SAFETY_LOCK(mm,m_MapMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Devices detected last loop: %d", m_mapPhoneDevice_Detected.size());
	g_pPlutoLogger->Write(LV_STATUS, "Devices detected this loop: %d", m_mapDevicesDetectedThisScan.size());

	list<PhoneDevice *> listDevicesLost;
	map<u_int64_t,class PhoneDevice *>::iterator itDevice;
	for(itDevice=m_mapPhoneDevice_Detected.begin();itDevice!=m_mapPhoneDevice_Detected.end();)
	{
		PhoneDevice *pDeviceDetected = (*itDevice).second;
		PhoneDevice *pDeviceDetectedThisScan = m_mapDevicesDetectedThisScan_Find(pDeviceDetected->m_iMacAddress);

		if( !pDeviceDetectedThisScan && pDeviceDetected->m_bIsConnected) //not found this loop
		{
			 g_pPlutoLogger->Write(LV_STATUS,"It seems that device %s wasn't detected this loop, but it is connected.", pDeviceDetected->m_sID.c_str());
		}
		
		if( !pDeviceDetectedThisScan && !pDeviceDetected->m_bIsConnected) //not found this loop
		{
			PhoneDevice *pPhoneDevice = (*itDevice).second;
			g_pPlutoLogger->Write(LV_STATUS,"Deleting lost device from map: %s size: %d",pPhoneDevice->m_sID.c_str(),(int) m_mapPhoneDevice_Detected.size());

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
		g_pPlutoLogger->Write(LV_WARNING, "Lost connection to device: mac %s, id %s", 
			(*itLost)->m_sMacAddress.c_str(), (*itLost)->m_sID.c_str());
		Intern_LostDevice(*itLost);
	}

	//prepare for next scanning loop
	m_mapDevicesDetectedThisScan.clear();
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

	PhoneDevice *pDExisting = m_mapPhoneDevice_Detected_Find(pDevice->m_iMacAddress);
	
    if(pDExisting)
		delete pDExisting;

	m_mapPhoneDevice_Detected[pDevice->m_iMacAddress] = pDevice;

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

void PhoneDetectionEngine::RemoveDeviceFromDetectionList(u_int64_t iMacAddress)
{
	PLUTO_SAFETY_LOCK(mm,m_MapMutex);
	map<u_int64_t,class PhoneDevice *>::iterator itDevice = m_mapPhoneDevice_Detected.find(iMacAddress);

	if(itDevice != m_mapPhoneDevice_Detected.end())
	{
		g_pPlutoLogger->Write(LV_STATUS, "Need to remove device %d from the detection list", iMacAddress);
		m_mapPhoneDevice_Detected.erase(itDevice);
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING, "Cannot remove device %d from the detection list of %d", iMacAddress,(int) m_mapPhoneDevice_Detected.size());
		for(map<u_int64_t,class PhoneDevice *>::iterator itDevice = m_mapPhoneDevice_Detected.begin();itDevice != m_mapPhoneDevice_Detected.end();++itDevice)		{
			class PhoneDevice *pPhoneDevice = (*itDevice).second;
g_pPlutoLogger->Write(LV_WARNING, "Map contains Cannot remove device %s from the detection list", pPhoneDevice->m_sMacAddress.c_str());
		}
	}
}

void PhoneDetectionEngine::AddDeviceToDetectionList(class PhoneDevice *pDevice)
{
	if( m_mapIgnoreMacs.find(pDevice->m_iMacAddress)!=m_mapIgnoreMacs.end() )
	{
g_pPlutoLogger->Write(LV_WARNING,"Ignoring MAC: %s",pDevice->m_sMacAddress.c_str());
		delete pDevice;
		return;
	}

	PLUTO_SAFETY_LOCK(mm,m_MapMutex);
	m_mapDevicesDetectedThisScan[pDevice->m_iMacAddress] = pDevice;

	PhoneDevice *pDExisting = m_mapPhoneDevice_Detected_Find(pDevice->m_iMacAddress);

	bool bConnected = pDExisting && pDExisting->m_bIsConnected;
	
	g_pPlutoLogger->Write(LV_STATUS, "Analyzing device %s. Existing %p, connected %d ", pDevice->m_sMacAddress.c_str(),
			pDExisting, bConnected);
	
	
	//first time detected or not connected yet
	if( !pDExisting || !pDExisting->m_bIsConnected)
	{
		Intern_NewDeviceDetected(pDevice);
	}
	else 
	{
		g_pPlutoLogger->Write(LV_STATUS, "This is not a new device");
		if(pDExisting && pDExisting->m_bIsConnected) //let's get the link quality
		{
			int iLinkQuality = GetLinkQuality(pDevice->m_sMacAddress.c_str());
			
			if(iLinkQuality)
				pDevice->m_iLinkQuality = iLinkQuality;
		}

		//if this is not first time detected, but the link quality changed
		if(pDExisting && abs(pDExisting->m_iLinkQuality - pDevice->m_iLinkQuality) >= 10)
		{
			Intern_SignalStrengthChanged(pDevice);
		}
		else //an existing one and with the same link quality
		{
			delete pDevice;
		}
	}
}

/*virtual*/ void PhoneDetectionEngine::SuspendScanning()
{
    g_pPlutoLogger->Write(LV_WARNING, "Suspending scanning...");
    m_bScanningSuspended = true;
}

/*virtual*/ void PhoneDetectionEngine::ResumeScanning()
{
    g_pPlutoLogger->Write(LV_WARNING, "Resuming scanning...");
    m_bScanningSuspended = false;
}