//<-dceag-d-b->
#ifndef Bluetooth_Dongle_h
#define Bluetooth_Dongle_h

//	DCE Implemenation for #13 Bluetooth Dongle

#include "Gen_Devices/Bluetooth_DongleBase.h"
//<-dceag-d-e->

namespace DCE
{
	class Orbiter;
}

using namespace DCE;

#include "BD/BDCommandProcessor.h"

class BD_Orbiter
{
public:
	class BDCommandProcessor *m_pBDCommandProcessor;
	Orbiter *m_pOrbiter;
	class PhoneDevice *m_pPhoneDevice;

	BD_Orbiter() { m_pBDCommandProcessor=NULL; m_pOrbiter=NULL; m_pPhoneDevice=NULL; }
};

#ifdef BT_SOCKET
	#include "VIPShared/PhoneDetection_Simulate.h"
#else
	#ifndef WIN32
	#include <bluetooth/bluetooth.h>
	#include <bluetooth/rfcomm.h>
	#include <bluetooth/hci.h>
	#include <bluetooth/hci_lib.h>
	#include "VIPShared/PhoneDetection_Bluetooth_Linux.h"
	#else
	#include "VIPShared/PhoneDetection_Bluetooth_Windows.h"
	#endif
#endif


//<-dceag-decl-b->! this is custom
namespace DCE
{
#ifdef BT_SOCKET
	class Bluetooth_Dongle : public Bluetooth_Dongle_Command, public PhoneDetection_Simulate
#else
	#ifdef WIN32
	class Bluetooth_Dongle : public Bluetooth_Dongle_Command, public PhoneDetection_Bluetooth_Windows
	#else
	class Bluetooth_Dongle : public Bluetooth_Dongle_Command, public PhoneDetection_Bluetooth_Linux
	#endif
#endif
{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Bluetooth_Dongle(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Bluetooth_Dongle();
		virtual bool Register();
//<-dceag-const-e->

	// Private member variables
	pluto_pthread_mutex_t m_BTMutex;
	int m_Channel;

	// Private methods
public:
	// Public member variables
	map<string, BD_Orbiter *> m_OrbiterSockets;
	BD_Orbiter *m_OrbiterSockets_Find(string MacAddress) {
		map<string, class BD_Orbiter *>::iterator it = m_OrbiterSockets.find(MacAddress);
		return it==m_OrbiterSockets.end() ? NULL : (*it).second;
	}

	// From PhoneDetectionEngine class 
	
	// This is the actual scan loop that gets called by the worker thread
	// The worker thread will set m_bInScanLoop to true, and keep calling it until
	// the thread returns false.  This loop will add new devices to the map, and call
	// Intern_ methods
	virtual bool ScanningLoop();

	// The derived class must decide how to handle when new devices are detected or lost, or 
	// when the signal strength changes by more than 10 in either direction
	virtual void NewDeviceDetected(class PhoneDevice *pDevice);
	virtual void LostDevice(class PhoneDevice *pDevice);
	virtual void SignalStrengthChanged(class PhoneDevice *pDevice);
	virtual void CheckConnection(class PhoneDevice *pDevice);

	string MacAddress() { return GetData()->m_sMacAddress; }
//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_Mobile_orbiter_detected(string sMac_Address,int iSignal_Strength,string sID);
	void EVENT_Mobile_orbiter_linked(string sMac_Address,string sVersion);
	void EVENT_Mobile_orbiter_lost(string sMac_Address,bool bConnectionFailed);

			*****COMMANDS***** we need to implement
	*/

/* 
	COMMAND: #60 - Link with mobile orbiter
	COMMENTS: After the dongle has detected the device, then the orbiter plugin will send this command to tell the dongle to link to the phone.
	PARAMETERS:
		#42 MediaPosition
			On = 1 (link to the orbiter or link to the phone); Off = 0 (unlink)
		#47 Mac address
			The mac address of the phone
*/
	virtual void CMD_Link_with_mobile_orbiter(int iMediaPosition,string sMac_address) { string sCMD_Result; CMD_Link_with_mobile_orbiter(iMediaPosition,sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Link_with_mobile_orbiter(int iMediaPosition,string sMac_address,string &sCMD_Result,Message *pMessage);

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
	virtual void CMD_Get_Signal_Strength(string sMac_address,int *iValue_int) { string sCMD_Result; CMD_Get_Signal_Strength(sMac_address.c_str(),iValue_int,sCMD_Result,NULL);};
	virtual void CMD_Get_Signal_Strength(string sMac_address,int *iValue_int,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #62 - Create Mobile Orbiter
	COMMENTS: The bluetooth dongle spawns an internal mobile orbiter which communicates with the phone.
	PARAMETERS:
		#2 PK_Device
			The ID of the controller to spawn.
		#47 Mac address
			The Mac Address of the phone.
*/
	virtual void CMD_Create_Mobile_Orbiter(int iPK_Device,string sMac_address) { string sCMD_Result; CMD_Create_Mobile_Orbiter(iPK_Device,sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Create_Mobile_Orbiter(int iPK_Device,string sMac_address,string &sCMD_Result,Message *pMessage);

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
	virtual void CMD_Send_File_To_Device(string sFilename,string sMac_address,string sIP_Address) { string sCMD_Result; CMD_Send_File_To_Device(sFilename.c_str(),sMac_address.c_str(),sIP_Address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_File_To_Device(string sFilename,string sMac_address,string sIP_Address,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
