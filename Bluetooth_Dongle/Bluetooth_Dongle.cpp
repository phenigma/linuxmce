//<-dceag-d-b->
#include "Bluetooth_Dongle.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_DeviceCategory.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../VIPShared/PhoneDevice.h"
#include "../DCE/DeviceData_Base.h"

#include "../Orbiter/Orbiter.h"
#include "../Orbiter/SDL_Bluetooth/StartOrbiterSDLBluetooth.h"
#include "../VIPShared/BD_CP_SendMeKeystrokes.h"

#include "../VIPShared/BDCommandProcessor_BluetoothDongle.h"
#include "BD/BDCommandProcessor.h"

/*
#ifdef BT_SOCKET

#else
	#ifndef WIN32
	#else
#include "BD/BDCommandProcessor.h"
	#endif
#endif
*/
//-----------------------------------------------------------------------------------------------------

#ifdef WIN32
#include "CRTDBG.h"
#endif

//-----------------------------------------------------------------------------------------------------
void *HandleBDCommandProcessorThread(void *p)
{
// TODO -- need a mutex to protect this, right
	BD_Orbiter *pBD_Orbiter = (BD_Orbiter *) p;
	if( !pBD_Orbiter || !pBD_Orbiter->m_pBDCommandProcessor || pBD_Orbiter->m_pBDCommandProcessor->m_bDead )
	{
		if( pBD_Orbiter && pBD_Orbiter->m_pBDCommandProcessor )
		{
			delete pBD_Orbiter->m_pBDCommandProcessor;
			pBD_Orbiter->m_pBDCommandProcessor=NULL;
		}
		g_pPlutoLogger->Write(LV_CRITICAL,"Command processor is invalid.  Aborting");
		return NULL;
	}

	BDCommand *pCommand = new BD_CP_SendMeKeystrokes(1);
	pBD_Orbiter->m_pBDCommandProcessor->AddCommand(pCommand);
	while( pBD_Orbiter->m_pBDCommandProcessor->ReceiveCommand(0,0,NULL) && !pBD_Orbiter->m_pBDCommandProcessor->m_bDead );

	g_pPlutoLogger->Write(LV_STATUS,"Exiting command processor: m_bDead: %d",(int) pBD_Orbiter->m_pBDCommandProcessor->m_bDead);
	delete pBD_Orbiter->m_pBDCommandProcessor;
	pBD_Orbiter->m_pBDCommandProcessor=NULL;
	return NULL;
}
//-----------------------------------------------------------------------------------------------------
//<-dceag-const-b->! custom
Bluetooth_Dongle::Bluetooth_Dongle(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: 
	Bluetooth_Dongle_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter), 
	m_BTMutex("Bluetooth socket mutex")
