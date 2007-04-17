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
#include "MythTV_PlugIn.h"
#include "../Media_Plugin/Media_Plugin.h"

#include <iostream>
using namespace std;
using namespace DCE;

#define MYTH_PROTOCOL	31
#include "Gen_Devices/AllCommandsRequests.h"

MythBackEnd_Socket::MythBackEnd_Socket(MythTV_PlugIn *pMythTV_PlugIn,string sIPAddress)
{
	m_pSocketBuffer = new char[MYTH_SOCKET_BUFFER_SIZE];
	m_pMythTV_PlugIn = pMythTV_PlugIn;
	m_sIPAddress = sIPAddress;
	m_bConnected=false;
	m_pSocket = NULL;
}

MythBackEnd_Socket::~MythBackEnd_Socket()
{
	delete m_pSocket;
	delete m_pSocketBuffer;
}

bool MythBackEnd_Socket::SendMythString(string sValue,string *sResponse)
{
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	if( InternalSendMythString(sValue,sResponse) )
		return true;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythBackEnd_Socket::SendMythString trying to reconnect");

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
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	if( (!m_bConnected || m_pSocket==NULL || m_pSocket->m_Socket == INVALID_SOCKET) && !Connect() )
		return false;

	// There might be stray stuff left in the buffer
	PurgeSocketBuffer();

	// Send our string
	char szSize[20];
	sprintf(szSize,"%d          ",sValue.size());
	szSize[8]=0;
	if( !m_pSocket->SendData(8,szSize) || !m_pSocket->SendData(sValue.size(),sValue.c_str()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket cannot send %s", sValue.c_str());
		DeleteSocket();
		return false;
	}

LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString send-%s: %s%s", sResponse==NULL ? "N" : "Y",szSize,sValue.c_str());

	// The user may not care about the response
	if( sResponse==NULL )
		return true;

	// Get the size of the response
	char pcData[1000];
	if( !m_pSocket->ReceiveData( 8, pcData, 5 ) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString cannot receive");
		DeleteSocket();
		return false;
	}

	// Get the size of the response
	size_t Size = atoi(pcData);
	if( Size<1 || Size>100000 ) // Arbitrary upper limit
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString invalid size %d",(int) Size);
		DeleteSocket();
		return false;
	}
	char *pcBuffer = new char[Size+1];
    if( !m_pSocket->ReceiveData( Size, pcBuffer, 5 ) )
	{
		delete pcBuffer;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString failed to read %d",(int) Size);
		DeleteSocket();
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
		DeleteSocket();
		return false;
	}

	if( sResponse!="OK" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythStringGetOk");
		DeleteSocket();
		return false;
	}
	return true;
}

bool MythBackEnd_Socket::Connect( )
{
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythBackEnd_Socket::Connect m_bConnected %d m_Socket %p",
		(int) m_bConnected, m_pSocket);

	if( m_bConnected && m_pSocket && m_pSocket->m_Socket != INVALID_SOCKET )
		return true;

	if( m_pSocket==NULL || m_pSocket->m_Socket == INVALID_SOCKET )
	{
		if( m_pSocket )
			delete m_pSocket;
		m_pSocket = new MythBackEnd_Socket_Wrapper(this,m_sIPAddress + ":6543");
		if( m_pSocket->Connect(0,"")==false )
		{
			DeleteSocket();
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect failed m_bConnected=false");
			return false;
		}
	}

	m_bConnected=true;

	string sResponse;
	if( !InternalSendMythString("MYTH_PROTO_VERSION " TOSTRING(MYTH_PROTOCOL),&sResponse) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect couldn't send MYTH_PROTO_VERSION m_bConnected=false");
		DeleteSocket();
		return false;
	}

	if( StringUtils::StartsWith(sResponse,"ACCEPT")==false )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect sent MYTH_PROTO_VERSION: m_bConnected=false got %s", sResponse.c_str());
		DeleteSocket();
		return false;
	}

	sResponse="";
	if( !InternalSendMythString("ANN Playback mythtv_plugin 01",&sResponse) || sResponse!="OK" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect couldn't send ANN m_bConnected=false");
		DeleteSocket();
		return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect m_bConnected=true");
	return true;
}


void MythBackEnd_Socket::PurgeSocketBuffer()
{
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	if( !m_pSocket || m_pSocket->m_Socket == INVALID_SOCKET )
		return;

	fd_set rfds;
	struct timeval tv;
	tv.tv_sec=tv.tv_usec=0;
	FD_ZERO(&rfds);
	FD_SET(m_pSocket->m_Socket, &rfds);

	while( true )
	{
		int iRet = select((int) (m_pSocket->m_Socket+1), &rfds, NULL, NULL, &tv);
		if( iRet<1 )
			return;

		char pcBuff[10];
		bool bRet = true;
		int iResult = recv( m_pSocket->m_Socket, pcBuff, 9, 0 );
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
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Close");
	m_bConnected=false;
}

void MythBackEnd_Socket_Wrapper::Close()
{ 
	m_pMythBackEnd_Socket->Close(); 
	ClientSocket::Close(); 
}
