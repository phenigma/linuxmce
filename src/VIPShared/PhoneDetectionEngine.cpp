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
	while( !pe->m_bAbortScanLoop && pe->ScanningLoop() );
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

void PhoneDetectionEngine::StartScanning()
{
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
g_pPlutoLotter->Write(LV_STATUS,"Adding device to map1: %s size is now: %d",pPhoneDevice->m_sID.c_str(),(int) m_mapPhoneDevice_Detected.size());

	pthread_t t;
	EnginePlusDevice *pED = new EnginePlusDevice(this,pDevice);
	pthread_create(&t, NULL, NewDeviceThread, (void*)pED);
}

void PhoneDetectionEngine::Intern_LostDevice(class PhoneDevice *pDevice)
{
	pthread_t t;
	EnginePlusDevice *pED = new EnginePlusDevice(this,pDevice);
	pthread_create(&t, NULL, LostDeviceThread, (void*)pED);
}

void PhoneDetectionEngine::Intern_SignalStrengthChanged(class PhoneDevice *pDevice)
{
	pthread_t t;
	EnginePlusDevice *pED = new EnginePlusDevice(this,pDevice);
	pthread_create(&t, NULL, SignalStrengthThread, (void*)pED);
}

