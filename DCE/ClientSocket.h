#ifndef DCECLIENTSOCKET_H
#define DCECLIENTSOCKET_H

// The ClientSocket's responsibility is to make, and keep a reliable
// connection to the server.

//#include "FileTransfer.h"
#include "Socket.h"

#define MAX_RETRIES 3 // Maximum retries to establish TCP connection

namespace DCE
{
	class SocketListener;

	class ClientSocket : public Socket
	{    
	public:
		string m_IPAddress;
		int m_DeviceID;
		clock_t m_clockTimeout;
		bool m_bWatchdogRunning, m_bStopWatchdog;
		pthread_t m_pThread;
		string m_MyIPAddress;

		virtual bool Connect(string ExtraInfo="");
		virtual bool OnConnect(string ExtraInfo="");
		virtual void Disconnect();

		ClientSocket( int DeviceID, string IPAddress,string Name);
		virtual ~ClientSocket();

		// Watchdog will kill the connection if it exceeds Timeout.
		// Eventually SendMessage will need to be overridden to create
		// a watchdog and retransmit if they don't go through.

		void StartWatchDog(clock_t Timeout);
		void StopWatchDog();
	};
}

#endif
