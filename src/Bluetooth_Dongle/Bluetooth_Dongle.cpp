/*
 Bluetooth_Dongle
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to 
 the terms of the Pluto Public License, available at: 
 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 
 See the Pluto Public License for more details.
 
 */

/**
 *
 * @file Bluetooth_Dongle.cpp
 * @brief source file for the BD_Orbiter and Bluetooth_Dongle classes
 * @author
 * @todo notcommented
 *
 */


 
//<-dceag-d-b->
#include "Bluetooth_Dongle.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "../DCE/DeviceData_Base.h"

#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_DeviceCategory.h"
#include "../pluto_main/Define_DeviceData.h"

#include "../Orbiter/Orbiter.h"
#include "../Orbiter/SDL_Bluetooth/StartOrbiterSDLBluetooth.h"

#include "../VIPShared/BD_CP_SendMeKeystrokes.h"
#include "../VIPShared/BD_CP_SendFile.h"
#include "../VIPShared/BDCommandProcessor_BluetoothDongle.h"

#include "BD/PhoneDevice.h"
#include "BD/BDCommandProcessor.h"

#include "PlutoUtils/PlutoDefs.h"

#include "../Orbiter/Simulator.h"

using namespace DCE;

/** @test
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

void *HandleBDCommandProcessorThread( void *p )
{
	BD_Orbiter_Plus_DongleHandle *pBD_Orbiter_Plus_DongleHandle = (BD_Orbiter_Plus_DongleHandle *)p;

	/** @todo -- need a mutex to protect this, right */
	BD_Orbiter *pBD_Orbiter = pBD_Orbiter_Plus_DongleHandle->m_pBD_Orbiter; // converting it to an BD_Orbiter object
	Bluetooth_Dongle *pBluetooth_Dongle = pBD_Orbiter_Plus_DongleHandle->m_pBluetooth_Dongle;
	string sMacAddress = pBD_Orbiter_Plus_DongleHandle->m_sMacAddress;
	u_int64_t iMacAddress = pBD_Orbiter_Plus_DongleHandle->m_iMacAddress;
	
	if( !pBD_Orbiter || !pBD_Orbiter->m_pBDCommandProcessor || pBD_Orbiter->m_pBDCommandProcessor->m_bDead ) // check if everything is OK first
	{
		if( pBD_Orbiter && pBD_Orbiter->m_pBDCommandProcessor ) // pBD_Orbiter->m_pBDCommandProcessor->m_bDead is true
		{
			delete pBD_Orbiter->m_pBDCommandProcessor;
			pBD_Orbiter->m_pBDCommandProcessor=NULL;
		}
		g_pPlutoLogger->Write( LV_CRITICAL, "Command processor is invalid. Aborting" );
		return NULL;
	}

	if( NULL != pBD_Orbiter->m_pPhoneDevice )
		pBD_Orbiter->m_pPhoneDevice->m_bIsConnected = true;

	BD_CP_SendMeKeystrokes *pBD_CP_SendMeKeystrokes = new BD_CP_SendMeKeystrokes( 1 );
	pBD_Orbiter->m_pBDCommandProcessor->AddCommand( pBD_CP_SendMeKeystrokes );

	/******************************************/
	/** @test BD_CP_SendFile

	char *pFileName = "a.gif"; ///HARDCODING WARNING!
	char *pFileData; 
	unsigned long iFileNameSize = string( pFileName ).size();
	unsigned long iFileDataSize;

	FILE* file;
	file = fopen( "C:\\a.gif", "rb" ); 
	fseek( file, 0, SEEK_END );
	iFileDataSize = ftell( file );
	fseek( file, 0, SEEK_SET );
	pFileData = new char[iFileDataSize];
	fread( pFileData, 1, iFileDataSize, file );
	fclose( file ); 

 	BD_CP_SendFile *pBD_CP_SendFile = new BD_CP_SendFile( pFileName, pFileData, iFileNameSize, iFileDataSize );
	pBD_Orbiter->m_pBDCommandProcessor->AddCommand( pBD_CP_SendFile );

	PLUTO_SAFE_DELETE( pFileData );
	*/
	/******************************************/

	while( pBD_Orbiter->m_pBDCommandProcessor->ReceiveCommand( 0, 0, NULL ) && NULL != pBD_Orbiter->m_pBDCommandProcessor && !pBD_Orbiter->m_pBDCommandProcessor->m_bDead ); // loop for receiving commands

	g_pPlutoLogger->Write( LV_STATUS, "Exiting command processor...");

	g_pPlutoLogger->Write( LV_STATUS, "Sending 'Mobile_orbiter_lost' to Orbiter_Plugin...");
	pBluetooth_Dongle->GetEvents()->Mobile_orbiter_lost( sMacAddress, true );
	
	g_pPlutoLogger->Write( LV_STATUS, "Removing phone from the detection list...");
	pBluetooth_Dongle->RemoveDeviceFromDetectionList( iMacAddress );
	
	if( NULL != pBD_Orbiter->m_pPhoneDevice )
	{
	    PLUTO_SAFETY_LOCK(mm,pBluetooth_Dongle->m_MapMutex);
	    pBluetooth_Dongle->m_mapPhoneDevice_Detected[iMacAddress]->m_bIsConnected = false;
		
		g_pPlutoLogger->Write( LV_STATUS, "Setting PhoneDevice IsConnected flag to false...");
		//pBD_Orbiter->m_pPhoneDevice->m_bIsConnected = false;
	}

	g_pPlutoLogger->Write( LV_STATUS, "Deleting command processor...");
	PLUTO_SAFE_DELETE(pBD_Orbiter->m_pBDCommandProcessor);

	g_pPlutoLogger->Write( LV_STATUS, "Deleting orbiter...");
	PLUTO_SAFE_DELETE(pBD_Orbiter->m_pOrbiter);

	g_pPlutoLogger->Write( LV_STATUS, "Deleting parameter...");
	PLUTO_SAFE_DELETE(pBD_Orbiter_Plus_DongleHandle);

	g_pPlutoLogger->Write( LV_STATUS, "Exiting HandleBDCommandProcessorThread...");
	return NULL;
}

