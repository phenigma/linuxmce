/* $Id: CorpClient.cpp,v 1.1.1.1 2004/10/05 11:36:47 radu Exp $ */

#include <string>

#include "CorpClient.h"
#include "DCE/Logger.h"
#include "PlutoServerCore.h"
#include "CorpServerDefines.h"
#include "Router.h"

using namespace ::std;

void *ReconnectThread(void *param)
{
	CorpClient *pCorpClient = (CorpClient *)param;
	
	// when should this exit?
	while(pCorpClient->m_bRun)
	{
		g_pPlutoLogger->Write(LV_CORPCLIENT, "Connecting to CorpServer....");
		if(pCorpClient->Connect())
		{
			pthread_join(pCorpClient->m_RequestHandlerThread, NULL);
		//	pCorpClient->Disconnect();
		}
		for(int i=0;i<40;i++)
		{
			if(pCorpClient->m_bRun == false)
				return NULL;
			Sleep(500);
		}
	}
	return NULL;
}


CorpClient::CorpClient(PlutoServerCore* pCore, int ServerID, string IPAddress) :
	HandleRequestSocket(ServerID, IPAddress)
{
	m_pCore = pCore;
	m_ServerID = ServerID;
	m_pEvent = NULL;
	HasConnected = false;
	m_bRun = true;
	m_ServerAddress = IPAddress;
	pthread_create(&m_ReconnectThread, NULL, ReconnectThread, (void *)this);
}

CorpClient::~CorpClient()
{
	m_bRun = false;
	Disconnect();
	pthread_join(m_ReconnectThread, NULL);
	delete m_pEvent;
	m_pEvent = NULL;
}


void CorpClient::Disconnect()
{
	if(m_pEvent)
	{
		delete m_pEvent;
		m_pEvent = NULL;
		m_pCore->m_pAlarmManager->CancelAlarmByType(ALARM_PING);
		g_pPlutoLogger->Write(LV_CORPCLIENT, "Disconnected!");
	}
	HandleRequestSocket::Disconnect();
}

bool CorpClient::OnConnect()
{
	if(!HandleRequestSocket::OnConnect())
		return false;
		
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Connected to CorpServer");
	m_pEvent = new Event_Impl(m_ServerID, m_ServerAddress);
	m_pCore->m_pAlarmManager->AddRelativeAlarm(5*60, this, ALARM_PING, NULL);

	if(HasConnected == false)
		Send_StartupEvent(CORPSERVER_STATUS_OK);
	else
		Send_StartupEvent(CORPSERVER_STATUS_RECONNECT);
	HasConnected = true;

	g_pPlutoLogger->Write(LV_CORPCLIENT, "Done sending package versions");
	return true;
}

void CorpClient::ReceivedString(string Line)
{ 
}

bool CorpClient::ReceivedMessage(Message *pMessage)
{ 
	if(pMessage->m_dwMessage_Type != MESSAGETYPE_SERVERCOMMAND)
	{
		unsigned long Size;
		char *Data;

		pMessage->ToData(Size, Data);
		m_pCore->m_pRouter->ReceivedMessage(this,new Message(Size, Data));
		delete Data;
		return true;
	}
		
		
	switch(pMessage->m_dwID)
	{
	case CORPSERVER_COMMAND_UPDATE_EPG:
		return Do_UpdateEPG(pMessage);
		
 	case CORPSERVER_COMMAND_UPDATE_CONFIG:
		return Do_UpdateConfig(pMessage);
		
	case CORPSERVER_COMMAND_UPDATE_SOFTWARE:
		return Do_UpdateSoftware(pMessage);
		
	case CORPSERVER_COMMAND_RESTART:
		return Do_Restart(pMessage);
		
	case CORPSERVER_COMMAND_SEND_LOG:
		return Do_SendLog(pMessage);
	}
	return false;
}

bool CorpClient::SendMessage(Message *pMessage)
{
	g_pPlutoLogger->Write(LV_STATUS, "CorpClient::SendMessage m_pEvent=%p",m_pEvent);
	if(m_pEvent)
	{
		m_pEvent->SendMessage(pMessage);
		return true;
	}
	return false;
}

bool CorpClient::SendString(string s)
{
	g_pPlutoLogger->Write(LV_STATUS, "CorpClient::SendString(%s)", s.c_str());
	bool ret = HandleRequestSocket::SendString(s);
	g_pPlutoLogger->Write(LV_STATUS, "CorpClient::SendString=%d", ret);
	return ret;
	
}

void CorpClient::Send_ShutdownEvent(int status)
{
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Sending Shutdown Event");
	SendMessage(new Message(m_ServerID, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, CORPSERVER_EVENT_SHUTDOWN,1,
		CORPSERVER_PARAM_STATUS, StringUtils::itos(status).c_str()));
}

void CorpClient::Send_StartupEvent(int status)
{
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Sending Startup Event");
	string ip = m_pCore->m_pRouter->GetPublicIP();
	SendMessage(new Message(m_ServerID, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, CORPSERVER_EVENT_STARTUP,2,
		CORPSERVER_PARAM_STATUS, StringUtils::itos(status).c_str(),
		CORPSERVER_PARAM_IP, ip.c_str()));
}

void CorpClient::Send_PackageVersion(string name, string version)
{
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Sending Package Version Event for %s (%s)", name.c_str(), version.c_str());
	SendMessage(new Message(m_ServerID, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, CORPSERVER_EVENT_PACKAGEVERSION,2,
		CORPSERVER_PARAM_NAME, name.c_str(),
		CORPSERVER_PARAM_VERSION, version.c_str()));

}

bool CorpClient::Do_UpdateEPG(Message* pMessage)
{
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Got UpdateEPG Command");
	SendString("OK");
	return true;
}
	
bool CorpClient::Do_Restart(Message* pMessage)
{
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Got Restart Command");
	m_pCore->m_bReload = true;
	SendString("OK");
	return true;
}

bool CorpClient::Do_UpdateConfig(Message* pMessage)
{
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Got Update Config Command");
	SendString("OK");
	return true;
}

bool CorpClient::Do_UpdateSoftware(Message* pMessage)
{
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Got Update Software Command");
	SendString("OK");
	return true;
}

bool CorpClient::Do_SendLog(Message* pMessage)
{
	Send_Log();
	return true;
}

void CorpClient::Send_Log()
{
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Got Send Log Command");
	SendString("OK");
}

void CorpClient::Send_Ping()
{
	g_pPlutoLogger->Write(LV_CORPCLIENT, "Sending Ping Event");
	string ip = m_pCore->m_pRouter->GetPublicIP();
	SendMessage(new Message(m_ServerID, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, CORPSERVER_EVENT_PING,1,
			CORPSERVER_PARAM_IP, ip.c_str()));
}

void CorpClient::AlarmCallback(int id, void* param)
{
	switch(id)
	{
		case ALARM_PING:
			Send_Ping();
			m_pCore->m_pAlarmManager->AddRelativeAlarm(5*60, this, ALARM_PING, NULL);
			break;
	}
}

