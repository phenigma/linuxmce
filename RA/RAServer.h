#ifndef RAServer_H
#define RAServer_H

#include "SocketListener.h"
#include "HandleRequestSocket.h"

class RAServer : public DCE::SocketListener  // Borrow DCE's Socket listener
{
public:
	RAServer();
	virtual ~RAServer();

	void Initialize();
	void Run();

	virtual DCE::Socket *CreateSocket(SOCKET newsock,string Name);

	// We're just borrowing DCE's socket listener class, which requires the following that are only for DCE.
	// We don't need them, but the base class is abstract, so we'll define them anyway
	virtual bool ReceivedString(DCE::Socket *pSocket, string Line) { return true; };
	virtual void ReceivedMessage(DCE::Socket *pSocket, DCE::Message *pMessage) {};
	virtual void RegisteredCommandHandler(int DeviceID)	{};

};

#endif