//-----------------------------------------------------------------------------------------------------

//<-dceag-const-b->! custom
Bluetooth_Dongle::Bluetooth_Dongle( int iDeviceID, string sServerAddress, bool bConnectEventHandler, bool bLocalMode, class Router *pRouter )
	: Bluetooth_Dongle_Command( iDeviceID, sServerAddress, bConnectEventHandler, bLocalMode, pRouter ), m_BTMutex( "Bluetooth socket mutex" )
//<-dceag-const-e->
{
	m_iChannel = 10;
	m_BTMutex.Init( NULL );

	Map_DeviceData_Base::iterator it;
	for( it=GetData()->m_AllDevices.m_mapDeviceData_Base.begin(); it!=GetData()->m_AllDevices.m_mapDeviceData_Base.end(); ++it )
	{
		DeviceData_Base *pDeviceData_Base = ( *it ).second;
		if( pDeviceData_Base->WithinCategory( DEVICECATEGORY_Mobile_Orbiter_CONST ) )
		{
			if( pDeviceData_Base->m_sMacAddress.length() ) // it has a associated mac address
			{
				m_mapOrbiterSockets[pDeviceData_Base->m_sMacAddress] = new BD_Orbiter;
				// We're definitely not connected to this right now.
				GetEvents()->Mobile_orbiter_lost( pDeviceData_Base->m_sMacAddress, false );
				g_pPlutoLogger->Write( LV_STATUS, "Accepting bluetooth connections from %s", pDeviceData_Base->m_sMacAddress.c_str() );
			}
			else
				g_pPlutoLogger->Write( LV_WARNING, "Mobile orbiter: %d found, with no mac address", pDeviceData_Base->m_dwPK_Device );
		}
	}
	StartScanning(); // start the scanning loop
}
//<-dceag-const2-b->!

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
	return Connect(PK_DeviceTemplate_get()); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Bluetooth_Dongle::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Bluetooth_Dongle::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//-----------------------------------------------------------------------------------------------------

