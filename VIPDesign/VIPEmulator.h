#ifndef VIPEmulator_H
#define VIPEmulator_H

#include "DCE/SocketListener.h"
#include "DCE/HandleRequestSocket.h"

using namespace DCE;

class VIPEmulator : public SocketListener
{
public:
	VIPEmulator() : 
		DCESocketListener("VIPEmulator")
	{
	}

	virtual ~VIPEmulator()
	{
	}

	void Initialize();


	virtual Socket *CreateSocket(SOCKET newsock);

	virtual bool ReceivedString(Socket *pSocket, string Line)
	{
		return true;
	};

	virtual void ReceivedDCEMessage(Socket *pSocket, Message *pMessage)
	{
	};

	virtual void RegisteredCommandHandler(int DeviceID)
	{
	};

	void Run();
};

#endif

