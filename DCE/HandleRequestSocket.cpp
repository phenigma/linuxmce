#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Socket.h"
#include "Message.h"
#include "HandleRequestSocket.h"

using namespace DCE;

void *BeginHandleRequestThread(void *param)
{
	HandleRequestSocket *pCS = (HandleRequestSocket *)param;
	pCS->RunThread();
	return NULL;
}

bool HandleRequestSocket::OnConnect(string ExtraInfo)
{
	SendString("REQUESTHANDLER "+StringUtils::itos(m_DeviceID));
	string Response;
	if (!ReceiveString(Response))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Requesthandler %p (device: %d) lost connection. %s",this,m_DeviceID,m_sName.c_str());
		return false;
	}
	if (Response=="CLOSED")
	{
		return false;
	}
	if (Response!="OK")
	{
#ifdef LOG_ALL_CONTROLLER_ACTIVITY
		LACA_B4_4("Connection for requesthandler %p (device: %d) reported %s. %s", this,m_DeviceID,Response.c_str(),m_sName.c_str());
#endif
		g_pPlutoLogger->Write(LV_CRITICAL, "Connection for requesthandler %p (device: %d) reported %s. %s", this,m_DeviceID,Response.c_str(),m_sName.c_str());
		return false;
	}
	m_bRunning=true;
	m_bTerminate=false;
    pthread_create(&m_RequestHandlerThread, NULL, BeginHandleRequestThread, (void *)this);
	return true;
}

void HandleRequestSocket::Disconnect()
{
	// We override Disconnect so our processor thread knows the difference
	// between an explicit disconnect and an unexpected one.

	g_pPlutoLogger->Write(LV_SOCKET, "RequestSocket::Disconnect %p device: %d", 
		this,m_DeviceID);

	m_bTerminate=true;
	ClientSocket::Disconnect();
}

void HandleRequestSocket::OnUnexpectedDisconnect()
{
}


HandleRequestSocket::HandleRequestSocket(int DeviceID, string IPAddress, string Name) : 
	ClientSocket(DeviceID, IPAddress, Name)
{
	m_RequestHandlerThread = 0;
//	g_pPlutoLogger->Write(LV_SOCKET,"Created handle request socket %p device: %d ip: %s",this,DeviceID,IPAddress.c_str());
}

HandleRequestSocket::~HandleRequestSocket()
{
	DisconnectAndWait();
}

void HandleRequestSocket::DisconnectAndWait()
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_SOCKET,"~HandleRequestSocket %p device: %d ip: %s",this,m_DeviceID,m_IPAddress.c_str());
#endif
	Disconnect();
	if (m_RequestHandlerThread)
	{
		pthread_join(m_RequestHandlerThread, NULL);
		m_RequestHandlerThread = 0;
	}
}

void HandleRequestSocket::RunThread()
{
#ifdef UNDER_CE
	__try
	{
#endif

	m_bUnexpected=false;
	string msg="";

	while(!m_bTerminate)
	{
		if (!ReceiveString(msg))
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Receive string failed in HandleRequestSocket %d:%s %s",msg.size(),msg.c_str(),m_sName.c_str());
			if (m_bTerminate==false)
			{
				PlutoLock::DumpOutstandingLocks();
				m_bUnexpected=true;
				m_bTerminate=true;
			}
		}
		else
		{
			if (msg=="CORRUPT SOCKET")
			{
				g_pPlutoLogger->Write(LV_STATUS, "Socket flagged as corrupted %p device: %d",this,m_DeviceID);
				m_bTerminate = true;
			}
			if (msg.substr(0,4)=="PING")
			{
#ifdef DEBUG
#ifdef WINDOWS
				LACA_B4_0("got ping, sending pong %s",m_sName.c_str());
				g_pPlutoLogger->Write(LV_STATUS, "got ping, sending pong %p device: %d %s",this,m_DeviceID,m_sName.c_str());
#endif
#endif
				SendString("PONG");
			}
			else
			{
#ifdef DEBUG
				g_pPlutoLogger->Write(LV_STATUS, "Received %s %p device: %d",msg.c_str(),this,m_DeviceID);
#endif
				if (msg.substr(0,7)  == "MESSAGE")	
				{
					Message *pMessage = ReceiveMessage(atoi(msg.substr(8).c_str()));
					if (pMessage)
					{
#ifdef DEBUG
						g_pPlutoLogger->Write(LV_STATUS, "Received Message type %d ID %d from %d to %d (device: %d)",
							pMessage->m_MessageType,pMessage->m_ID,pMessage->m_DeviceIDFrom,pMessage->m_DeviceIDTo,m_DeviceID);
#endif
						if (!ReceivedMessage(pMessage))
						{
							#ifdef LOG_ALL_CONTROLLER_ACTIVITY
								LACA_B4_6("Could not find a handler for message - from %d to %d Type: %d ID: %d (device: %d) %s",
									pMessage->m_DeviceIDFrom,pMessage->m_DeviceIDTo,
									pMessage->m_MessageType,pMessage->m_ID,m_DeviceID,m_sName.c_str());
							#endif
							g_pPlutoLogger->Write(LV_STATUS,"Could not find a handler for message - from %d to %d Type: %d ID: %d (device: %d) %s",
								pMessage->m_DeviceIDFrom,pMessage->m_DeviceIDTo,
								pMessage->m_MessageType,pMessage->m_ID,m_DeviceID,m_sName.c_str());
							SendString("UNHANDLED");
						}					
						delete pMessage;
					}
					else
					{
						#ifdef LOG_ALL_CONTROLLER_ACTIVITY
							LACA_B4_3("Incomplete message received %p (device: %d) %s",this,m_DeviceID,m_sName.c_str());
						#endif
						g_pPlutoLogger->Write(LV_CRITICAL,"Incomplete message received %p (device: %d) %s",this,m_DeviceID,m_sName.c_str());
					}
				}
				else
					ReceivedString(msg);
			}
		}
	}
	m_bRunning=false;
	g_pPlutoLogger->Write(LV_STATUS, "Closing event handler connection %d (%d,%s), Terminate: %d %s\n", 
		m_DeviceID,(int) m_bUnexpected,msg.c_str(),(int) m_bTerminate,m_sName.c_str());
	if (m_bUnexpected)
	{
		OnUnexpectedDisconnect();
	}
#ifdef UNDER_CE
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		::PostQuitMessage(255);
		
		PROCESS_INFORMATION pi;
		TCHAR tfn[81];
		::GetModuleFileName(NULL, tfn, sizeof(tfn));	
		CreateProcess(tfn, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, &pi);
	}
#endif
	return;
}

