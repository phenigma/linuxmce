/**
 *
 * @file Bluetooth_Dongle.h
 * @brief header file for the BD_Orbiter and Bluetooth_Dongle classes
 * @author
 * @todo notcommented
 *
 */

 
//<-dceag-d-b->
#ifndef Bluetooth_Dongle_h
#define Bluetooth_Dongle_h

//	DCE Implemenation for #13 Bluetooth Dongle

#include "Gen_Devices/Bluetooth_DongleBase.h"
//<-dceag-d-e->

#ifdef SIMULATE_DETECTION
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

namespace DCE
{
	class Orbiter; /**< to be able to use it in declarations */
}

using namespace DCE;

#include "BD/BDCommandProcessor.h"

/**
 * @brief this class incapsulates what we need to assure the communication between a phone device and an orbiter instance
 * @todo ask
 */
class BD_Orbiter
{

public:

	class BDCommandProcessor *m_pBDCommandProcessor; /**< points to a BD command processor used for communicaion between the phone and the orbiter @todo ask */
	class PhoneDevice *m_pPhoneDevice; /**< the phone device */
	
	Orbiter *m_pOrbiter; /**< points to the Orbiter object */

	/**
	 * @brief constructor that assignes NULL to the member data ( witch are all pointers )
	 */
	BD_Orbiter() { m_pBDCommandProcessor=NULL; m_pOrbiter=NULL; m_pPhoneDevice=NULL; }
	
};

namespace DCE
{
	class Bluetooth_Dongle;
}

class BD_Orbiter_Plus_DongleHandle
{
public:
	BD_Orbiter* m_pBD_Orbiter;
	Bluetooth_Dongle* m_pBluetooth_Dongle;
	string m_sMacAddress;
	u_int64_t m_iMacAddress;

	BD_Orbiter_Plus_DongleHandle(class BD_Orbiter* pBD_Orbiter, class Bluetooth_Dongle* pBluetooth_Dongle, 
			string sMacAddress, u_int64_t iMacAddress)
	{
		m_pBD_Orbiter = pBD_Orbiter;
		m_pBluetooth_Dongle = pBluetooth_Dongle;
		m_sMacAddress = sMacAddress;
		m_iMacAddress = iMacAddress;
	}
};

//<-dceag-decl-b->! this is custom
namespace DCE
{
	/** we use different definitions depending on the environment */
	
	/**
	 * @brief used to allow comunication between the router and the phone
	 */
#ifdef SIMULATE_DETECTION
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
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	
		pluto_pthread_mutex_t m_BTMutex; /**< to control access to the shared memory */
		int m_iChannel; /**< specifies the channel to use */
	
	public:
		// Public member variables
		map<string, BD_Orbiter *> m_mapOrbiterSockets;
		
		/**
		 * @brief finds the BD_Orbiter object searching it by mac address
		 */
		BD_Orbiter *m_mapOrbiterSockets_Find( string sMacAddress ) 
		{
			map<string, class BD_Orbiter *>::iterator it = m_mapOrbiterSockets.find( sMacAddress );
			return it == m_mapOrbiterSockets.end() ? NULL : ( *it ).second;
		}
	
		/** From PhoneDetectionEngine class */
		
		/** 
		 * @brief This is the actual scan loop that gets called by the worker thread
		 * The worker thread will set m_bInScanLoop to true, and keep calling it until
		 * the thread returns false. This loop will add new devices to the map, and call
		 * Intern_ methods
		 */
		virtual bool ScanningLoop();
	
		/**
		 * The derived class must decide how to handle when new devices are detected or lost, or 
		 * when the signal strength changes by more than 10 in either direction
		 */
		
		/**
		 * @brief used to register a new device by the PhoneDetection base class
		 * calls SignalStrengthChanged to register the new device 
		 * @see SignalStrengthChanged
		 */ 
		virtual void NewDeviceDetected( class PhoneDevice *pDevice );

		/**
		 * @brief checks for different situations in which the phone device might be lost and deletes the allocated data if it is really lost
		 * @todo ask
		 */
		virtual void LostDevice( class PhoneDevice *pDevice );
		
		virtual void Intern_NewDeviceDetected(class PhoneDevice *pDevice);

		virtual void Intern_LostDevice(class PhoneDevice *pDevice);

		/**
		 * @brief called if the signal strength varies by moe then 10 to refresh the connection
		 * @todo ask
		 */
		virtual void SignalStrengthChanged( class PhoneDevice *pDevice );
		
		/**
		 * @brief gets the private member data
		 */
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


//<-dceag-h-e->
	};
	
	//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
