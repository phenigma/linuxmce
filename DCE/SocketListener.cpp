#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include <fcntl.h>

#include "DCE/Logger.h"
#include "ServerSocket.h"
#include "SocketListener.h"

using namespace DCE;

void *BeginListenerThread(void *param)
{
	SocketListener *pListener = (SocketListener *)param;
	pListener->Run();
	return NULL;
}

SocketListener::SocketListener(string Name) : 
	m_ListenerMutex("listener " + Name)
{
	m_sName=Name;
	m_ListenerThreadID = 0;
	m_ListenerMutex.Init(NULL);
}

void SocketListener::StartListening(int PortNumber)
{
	m_bClosed=false;
	m_bTerminate=false;
	m_bRunning=true;
	m_ListenPort = PortNumber;
    pthread_create(&m_ListenerThreadID, NULL, BeginListenerThread, (void *)this);
}

SocketListener::~SocketListener()
{
	m_bTerminate=true;
	g_pPlutoLogger->Write(LV_SOCKET,"~SocketListener %d",m_Socket);
	if (m_Socket != INVALID_SOCKET)
	{
/*		PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);
		list<Socket *>::iterator i;

		for(i=m_Clients.begin();i!=m_Clients.end();i++)
		{
			Socket *pSocket = *i;
			if( !pSocket )
				g_pPlutoLogger->Write(LV_CRITICAL,"SocketListener::~SocketListener socket is NULL");
			else if( pSocket->m_Socket!=INVALID_SOCKET )
			{
				g_pPlutoLogger->Write(LV_SOCKET,"closing socket %p %d",pSocket,pSocket->m_Socket);
				closesocket(pSocket->m_Socket);
				pSocket->m_Socket = INVALID_SOCKET;
			}
		}
		lm.Release(); */
		g_pPlutoLogger->Write(LV_SOCKET,"closing listener socket %d",m_Socket);
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	if (m_ListenerThreadID)
		pthread_join(m_ListenerThreadID, 0);
  	pthread_mutex_destroy(&m_ListenerMutex.mutex);
}

void SocketListener::Run()
{
	//int nRetries = 0;
	struct sockaddr_in  addrT;

	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
//	setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (SOCKOPTTYPE)&b, sizeof(b));

	if ( m_Socket == INVALID_SOCKET )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Couldn't create listener socket.\r");
		m_bRunning=false;
		return;
	}
	
	int opt = 1;
	if(setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (SOCKOPTTYPE)&opt, sizeof (opt)))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "setsockopt failed");	

		//jjhuff
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		m_bRunning=false;
		return;
	}
	
	g_pPlutoLogger->Write(LV_SOCKET, "TCPIP: Listening on %d.", m_ListenPort);

	memset(&addrT, 0, sizeof(addrT));
	addrT.sin_family = AF_INET;
	addrT.sin_port = htons(m_ListenPort);
	addrT.sin_addr.s_addr = INADDR_ANY;

	if ( SOCKFAIL(bind(m_Socket, (struct sockaddr*)&addrT, sizeof(addrT))))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to bind to port.\r");
	}
	else if (SOCKFAIL(listen(m_Socket, SOMAXCONN)))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to listen to port.\r");
	}
	else
 	{
#ifdef WIN32
		unsigned long on = 1;
		::ioctlsocket(m_Socket, FIONBIO, &on);
#else
		long flags = fcntl(m_Socket, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(m_Socket, F_SETFL,flags);
#endif
		while(!m_bTerminate)
		{
			fd_set rfds;
			struct timeval tv;

			int ret = 0;
		
			while(!m_bTerminate && ret!=1)
			{
				FD_ZERO(&rfds);
				FD_SET(m_Socket, &rfds);
				tv.tv_sec = 1;
				tv.tv_usec = 0;
				fflush(stdout);
				ret = select((int) (m_Socket+1), &rfds, NULL, NULL, &tv);
			}
			if(m_bTerminate)
				break;

			struct sockaddr_in addr;
			socklen_t len = sizeof(struct sockaddr);
			SOCKET newsock = accept(m_Socket, (sockaddr *)&addr, &len);
			if (newsock != INVALID_SOCKET)
			{
				if (m_bClosed)
				{
					g_pPlutoLogger->Write(LV_STATUS, "Got an incoming connection, but the server state is closed.");
					send(newsock, "CLOSED\n", 6, 0);
  					closesocket(newsock);
				}
				else
				{
					g_pPlutoLogger->Write(LV_SOCKET, "TCPIP: Accepting incoming connection on socket %d, port %d, from IP %s.", newsock, m_ListenPort, inet_ntoa(addr.sin_addr));
#ifdef WIN32
					unsigned long on = 1;
					::ioctlsocket(newsock, FIONBIO, &on);
#else
					long flags = fcntl(newsock, F_GETFL);
					flags |= O_NONBLOCK;
					fcntl(newsock, F_SETFL,flags);
#endif
//					setsockopt(newsock, IPPROTO_TCP, TCP_NODELAY, (SOCKOPTTYPE) &b, sizeof(b));
					Socket *has = CreateSocket(newsock,"Incoming_Conn Socket " + StringUtils::itos(newsock) + " " + inet_ntoa(addr.sin_addr));
					PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);
					m_Clients.push_back(has);
					lm.Release();
				}
			}
			else
			{
				if (!m_bTerminate)
				{
					g_pPlutoLogger->Write(LV_CRITICAL, "Invalid connection socket (accept failed).\r");
					Sleep(1000);
				}
			}
			Sleep(0);
		}
	}

	//jjhuff
	closesocket(m_Socket);

	m_Socket = INVALID_SOCKET;
	m_bRunning=false;
	return;
}

