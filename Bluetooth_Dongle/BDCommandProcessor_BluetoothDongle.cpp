#include "BD/BDCommandProcessor.h"
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"

//-----------------------------------------------------------------------------------------------------
BDCommandProcessor_BluetoothDongle::BDCommandProcessor_BluetoothDongle(
	class Bluetooth_Dongle *pBluetooth_Dongle, 
	string sMacAddressPhone,
	class PhoneDevice *pPhoneDevice
#ifdef BT_SOCKET
	,class Socket *pSocket
#endif
) :
#ifdef BT_SOCKET
		BDCommandProcessor_Windows_Socket(sMacAddressPhone, pSocket)
#else
	#ifdef WIN32
		BDCommandProcessor_Windows_Bluetooth(sMacAddressPhone, pBluetooth_Dongle->MacAddress())
	#else
		BDCommandProcessor_Linux_Bluetooth(sMacAddressPhone,pBluetooth_Dongle->MacAddress(),pPhoneDevice)
	#endif //WIN32
#endif //BT_SOCKET
{
	m_pBluetooth_Dongle = pBluetooth_Dongle;
}
//-----------------------------------------------------------------------------------------------------
