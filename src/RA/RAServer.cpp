// OCDeviceTemplate.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <list>
#include <sstream>
#include <assert.h>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "RAServerSocket.h"
#include "RAServer.h"

RAServer::RAServer() : SocketListener("RAServer")
{
}

RAServer::~RAServer()
{
}

DCE::Socket *RAServer::CreateSocket( SOCKET newsock, string sName, string sIPAddress )
{
	DCE::Socket *pSocket = new RAServerSocket(this, newsock);
	pSocket->m_sIPAddress = sIPAddress;
	return pSocket;
}

void RAServer::Initialize()
{
}


void RAServer::Run()
{
	if (m_ListenerThreadID)
		pthread_join(m_ListenerThreadID, NULL);
}
