// OCDeviceTemplate.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <list>
#include <sstream>


#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "RAServerSocket.h"
#include "RAServer.h"

DCE::Socket *RAServer::CreateSocket(SOCKET newsock,string Name)
{
	DCE::Socket *pSocket = new RAServerSocket(this, newsock);
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