Socket *SocketListener::CreateSocket(SOCKET newsock,string Name)
{
	Socket *pSocket = new ServerSocket(this, newsock,Name);
	return pSocket;
}				

void SocketListener::RegisterCommandHandler(ServerSocket *Socket, int DeviceID)
{
	PLUTO_SAFETY_LOCK(ll,m_ListenerMutex);
	Socket->m_sName+=" dev " + StringUtils::itos(DeviceID);
	Socket->m_ConnectionMutex.m_sName += " dev " + StringUtils::itos(DeviceID);
	Socket->m_SocketMutex.m_sName += " dev " + StringUtils::itos(DeviceID);
	DeviceClientMap::iterator iDC = m_mapCommandHandlers.find(DeviceID);
	if (iDC!=m_mapCommandHandlers.end())
	{
		g_pPlutoLogger->Write(LV_REGISTRATION, "!!! Replacing command handler on device ID \x1b[34;1m%d!\x1b[0m", DeviceID);
		ll.Release();
		RemoveSocket((*iDC).second);
		ll.Relock();
	}
	m_mapCommandHandlers[DeviceID] = Socket;
	Socket->SetReceiveTimeout(30);
	ll.Release();
	g_pPlutoLogger->Write(LV_REGISTRATION, "Device ID \x1b[34;1m%d's command handler\x1b[0m registered.", DeviceID);
	RegisteredCommandHandler(DeviceID);
}


string SocketListener::GetIPAddress()
{
	char buf[256];
	gethostname(buf, sizeof(buf));
	
	struct hostent *he = gethostbyname (buf);

	char* addr = he->h_addr_list[0];
	if (addr)
	{
		struct in_addr  inAddr;
		memmove (&inAddr, addr, 4);
		return string(inet_ntoa(inAddr));
	}
	return "127.0.0.1";
}

string SocketListener::GetIPAddress(int DeviceID)
{
	ServerSocket* ss = m_mapCommandHandlers[DeviceID];
	if(ss == NULL)
		return "";
		
	sockaddr_in inAddr;
	socklen_t sl = sizeof(inAddr);
	if(getpeername(ss->m_Socket, (sockaddr*)&inAddr, &sl))
		return "";
	return  string(inet_ntoa(inAddr.sin_addr));
	
}

void SocketListener::RemoveSocket(Socket *Socket)
{
	PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);

	m_Clients.remove(Socket);
	closesocket(Socket->m_Socket);

	// See if we have this socket mapped to a device ID

	ServerSocket *pServerSocket = /*dynamic_cast<*/(ServerSocket *)/*>*/(Socket);
	if (pServerSocket)
	{
		int DeviceID = pServerSocket->m_dwPK_Device;
		Socket->m_Socket = INVALID_SOCKET;
		if(DeviceID >= 0 )
		{
			DeviceClientMap::iterator i = m_mapCommandHandlers.find(DeviceID);
			if (i!=m_mapCommandHandlers.end() && (*i).second == Socket )
			{
				g_pPlutoLogger->Write(LV_REGISTRATION, "Command handler for \x1b[34;1m%d\x1b[0m closed.", DeviceID);
				OnDisconnected(DeviceID);
				m_mapCommandHandlers.erase(DeviceID);
			}
			else
			{
				g_pPlutoLogger->Write(LV_REGISTRATION, "Event connection for \x1b[34;1m%d\x1b[0m closed.", DeviceID);
			}
		}
	}
}

bool SocketListener::SendMessage(int DeviceID, Message *pMessage)
{
	DeviceClientMap::iterator i = m_mapCommandHandlers.find(DeviceID);
	if (i==m_mapCommandHandlers.end())
		return false;

	(*i).second->SendMessage(pMessage);
	return true;
}

bool SocketListener::SendString(int DeviceID, string Line)
{
	DeviceClientMap::iterator i = m_mapCommandHandlers.find(DeviceID);
	if (i==m_mapCommandHandlers.end())
		return false;

	(*i).second->SendString(Line);
	return true;
}

bool SocketListener::SendData(int DeviceID, int Length, const char *Data)
{
	DeviceClientMap::iterator i = m_mapCommandHandlers.find(DeviceID);
	if (i==m_mapCommandHandlers.end())
		return false;

	(*i).second->SendData(Length, Data);
	return true;
}

void SocketListener::FlagForRemoval(int DeviceID)
{
	PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);
/*	1-2-2004 This needs to be fixed.  See Rob's email
    for(std::list<Socket *>::iterator iSocketList = m_Clients.begin(); iSocketList != m_Clients.end(); ++iSocketList)
	{
		ServerSocket *pSocket = dynamic_cast<ServerSocket *>(*iSocketList);
		if (pSocket && pSocket->m_dwPK_Device == DeviceID)
			pSocket->m_bFlaggedForClosure = true;
	}
*/
}