bool Bluetooth_Dongle::ScanningLoop()
{
	// First, scan the map for connections that have died.
	map<string, BD_Orbiter *>::iterator iMos;
	{
		PLUTO_SAFETY_LOCK( bm, m_BTMutex );
		for( iMos = m_mapOrbiterSockets.begin(); 
			iMos != m_mapOrbiterSockets.end(); ++iMos )
		{
			BD_Orbiter *p_BD_Orbiter = ( *iMos ).second;
			if ( NULL != p_BD_Orbiter->m_pBDCommandProcessor && p_BD_Orbiter->m_pBDCommandProcessor->m_bDead )
			{
				GetEvents()->Mobile_orbiter_lost( ( *iMos ).first.c_str(), false );
				BDCommandProcessor *pSock = p_BD_Orbiter->m_pBDCommandProcessor;
				p_BD_Orbiter->m_pBDCommandProcessor = NULL;
				pSock->m_bExit = true; // Cause the scan loop to terminate
			}		
		}
	}

#ifdef SIMULATE_DETECTION
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

// temporary hacks
/* TODO - FIND A SOLUTION

THE FRAMEWORK DETECTS A PHONE ONE SCAN, CONNECTS, BUT THEN DOESN'T DETECT IT AGAIN.  SEEMS TO BE A BUG
IN THE BLUETOOTH LIBRARIES.  SO, IF THE FRAMEWORK THINKS A DEVICE IS LOST, WE'LL INTERCEPT IT HERE AND
CHECK TO SEE IF WE HAVE A CONNECTION.  IF SO WE'LL IGNORE THE LOST DEVICE.  SIMILARLY WE'LL IGNOER
THE FOUND DEVICE IF WE'RE ALREADY CONNECTED.  WE'LL JUST PUT IT IN THE MAP AND DO NOTHING */

// This will also add the phone to the map
void Bluetooth_Dongle::Intern_NewDeviceDetected(class PhoneDevice *pDevice)
{
	BD_Orbiter *pBD_Orbiter = m_mapOrbiterSockets_Find( pDevice->m_sMacAddress );
	if ( pBD_Orbiter==NULL || pBD_Orbiter->m_pOrbiter==NULL || pBD_Orbiter->m_pBDCommandProcessor==NULL || pBD_Orbiter->m_pBDCommandProcessor->m_bDead )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Bluetooth dongle intercepted new device.  We're not connected.  Proceeding like normal.");
		PhoneDetectionEngine::Intern_NewDeviceDetected(pDevice);
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Bluetooth dongle intercepted new device.  We're connected.  Ignoring it %p",pBD_Orbiter);

		if( pBD_Orbiter )
			g_pPlutoLogger->Write(LV_STATUS,"Status: %p %p %p %d %d",pBD_Orbiter->m_pOrbiter,pBD_Orbiter->m_pBDCommandProcessor,
					pBD_Orbiter->m_pPhoneDevice, pBD_Orbiter->m_pPhoneDevice->m_bIsConnected,
					pBD_Orbiter->m_pBDCommandProcessor->m_bDead ? 1 : 0);

		/*
		PhoneDevice *pDExisting = m_mapPhoneDevice_Detected_Find(pDevice->m_iMacAddress);

		if(pDExisting)
			delete pDExisting;

		PLUTO_SAFETY_LOCK(mm,m_MapMutex);
		m_mapPhoneDevice_Detected[pDevice->m_iMacAddress] = pDevice;
		*/

		//don't replace the phonedevice`
		if(pDevice)
			delete pDevice;
	}
}

//-----------------------------------------------------------------------------------------------------

void Bluetooth_Dongle::Intern_LostDevice(class PhoneDevice *pDevice)
{
	BD_Orbiter *pBD_Orbiter = m_mapOrbiterSockets_Find( pDevice->m_sMacAddress );
	if ( pBD_Orbiter==NULL || pBD_Orbiter->m_pOrbiter==NULL || pBD_Orbiter->m_pBDCommandProcessor==NULL || pBD_Orbiter->m_pBDCommandProcessor->m_bDead )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Bluetooth dongle intercepted lost device.  We're not connected.  Proceeding like normal");
		PhoneDetectionEngine::Intern_LostDevice(pDevice);
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Bluetooth dongle intercepted lost device.  We are connected.  Ignoring it%p", pBD_Orbiter);
		if( pBD_Orbiter )
			g_pPlutoLogger->Write(LV_STATUS,"Status: %p %p %d",pBD_Orbiter->m_pOrbiter,pBD_Orbiter->m_pBDCommandProcessor,pBD_Orbiter->m_pBDCommandProcessor->m_bDead ? 1 : 0);
	}
}

