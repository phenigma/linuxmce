#ifndef VIPEmulator_SOCKET_H
#define VIPEmulator_SOCKET_H

#include "DCE/Socket.h"

using namespace std;

class OCServerCore;
class DCELogger;
class VIPEmulator;

using namespace DCE;

class VIPEmulatorSocket : public Socket
{
public:
	long m_DeviceID;
	pthread_t m_ClientThreadID;
	map<string,string> m_mapHTTPHeaders;
	string m_CMDString;
    
	class DCELogger *g_pDCELogger;

	VIPEmulator *m_pListener;
	class OCDeviceCommand *m_pCommand;

	VIPEmulatorSocket(VIPEmulator *pListener, SOCKET Sock);
	virtual ~VIPEmulatorSocket();

	string MakeUrl(map<long,string>::iterator &i, string Page);

	void Run();
};

#endif

