/**
 *
 * @file BDCommandProcessor_BluetoothDongle.h
 * @brief header file for the BDCommandProcessor_BluetoothDongle class
 * @author
 * @todo notcommented
 *
 */

#ifndef BDCOMMANDPROCESSORBLUETOOTHDONGLE_H
#define BDCOMMANDPROCESSORBLUETOOTHDONGLE_H

//-----------------------------------------------------------------------------------------------------

#ifdef BT_SOCKET
#else
	#ifdef WIN32
		#include "../VIPShared/BDCommandProcessor_Windows_Bluetooth.h"
	#else
		#include "../VIPShared/BDCommandProcessor_Linux_Bluetooth.h"
	#endif //WIN32
#endif //BT_SOCKET

//-----------------------------------------------------------------------------------------------------

/** we use different definitions depending on the environment */

/**
 * @brief implementation for the BDCommandProcessor specific to the BluetoothDongle
 * @todo ask
 */
#ifdef BT_SOCKET
	#ifdef WIN32
class BDCommandProcessor_BluetoothDongle : public BDCommandProcessor_Windows_Socket
	#else
class BDCommandProcessor_BluetoothDongle : public BDCommandProcessor_Linux_Socket
	#endif
#else
	#ifdef WIN32
class BDCommandProcessor_BluetoothDongle : public BDCommandProcessor_Windows_Bluetooth
	#else
class BDCommandProcessor_BluetoothDongle : public BDCommandProcessor_Linux_Bluetooth
	#endif //WIN32
#endif //BT_SOCKET
{
public:

	class Bluetooth_Dongle *m_pBluetooth_Dongle; /**< points to the bluetooth dongle object for this command processor @todo better comment */

	BDCommandProcessor_BluetoothDongle( class Bluetooth_Dongle *pBluetooth_Dongle, string sMacAddressPhone, class PhoneDevice *pPhoneDevice
#ifdef BT_SOCKET
		, class Socket *pSocket
#endif
	 );
};

//-----------------------------------------------------------------------------------------------------

#endif // __BTCOMMANDPROCESSOR_BLUETOOTH_DONGLE_H__