//-----------------------------------------------------------------------------------------------------

void Bluetooth_Dongle::NewDeviceDetected( class PhoneDevice *pDevice )
{
	// We'll just handle this the same way
	SignalStrengthChanged( pDevice );
}

//-----------------------------------------------------------------------------------------------------

void Bluetooth_Dongle::LostDevice( class PhoneDevice *pDevice )
{
	BD_Orbiter *pBD_Orbiter = m_mapOrbiterSockets_Find( pDevice->m_sMacAddress );

	bool bConnectionFailed = 
		NULL != pBD_Orbiter 
		&& NULL != pBD_Orbiter->m_pPhoneDevice
		&& !pBD_Orbiter->m_pPhoneDevice->m_bIsConnected;

	GetEvents()->Mobile_orbiter_lost( pDevice->m_sMacAddress.c_str(), bConnectionFailed );

	if ( NULL != pBD_Orbiter && NULL != pBD_Orbiter->m_pOrbiter )
	{
		g_pPlutoLogger->Write( LV_WARNING, "Lost %s device!", pDevice->m_sMacAddress.c_str() );
		
		if( NULL != pBD_Orbiter->m_pPhoneDevice )
			pBD_Orbiter->m_pPhoneDevice->m_bIsConnected = false;

		delete pBD_Orbiter->m_pOrbiter;
		pBD_Orbiter->m_pOrbiter = NULL;

		g_pPlutoLogger->Write( LV_WARNING, "Orbiter deleted for %s device!", pDevice->m_sMacAddress.c_str() );
			
		if( NULL != pBD_Orbiter->m_pBDCommandProcessor )
		{
			delete pBD_Orbiter->m_pBDCommandProcessor;
			pBD_Orbiter->m_pBDCommandProcessor = NULL;

			g_pPlutoLogger->Write( LV_WARNING, "CommandProcessor deleted for %s device!", pDevice->m_sMacAddress.c_str() );
		}

		return;
	}
}

//-----------------------------------------------------------------------------------------------------

void Bluetooth_Dongle::SignalStrengthChanged( class PhoneDevice *pDevice )
{
	BD_Orbiter *pBD_Orbiter = m_mapOrbiterSockets_Find( pDevice->m_sMacAddress );

	if( NULL != pBD_Orbiter )
		pBD_Orbiter->m_pPhoneDevice = pDevice;

 	if ( !pDevice->m_bIsConnected )
	{
		g_pPlutoLogger->Write( LV_WARNING, "Detected device mac: %s link quality: %d", 
			pDevice->m_sMacAddress.c_str(), pDevice->m_iLinkQuality );

		GetEvents()->Mobile_orbiter_detected( pDevice->m_sMacAddress, pDevice->m_iLinkQuality, pDevice->m_sID );
	}	
}

//-----------------------------------------------------------------------------------------------------
/*

	COMMANDS TO IMPLEMENT

*/
//-----------------------------------------------------------------------------------------------------

//<-dceag-c60-b->

	/** @brief COMMAND: #60 - Link with mobile orbiter */
	/** After the dongle has detected the device, then the orbiter plugin will send this command to tell the dongle to link to the phone. */
		/** @param #42 MediaPosition */
			/** On = 1 (link to the orbiter or link to the phone); Off = 0 (unlink) */
		/** @param #47 Mac address */
			/** The mac address of the phone */

