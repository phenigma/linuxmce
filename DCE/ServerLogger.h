#ifndef ServerLogger_H
#define ServerLogger_H

#include "ClientSocket.h"
#include "AlarmManager.h"
#include "DCE/Logger.h"

namespace DCE
{

class ServerLogger : public Logger, public ClientSocket
{
public:
	//ServerLogger();
	ServerLogger(int DeviceID, int PK_DeviceTemplate, ::std::string server);
	virtual ~ServerLogger();

	virtual void WriteEntry(class Logger::Entry& entry);
	
	virtual void Start();
	virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" )
	{ 
		m_bConnected = ClientSocket::OnConnect(PK_DeviceTemplate,"servlog"); 
		return m_bConnected;
	};

	void RunConnectThread();

	virtual bool IsConnected() { return m_bConnected; }

	virtual int GetType() { return LT_LOGGER_SERVER; }

protected:
	bool m_bConnected;

	bool m_bQuit;
	pthread_t m_Thread;
};
}

#endif
