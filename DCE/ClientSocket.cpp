#include "PlutoUtils/CommonIncludes.h"	

#ifndef WIN32
#include <fcntl.h>
#endif

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "ClientSocket.h"

#define DCE_DEFAULT_PORT 3450

using namespace DCE;

ClientSocket::ClientSocket( int DeviceID, string IPAddress,string Name) : Socket(Name)
{
	m_DeviceID = DeviceID;
	m_IPAddress = IPAddress;

//	if( g_pPlutoLogger ) // This won't be created yet if this is the server logger socket
//		g_pPlutoLogger->Write(LV_SOCKET,"Created client socket %p device: %d ip: %s",this,m_DeviceID,m_IPAddress.c_str());
}

ClientSocket::~ClientSocket()
{
	
}

bool ClientSocket::Connect(string ExtraInfo)
{
	bool bSuccess = false;
	
	// Flush the socket buffer
	m_pCurInsockBuffer = NULL;

	if ( m_Socket != INVALID_SOCKET )
	{
#ifdef DEBUG
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_SOCKET,"ClientSocket::Connect - disconnecting previous socket this: %p device: %d socket %d",this,m_DeviceID,m_Socket);
		else
			cerr << "ClientSocket::Connect - disconnecting previous socket this: device: " << m_DeviceID << " Socket: " << (int) m_Socket << endl;
#endif
		Disconnect();
	}

	m_Socket= socket(AF_INET, SOCK_STREAM, 0);

		//int b = 1;
//		setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (SOCKOPTTYPE) &b, sizeof(b));
     
	if ( m_Socket!= INVALID_SOCKET )
	{
		int                 nRetries = 0, nRet;
		struct sockaddr_in  addrT;
		struct hostent*   phe;

		for (bSuccess = false;  nRetries < MAX_RETRIES && !bSuccess;  nRetries++ )
		{
 			memset(&addrT, 0, sizeof(addrT));
			addrT.sin_family = AF_INET;
			string Address;
			int Port;
			StringUtils::AddressAndPortFromString(m_IPAddress, DCE_DEFAULT_PORT, Address, Port);
			addrT.sin_port = htons(Port);

			unsigned long addr = inet_addr(Address.c_str());

			if (addr == INADDR_NONE)
			{ // server address is a name
				phe = gethostbyname(Address.c_str());
				if (!phe)
				{
#ifdef _WIN32
					int ec = WSAGetLastError();
#else
					int ec = h_errno;	
#endif 
					if( g_pPlutoLogger )
						g_pPlutoLogger->Write(LV_CRITICAL, "gethostbyname for '%s', failed, Last Error Code %d, Device: %d", Address.c_str(), ec, m_DeviceID);
					else
						cerr << "gethostbyname for " << Address << " failed, Last Error Code " << ec << " device: " << m_DeviceID << endl;

					Disconnect();
					break;
				}
				addrT.sin_addr.s_addr = *(long *)phe->h_addr; 

			}
			else
			{ // Convert nnn.nnn address to a usable one
				addrT.sin_addr.s_addr = addr; 
			}

			nRet = connect(m_Socket, (sockaddr *)&addrT, sizeof(addrT));
			if ( !SOCKFAIL(nRet)) 
			{
#ifdef _WIN32
				// TODO: inet_ntop doesn't exist in Windows
				m_MyIPAddress = "unsupported";
#else
				// Determine the local IP address
				char address[512];
				//struct sockaddr_in SocketAddr;
				socklen_t len = sizeof(struct sockaddr);
				getsockname (m_Socket, (struct sockaddr*) &addrT, &len);
				inet_ntop( AF_INET, &( addrT.sin_addr.s_addr), address, 32 );
				m_MyIPAddress = address;
#endif
				bSuccess = true;
			}
			else
			{
#ifdef _WIN32
				int ec = WSAGetLastError();
#else
				int ec = errno;
#endif
				if( g_pPlutoLogger )
					g_pPlutoLogger->Write(LV_CRITICAL, "Connect() failed, Error Code %d", ec);
				else
					cerr << "Connect() failed, Error Code" << ec << endl;
			}
			
		}
		if ( !bSuccess)
		{
			if( g_pPlutoLogger )
				g_pPlutoLogger->Write(LV_CRITICAL, "ClientSocket::Connect() not successful");
			else
				cerr << "ClientSocket::Connect() not successful" << endl;
			Disconnect();
		}
	}
	else
	{
			if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL, "socket() failed, is the TCP stack initialized?, device: %d", m_DeviceID);
			else
				cerr << "socket() failed, is the TCP stack initialized?, device: " << m_DeviceID << endl;
	}
	if (bSuccess)
	{
//		setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));
#ifdef WIN32
		unsigned long on = 1;
		::ioctlsocket(m_Socket, FIONBIO, &on);
#else
		//int on = 1;
		long flags = fcntl(m_Socket, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(m_Socket, F_SETFL,flags);
#endif
		OnConnect(ExtraInfo);
	}
	return bSuccess;    
}

void ClientSocket::Disconnect()
{
	if ( m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket= INVALID_SOCKET;
	}
}

void *WatchDogThread(void *pData)
{
	ClientSocket *pClientSocket = (ClientSocket *)pData;
	while(!pClientSocket->m_bStopWatchdog && clock() < pClientSocket->m_clockTimeout)
	{
		Sleep(10);
	}
	if (clock() >= pClientSocket->m_clockTimeout)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Connection timed out");
#ifdef _WIN32
		::MessageBeep(MB_ICONEXCLAMATION);
#endif
		pClientSocket->Disconnect();
	}
	pClientSocket->m_bWatchdogRunning = false;
	return NULL;
}

void ClientSocket::StopWatchDog()
{
    if (m_pThread!=0)
    {
        m_bStopWatchdog = true;
        pthread_join(m_pThread, NULL);
        m_pThread = 0;
    }
}

void ClientSocket::StartWatchDog(clock_t Timeout)
{
	m_bStopWatchdog = false;
	m_clockTimeout = clock()+Timeout;
	if (m_bWatchdogRunning)
	{
		return;
	}
	m_bWatchdogRunning = true;
    pthread_create(&m_pThread, NULL, WatchDogThread, (void *)this);
    Sleep(1);
}

bool ClientSocket::OnConnect(string ExtraInfo)
{
	SendString("HELLO "+StringUtils::itos(m_DeviceID) + "," + ExtraInfo);
	string Response;
	if (!ReceiveString(Response))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Lost connection device: %d", m_DeviceID);
	}
	else if (Response!="OK")
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Connection for client socket reported %s, device %d", Response.c_str(), m_DeviceID);
		SendString("CORRUPT SOCKET");
		Sleep(500);
		Disconnect();
	}
	else
	{
		int k=2;
	}
	return true;
}



