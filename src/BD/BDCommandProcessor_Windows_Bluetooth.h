#ifndef BDCommandProcessor_Windows_Bluetooth_h
#define BDCommandProcessor_Windows_Bluetooth_h

#include "BDCommandProcessor.h"

#include "WinBluetooth/inc/BtIfDefinitions.h"
#include "WinBluetooth/inc/BtIfClasses.h"
#include "WinBluetooth/StdDll/StdDLLif.h"
#include "WinBluetooth/StdDll/BtServiceSelection.h"

class BDCommandProcessor_Windows_Bluetooth : public BDCommandProcessor, public CRfCommTranspCallbackIf
{
protected:
	class Socket *m_pSocket;
	CRfCommTransport	*m_ptr_rfcm;
	bool	m_IsConnected;
	char *m_pReceiveBuffer;
	unsigned long m_iSizeReceiveBuffer;
	pluto_pthread_mutex_t m_ReceiveBufferMutex;

public:
	pthread_t m_BDSockThreadID;
	pthread_cond_t m_PollingCond;

	BDCommandProcessor_Windows_Bluetooth(string sMacAddressPhone,string sMacAddressDongle);
	~BDCommandProcessor_Windows_Bluetooth();

	bool SendData(int size,const char *data);
	char *ReceiveData(int size);

	void OnTranspEventReceived(  UINT32 event_code )
	{
	}
	void OnTranspDataReceived( BYTE* data, ULONG len );


};

#endif
