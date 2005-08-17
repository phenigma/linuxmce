#ifndef BDCommandProcessor_Smartphone_Bluetooth_h
#define BDCommandProcessor_Smartphone_Bluetooth_h

#include "BDCommandProcessor.h"

class BDCommandProcessor_Smartphone_Bluetooth : public BDCommandProcessor
{
	//char *m_pReceiveBuffer;
	//unsigned long m_iSizeReceiveBuffer;
	//pluto_pthread_mutex_t m_ReceiveBufferMutex;

public:
	SOCKET m_ServerSocket;
	SOCKET m_ClientSocket;

	bool m_bClientConnected;
	bool m_bRunning;
	
	pthread_t m_ServerThreadID;

	BDCommandProcessor_Smartphone_Bluetooth();
	~BDCommandProcessor_Smartphone_Bluetooth();

	//from BDCommandProcessor
	bool SendData(int size,const char *data);
	char *ReceiveData(int size);

private:
	bool BT_SetService();
};

#endif