//<-dceag-const-e->
{
	m_Channel = 10;
	m_BTMutex.Init(NULL);

	Map_DeviceData_Base::iterator it;
	for(it=GetData()->m_AllDevices.m_mapDeviceData_Base.begin();
		it!=GetData()->m_AllDevices.m_mapDeviceData_Base.end();
		++it )
	{
		DeviceData_Base *pDeviceData_Base = (*it).second;
		if( pDeviceData_Base->WithinCategory(DEVICECATEGORY_Mobile_Orbiter_CONST) )
		{
			if( pDeviceData_Base->m_sMacAddress.length() )
			{
				m_OrbiterSockets[pDeviceData_Base->m_sMacAddress] = new BD_Orbiter;

				// We're definitely not connected to this right now.
				GetEvents()->Mobile_orbiter_lost( pDeviceData_Base->m_sMacAddress );
				g_pPlutoLogger->Write(LV_STATUS,"Accepting bluetooth connections from %s", pDeviceData_Base->m_sMacAddress.c_str());
			}
			else
			{
				g_pPlutoLogger->Write(LV_WARNING,"Mobile orbiter: %d found, with no mac address",pDeviceData_Base->m_iPK_Device);
			}
		}
	}
	StartScanning();
}
//-----------------------------------------------------------------------------------------------------
//<-dceag-dest-b->
Bluetooth_Dongle::~Bluetooth_Dongle()
//<-dceag-dest-e->
{
}
//-----------------------------------------------------------------------------------------------------
//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Bluetooth_Dongle::Register()
//<-dceag-reg-e->
{
	return Connect(); 
}
//-----------------------------------------------------------------------------------------------------
// This is the actual scan loop that gets called by the worker thread
// The worker thread will set m_bInScanLoop to true, and keep calling it until
// the thread returns false.  This loop will add new devices to the map, and call
// Intern_ methods
bool Bluetooth_Dongle::ScanningLoop()
{
	// First, scan the map for connections that have died.  
	map<string, BD_Orbiter *>::iterator iMos;
	{
		PLUTO_SAFETY_LOCK(bm, m_BTMutex);
		
		for(iMos = m_OrbiterSockets.begin(); 
			iMos != m_OrbiterSockets.end(); ++iMos)
		{
			BD_Orbiter *p_BD_Orbiter = (*iMos).second;

			if (NULL != p_BD_Orbiter->m_pBDCommandProcessor && p_BD_Orbiter->m_pBDCommandProcessor->m_bDead)
			{
				GetEvents()->Mobile_orbiter_lost((*iMos).first.c_str());
				BDCommandProcessor *pSock = p_BD_Orbiter->m_pBDCommandProcessor;
				p_BD_Orbiter->m_pBDCommandProcessor = NULL;
				pSock->m_bExit = true;  // Cause the scan loop to terminate
			}		
		}
	}

#ifdef BT_SOCKET
	return PhoneDetection_Simulate::ScanningLoop();
#else
	#ifdef WIN32
		return PhoneDetection_Bluetooth_Windows::ScanningLoop();
	#else
		return PhoneDetection_Bluetooth_Linux::ScanningLoop();
	#endif
#endif	
}
//-----------------------------------------------------------------------------------------------------

// The derived class must decide how to handle when new devices are detected or lost, or 
// when the signal strength changes by more than 10 in either direction

