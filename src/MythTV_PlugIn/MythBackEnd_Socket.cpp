 /**
 *
 * @file MythBackEnd_Socket.cpp
 * @brief source file for the MythTvStream, MythBackEnd_Socket classes
 *
 */

#include "MythBackEnd_Socket.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#define MYTH_PROTOCOL	31
#include "Gen_Devices/AllCommandsRequests.h"

MythBackEnd_Socket::MythBackEnd_Socket(string sIPAddress)
: ClientSocket(0, /*sIPAddress*/ "192.168.80.1:6543", "MythBackend")
{
	m_bConnected=false;
}

bool MythBackEnd_Socket::SendMythString(string sValue,string *sResponse)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
	if( InternalSendMythString(sValue,sResponse) )
		return true;

	bool bResult = Connect();
	if( bResult )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythBackEnd_Socket::SendMythString failed -- reconnected");
		return InternalSendMythString(sValue,sResponse);
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString failed to reconnect");
	return false;
}

bool MythBackEnd_Socket::InternalSendMythString(string sValue,string *sResponse)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
	if( (!m_bConnected || m_Socket == INVALID_SOCKET) && !Connect() )
		return false;

	// There might be stray stuff left in the buffer
	PurgeSocketBuffer();

	// Send our string
	char szSize[20];
	sprintf(szSize,"%d          ",sValue.size());
	szSize[8]=0;
	if( !SendData(8,szSize) || !SendData(sValue.size(),sValue.c_str()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket cannot send");
		Close();
		return false;
	}

LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString send-%s: %s%s", sResponse==NULL ? "N" : "Y",szSize,sValue.c_str());

	// The user may not care about the response
	if( sResponse==NULL )
		return true;

	// Get the size of the response
	char pcData[1000];
	if( !ReceiveData( 8, pcData, 5 ) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString cannot receive");
		Close();
		return false;
	}

	// Get the size of the response
	size_t Size = atoi(pcData);
	if( Size<1 || Size>100000 ) // Arbitrary upper limit
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString invalid size %d",(int) Size);
		Close();
		return false;
	}
	char *pcBuffer = new char[Size+1];
    if( !ReceiveData( Size, pcBuffer, 5 ) )
	{
		delete pcBuffer;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString failed to read %d",(int) Size);
		Close();
		return false;
	}

	pcBuffer[Size]=0;
	*sResponse = pcBuffer;
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket receive: %s", sResponse->c_str());
	delete[] pcBuffer;
	return true;
}

bool MythBackEnd_Socket::SendMythStringGetOk(string sValue)
{
	string sResponse;
	if( !SendMythString(sValue,&sResponse) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythStringGetOk couldn't send");
		return false;
	}

	if( sResponse!="OK" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythStringGetOk");
		Close();
		return false;
	}
	return true;
}

bool MythBackEnd_Socket::Connect( int PK_DeviceTemplate,string sExtraInfo )
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythBackEnd_Socket::Connect m_bConnected %d m_Socket %d",
		(int) m_bConnected, (int) m_Socket);
	if( m_bConnected && m_Socket != INVALID_SOCKET )
		return true;

	if( ClientSocket::Connect(0,"")==false )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect failed m_bConnected=false");
		m_bConnected=false;
		return false;
	}

	m_bConnected=true;

	string sResponse;
	if( !InternalSendMythString("MYTH_PROTO_VERSION " TOSTRING(MYTH_PROTOCOL),&sResponse) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect couldn't send MYTH_PROTO_VERSION m_bConnected=false");
		Close();
		m_bConnected=false;
		return false;
	}

	if( StringUtils::StartsWith(sResponse,"ACCEPT")==false )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect sent MYTH_PROTO_VERSION: m_bConnected=false got %s", sResponse.c_str());
		Close();
		m_bConnected=false;
		return false;
	}

	sResponse="";
	if( !InternalSendMythString("ANN Playback mythtv_plugin 01",&sResponse) || sResponse!="OK" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect couldn't send ANN m_bConnected=false");
		Close();
		m_bConnected=false;
		return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect m_bConnected=true");
	return true;
}


void MythBackEnd_Socket::PurgeSocketBuffer()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
	if( m_Socket == INVALID_SOCKET )
		return;

	fd_set rfds;
	struct timeval tv;
	tv.tv_sec=tv.tv_usec=0;
	FD_ZERO(&rfds);
	FD_SET(m_Socket, &rfds);

	while( true )
	{
		int iRet = select((int) (m_Socket+1), &rfds, NULL, NULL, &tv);
		if( iRet<1 )
			return;

		char pcBuff[10];
		bool bRet = true;
		int iResult = recv( m_Socket, pcBuff, 9, 0 );
		if( iResult<1 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket purge socket is dead");
			return;
		}
pcBuff[9]=0;
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket purge: %s", pcBuff);
	}
}

void MythBackEnd_Socket::Close()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sSM,m_SocketMutex);  // don't log anything but failures
	m_bConnected=false;
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Close m_bConnected=false");
	ClientSocket::Close();
	m_bCancelSocketOp = false;  // Normally sockets aren't re-usable.  We'll reuse this one
}

