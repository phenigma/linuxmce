
#ifndef __CORP_CLIENT_H__3562
#define __CORP_CLIENT_H__3562

#include <string>
#include "PlutoUtils/CommonIncludes.h"	
#include "SocketListener.h"
#include "AlarmManager.h"
#include "HandleRequestSocket.h"
#include "Event_Impl.h"

class PlutoServerCore;

class CorpClient : public HandleRequestSocket, AlarmEvent
{
public:
	CorpClient(PlutoServerCore* pCore, int ServerID, string IPAddress);
	
	virtual ~CorpClient();
	virtual void Disconnect();
	virtual bool OnConnect();

	virtual bool SendString(string s);


	virtual void ReceivedString(string Line);
	virtual bool ReceivedMessage(Message *pMessage);
	virtual bool SendMessage(Message *pMessage);
	

	void Send_ShutdownEvent(int status);
	void Send_StartupEvent(int status);
	void Send_Ping();
	void Send_PackageVersion(string name, string version);
	void Send_Log();
	
	bool m_bRun;
	virtual void AlarmCallback(int id, void* param);
protected:
	bool Do_UpdateEPG(Message* pMessage);
	bool Do_UpdateConfig(Message* pMessage);
	bool Do_UpdateSoftware(Message* pMessage);
	bool Do_Restart(Message* pMessage);
	bool Do_SendLog(Message* pMessage);

	PlutoServerCore* m_pCore;
	Event_Impl* m_pEvent;
	int m_ServerID;
	string m_ServerAddress;
	pthread_t m_ReconnectThread;

	bool HasConnected;
	enum {ALARM_PING};
};

#endif