void Bluetooth_Dongle::NewDeviceDetected(class PhoneDevice *pDevice)
{
	// We'll just handle this the same way
	SignalStrengthChanged(pDevice);
}
//-----------------------------------------------------------------------------------------------------
void Bluetooth_Dongle::LostDevice(class PhoneDevice *pDevice)
{
	GetEvents()->Mobile_orbiter_lost(pDevice->m_sMacAddress.c_str());
}
//-----------------------------------------------------------------------------------------------------
void Bluetooth_Dongle::SignalStrengthChanged(class PhoneDevice *pDevice)
{
 	if (!pDevice->m_bIsConnected)
	{
		//printf("Detection event, link quality: %d", pDevice->m_iLinkQuality);

		GetEvents()->Mobile_orbiter_detected(pDevice->m_sMacAddress, pDevice->m_iLinkQuality, pDevice->m_sID);
	}	
}
//-----------------------------------------------------------------------------------------------------
void Bluetooth_Dongle::CheckConnection(class PhoneDevice *pDevice)
{
	BD_Orbiter *pBD_Orbiter = m_OrbiterSockets_Find(pDevice->m_sMacAddress);

	if(
		NULL != pBD_Orbiter						   && 
		(
			(
				NULL != pBD_Orbiter->m_pBDCommandProcessor &&
				pBD_Orbiter->m_pBDCommandProcessor->m_bDead
			) 
			||
			NULL == pBD_Orbiter->m_pBDCommandProcessor
		)
	)
	{
		g_pPlutoLogger->Write(LV_WARNING, "No connection with app on %s device!", pDevice->m_sID.c_str());
	
		//GetEvents()->Mobile_orbiter_lost(pDevice->m_sMacAddress.c_str());

		pDevice->m_iLinkQuality = 0;
	}
}
//-----------------------------------------------------------------------------------------------------
/*

	COMMANDS TO IMPLEMENT

*/
//-----------------------------------------------------------------------------------------------------
//<-dceag-c60-b->
/* 
	COMMAND: #60 - Link with mobile orbiter
	COMMENTS: After the dongle has detected the device, then the orbiter plugin will send this command to tell the dongle to link to the phone.
	PARAMETERS:
		#42 MediaPosition
			On = 1 (link to the orbiter or link to the phone); Off = 0 (unlink)
		#47 Mac address
			The mac address of the phone
*/
void Bluetooth_Dongle::CMD_Link_with_mobile_orbiter(int iMediaPosition,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c60-e->
{
	cout << "Command #60 - Link with mobile orbiter" << endl;
	cout << "Parm #42 - MediaPosition=" << iMediaPosition << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;

	//printf("Got link with MO log: %p\n\n",g_pPlutoLogger);
	//printf("on_off: %d\nID: %s\n", iMediaPosition, sMac_address.c_str());

	{
		PLUTO_SAFETY_LOCK(bm,m_BTMutex);
		map<string, BD_Orbiter *>::iterator iMos = m_OrbiterSockets.find(sMac_address);
		if (iMos == m_OrbiterSockets.end())
		{
			 //#define NOSQL
			#ifdef NOSQL
				// hack it in so it works without the database
				m_OrbiterSockets[sMac_address] = new BD_Orbiter;
				iMos = m_OrbiterSockets.find(sMac_address);
			#else
				g_pPlutoLogger->Write(LV_CRITICAL, "Got link with MO for nonexistent ID %s!", sMac_address.c_str());
				return;
			#endif
		}

		BD_Orbiter *pBD_Orbiter = (*iMos).second;

		if (iMediaPosition == 0)
		{
			if (pBD_Orbiter->m_pBDCommandProcessor == NULL)
			{
				g_pPlutoLogger->Write(LV_STATUS, "Got disconnect for unlinked MO %s!", sMac_address.c_str());
			}
			else
			{
				pBD_Orbiter->m_pBDCommandProcessor = NULL;
				bm.Release();
				pBD_Orbiter->m_pBDCommandProcessor->m_bExit = true;  // Cause the scan loop to terminate
			}
			return;
		}

		if (pBD_Orbiter->m_pBDCommandProcessor != NULL)
		{
			if(!pBD_Orbiter->m_pBDCommandProcessor->m_bDead)
			{
				g_pPlutoLogger->Write(LV_STATUS, "Try to reconnect to MO %s. The connection with the app seems to be dead.", sMac_address.c_str());

				delete pBD_Orbiter->m_pBDCommandProcessor;
				pBD_Orbiter->m_pBDCommandProcessor = NULL;
			}
			else
			{
				g_pPlutoLogger->Write(LV_STATUS, "Got link with MO %s that's already linked!", sMac_address.c_str());
				return;
			}
		}
	}
	GetEvents()->Mobile_orbiter_linked(sMac_address, "version"); // TODO: Chris add this

	// Do this just to get the id
	PhoneDevice pdTemp("",sMac_address,0);
	PhoneDevice *pD = m_mapPhoneDevice_Detected_Find(pdTemp.m_iMacAddress);
	if( !pD )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find device anymore to link to");
		return;
	}


#ifdef BT_SOCKET
	// We must be using the DetectionSimulator
	// We'll borrow the establishment socket
	EstablishmentSocket *pSocket = new EstablishmentSocket(1,"localhost:3461","foo");
	if( !pSocket->Connect() )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot connect to phone on Socket ServerAddress");
		return;
	}

	BDCommandProcessor *pProcessor = 
		new BDCommandProcessor_BluetoothDongle(this, sMac_address, pSocket);
#else
	//printf("create cmd proc: %p\n\n",g_pPlutoLogger);
	BDCommandProcessor *pProcessor = 
		new BDCommandProcessor_BluetoothDongle(this, sMac_address, pD);
