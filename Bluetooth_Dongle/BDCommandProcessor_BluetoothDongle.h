#ifndef __BTCOMMANDPROCESSOR_BLUETOOTH_DONGLE_H__
#define __BTCOMMANDPROCESSOR_BLUETOOTH_DONGLE_H__

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

	class Bluetooth_Dongle *m_pBluetooth_Dongle;

	BDCommandProcessor_BluetoothDongle(
		class Bluetooth_Dongle *pBluetooth_Dongle,
		string sMacAddressPhone,
		class PhoneDevice *pPhoneDevice
#ifdef BT_SOCKET
		,class Socket *pSocket
#endif
	);
};
//-----------------------------------------------------------------------------------------------------
#endif // __BTCOMMANDPROCESSOR_BLUETOOTH_DONGLE_H__