void Bluetooth_Dongle::CMD_Link_with_mobile_orbiter(int iMediaPosition,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c60-e->
{
	cout << "Command #60 - Link with mobile orbiter" << endl;
	cout << "Parm #42 - MediaPosition=" << iMediaPosition << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;

	//printf( "Got link with MO log: %p\n\n", g_pPlutoLogger );
	//printf( "on_off: %d\nID: %s\n", iMediaPosition, sMac_address.c_str() );

	{
		PLUTO_SAFETY_LOCK( bm, m_BTMutex );
		map<string, BD_Orbiter *>::iterator iMos = m_mapOrbiterSockets.find( sMac_address );
		if ( iMos == m_mapOrbiterSockets.end() )
		{
			 //#define NOSQL
			#ifdef NOSQL
				// hack it in so it works without the database
				m_mapOrbiterSockets[sMac_address] = new BD_Orbiter;
				iMos = m_mapOrbiterSockets.find( sMac_address );
			#else
				g_pPlutoLogger->Write( LV_CRITICAL, "Got link with MO for nonexistent ID %s!", sMac_address.c_str() );
				return;
			#endif
		}

		BD_Orbiter *pBD_Orbiter = ( *iMos ).second;

		if ( iMediaPosition == 0 )
		{
			if ( pBD_Orbiter->m_pBDCommandProcessor == NULL )
			{
				g_pPlutoLogger->Write( LV_STATUS, "Got disconnect for unlinked MO %s!", sMac_address.c_str() );
			}
			else
			{
				pBD_Orbiter->m_pBDCommandProcessor = NULL;
				bm.Release();
				pBD_Orbiter->m_pBDCommandProcessor->m_bExit = true; // Cause the scan loop to terminate
			}
			return;
		}

		if ( pBD_Orbiter->m_pBDCommandProcessor != NULL )
		{
			if( pBD_Orbiter->m_pBDCommandProcessor->m_bDead )
			{
				g_pPlutoLogger->Write( LV_STATUS, "Try to reconnect to MO %s. The connection with the app seems to be dead.", sMac_address.c_str() );

				delete pBD_Orbiter->m_pBDCommandProcessor;
				pBD_Orbiter->m_pBDCommandProcessor = NULL;
			}
			else
			{
				g_pPlutoLogger->Write( LV_STATUS, "Got link with MO %s that's already linked!", sMac_address.c_str() );
				return;
			}
		}
	}

	// Do this just to get the id
	PhoneDevice pdTemp( "", sMac_address, 0 );
	PhoneDevice *pD = m_mapPhoneDevice_Detected_Find( pdTemp.m_iMacAddress );
	if( !pD )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot find device %s anymore to link to, map has %d entries", sMac_address.c_str(), (int) m_mapPhoneDevice_Detected.size());
		return;
	}


#ifdef BT_SOCKET
	// We must be using the DetectionSimulator
	// We'll borrow the establishment socket
	EstablishmentSocket *pSocket = new EstablishmentSocket( 1, "localhost:3461", "foo" );
	if( !pSocket->Connect() )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to phone on Socket sServerAddress" );
		return;
	}

	BDCommandProcessor *pProcessor = 
		new BDCommandProcessor_BluetoothDongle( this, sMac_address, pSocket );
#else
	//printf( "create cmd proc: %p\n\n", g_pPlutoLogger );
	BDCommandProcessor *pProcessor = 
		new BDCommandProcessor_BluetoothDongle( this, sMac_address, pD );
#endif
 /** @test */
	//PLUTO_SAFETY_LOCK( bm, m_BTMutex );
	BD_Orbiter *pBD_Orbiter = m_mapOrbiterSockets_Find( sMac_address );
	pBD_Orbiter->m_pBDCommandProcessor = pProcessor;
	
	if( NULL != pProcessor && !pProcessor->m_bDead )
	{
	    pthread_create( &pProcessor->m_BDSockThreadID, NULL, HandleBDCommandProcessorThread, 
			( void* )new BD_Orbiter_Plus_DongleHandle(pBD_Orbiter, this, 
													  pBD_Orbiter->m_pPhoneDevice->m_sMacAddress,
													  pBD_Orbiter->m_pPhoneDevice->m_iMacAddress
		    ) 
		);

		//GetEvents()->Mobile_orbiter_linked( sMac_address, "version" ); /** @todo: Chris add this */
	}
}

//-----------------------------------------------------------------------------------------------------

//<-dceag-c61-b->

	/** @brief COMMAND: #61 - Get Signal Strength */
	/** Returns the actual signal strength of the phone for this dongle. */
		/** @param #47 Mac address */
			/** The mac address of the phone */
		/** @param #48 Value */
			/** The strength of the connection
(255 is the maximum strength) */