#endif

	BD_Orbiter *pBD_Orbiter = m_OrbiterSockets_Find(sMac_address);
	pBD_Orbiter->m_pBDCommandProcessor = pProcessor;
	
	if(NULL != pProcessor && !pProcessor->m_bDead)
        pthread_create(&pProcessor->m_BDSockThreadID, NULL, HandleBDCommandProcessorThread, (void*)pBD_Orbiter);
}
//-----------------------------------------------------------------------------------------------------
//<-dceag-c61-b->
/* 
	COMMAND: #61 - Get Signal Strength
	COMMENTS: Returns the actual signal strength of the phone for this dongle.
	PARAMETERS:
		#47 Mac address
			The mac address of the phone
		#48 Value (int)
			The strength of the connection
(255 is the maximum strength)
*/
void Bluetooth_Dongle::CMD_Get_Signal_Strength(string sMac_address,int *iValue_int,string &sCMD_Result,Message *pMessage)
//<-dceag-c61-e->
{
	cout << "Need to implement command #61 - Get Signal Strength" << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;
	cout << "Parm #48 - Value_int_=" << iValue_int << endl;

	//BDCommandProcessor *pBDCommandProcessor = m_OrbiterSockets_Find(sMac_address);

	//TO BE IMPLEMENTED!!
	*iValue_int = 255;
} 
//-----------------------------------------------------------------------------------------------------
//<-dceag-c62-b->
/* 
	COMMAND: #62 - Create Mobile Orbiter
	COMMENTS: The bluetooth dongle spawns an internal mobile orbiter which communicates with the phone.
	PARAMETERS:
		#2 PK_Device
			The ID of the controller to spawn.
		#47 Mac address
			The Mac Address of the phone.
*/
void Bluetooth_Dongle::CMD_Create_Mobile_Orbiter(int iPK_Device,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c62-e->
{
	// Lookup the mobile orbiter entry
	BD_Orbiter *pBD_Orbiter = m_OrbiterSockets_Find(sMac_address);

	if(
		NULL != pBD_Orbiter->m_pBDCommandProcessor  &&
		!pBD_Orbiter->m_pBDCommandProcessor->m_bDead
	)
	{
		if(NULL != pBD_Orbiter->m_pOrbiter)
		{
			delete pBD_Orbiter->m_pOrbiter;
			pBD_Orbiter->m_pOrbiter = NULL;
		}

		if(NULL == pBD_Orbiter->m_pOrbiter)
		{
			class OrbiterSDLBluetooth *pOrbiter = 
				StartOrbiterSDLBluetooth(
					pBD_Orbiter->m_pBDCommandProcessor,
					iPK_Device, 
					m_IPAddress, 
					"", 
					false,
					176,
					300
				);

			pBD_Orbiter->m_pOrbiter = (Orbiter *)pOrbiter;
		}
	}
}
//-----------------------------------------------------------------------------------------------------


//<-dceag-sample-b->! no sample
//<-dceag-c80-b->
/* 
	COMMAND: #80 - Send File To Device
	COMMENTS: Send a file to a Bluetooth device.
	PARAMETERS:
		#13 Filename
			The file to send
		#47 Mac address
			The MAC Address
		#58 IP Address
			The IP Address
*/
void Bluetooth_Dongle::CMD_Send_File_To_Device(string sFilename,string sMac_address,string sIP_Address,string &sCMD_Result,Message *pMessage)
//<-dceag-c80-e->
{
	g_pPlutoLogger->Write(LV_STATUS,"Send File: %s to %s",sFilename.c_str(),sMac_address.c_str());
	
#ifndef _WIN32
	sleep(10);
	
	string ObexPushScriptPath = "/pluto/bin/";//TODO
	string Command = ObexPushScriptPath + "PhoneInstall.sh ";
		
	Command += sMac_address + " ";
	Command += "/pluto/bin/" + sFilename;

	//linux only
	system(Command.c_str());
#endif
}