void Bluetooth_Dongle::CMD_Get_Signal_Strength(string sMac_address,int *iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c61-e->
{
	cout << "Need to implement command #61 - Get Signal Strength" << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;
	//cout << "Parm #48 - Value_int_=" << *iValue << endl;

	//BDCommandProcessor *pBDCommandProcessor = m_mapOrbiterSockets_Find( sMac_address );
	int iLinkQuality = GetLinkQuality(sMac_address.c_str());

	g_pPlutoLogger->Write( LV_WARNING, "Link quality for %s device is %d", sMac_address.c_str(), iLinkQuality );

	*iValue = iLinkQuality;
} 

//-----------------------------------------------------------------------------------------------------

//<-dceag-c62-b->

	/** @brief COMMAND: #62 - Create Mobile Orbiter */
	/** The bluetooth dongle spawns an internal mobile orbiter which communicates with the phone. */
		/** @param #2 PK_Device */
			/** The ID of the controller to spawn. */
		/** @param #47 Mac address */
			/** The Mac Address of the phone. */

void Bluetooth_Dongle::CMD_Create_Mobile_Orbiter(int iPK_Device,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c62-e->
{
	PLUTO_SAFETY_LOCK( bm, m_BTMutex );
	
	// Lookup the mobile orbiter entry
	BD_Orbiter *pBD_Orbiter = m_mapOrbiterSockets_Find( sMac_address );

	g_pPlutoLogger->Write( LV_WARNING, "$$$ CMD_Create_Mobile_Orbiter received for %s device $$$", sMac_address.c_str() );
	
	if( NULL != pBD_Orbiter->m_pBDCommandProcessor && !pBD_Orbiter->m_pBDCommandProcessor->m_bDead )
	{
printf( "$$$ Ready to delete the orbiter...\n" );
		
		if( NULL != pBD_Orbiter->m_pOrbiter )
		{
			delete pBD_Orbiter->m_pOrbiter;
			pBD_Orbiter->m_pOrbiter = NULL;
		}

		g_pPlutoLogger->Write( LV_WARNING, "Orbiter created for %s device", sMac_address.c_str() );

		class OrbiterSDLBluetooth *pOrbiter = 
			StartOrbiterSDLBluetooth( pBD_Orbiter->m_pBDCommandProcessor, iPK_Device, m_sIPAddress, "", false, 176, 208 );

		if(NULL != pOrbiter)
		{
			Simulator::GetInstance()->m_pOrbiter = (Orbiter *)pOrbiter;	
#ifdef WIN32			
			Simulator::GetInstance()->LoadConfigurationFile("BluetoothDongle.conf");
#else //LINUX
			Simulator::GetInstance()->LoadConfigurationFile("/etc/BluetoothDongle.conf");
#endif			
		}	

		pBD_Orbiter->m_pOrbiter = ( Orbiter * )pOrbiter;
	}
 else
	{
		if( NULL == pBD_Orbiter->m_pBDCommandProcessor )
			g_pPlutoLogger->Write( LV_WARNING, "Cannot create orbiter for %s device: the CommandProcessor is null!", sMac_address.c_str() );
		else
			g_pPlutoLogger->Write( LV_WARNING, "Cannot create orbiter for %s device: the CommandProcessor is dead!", sMac_address.c_str() );
	}

printf( "$$$ CMD_Create_Mobile_Orbiter end for %s device\n", sMac_address.c_str() );
}

//-----------------------------------------------------------------------------------------------------

//<-dceag-sample-b->! no sample
//<-dceag-c80-b->

	/** @brief COMMAND: #80 - Send File To Device */
	/** Send a file to a Bluetooth device. */
		/** @param #13 Filename */
			/** The file to send */
		/** @param #47 Mac address */
			/** The MAC Address */
		/** @param #58 IP Address */
			/** The IP Address */

void Bluetooth_Dongle::CMD_Send_File_To_Device(string sFilename,string sMac_address,string sIP_Address,string &sCMD_Result,Message *pMessage)
//<-dceag-c80-e->
{
	g_pPlutoLogger->Write( LV_STATUS, "Send File: %s to %s", sFilename.c_str(), sMac_address.c_str() );
	
#ifndef _WIN32
	//sleep( 10 );
	
	string ObexPushScriptPath = "/usr/pluto/bin/";//TODO
	string Command = ObexPushScriptPath + "PhoneInstall.sh ";
		
	Command += sMac_address + " ";
	Command += "/usr/pluto/bin/" + sFilename;

	//linux only
	system( Command.c_str() );
#endif
}

//<-dceag-createinst-b->!
