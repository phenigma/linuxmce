#include <iostream>
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "DCE/DCEMySqlConfig.h"

#include "RAServer.h"
#include "RAServerSocket.h"
#include "RA/RA_Request.h"
#include "RA/RA_Processor.h"
#include <iostream>

void *BeginClientThread(void *param)
{	
	RAServerSocket *pCS = (RAServerSocket *)param;
	pCS->Run();
	return NULL;
}

RAServerSocket::RAServerSocket(class RAServer *pListener, SOCKET Sock) : Socket("raserver")
{
	m_Socket = Sock;
	m_pListener = pListener;

	pthread_create(&m_ClientThreadID, NULL, BeginClientThread, (void *)this);
}

RAServerSocket::~RAServerSocket()
{
}

void RAServerSocket::Run()
{
	RA_Processor *pRA_Processor = RA_Processor::CreateRA_Processor(new DCEMySqlConfig("/etc/pluto.conf"));
	while( pRA_Processor->ReceiveRequests(this) );
	delete pRA_Processor;
	if (!SOCKFAIL(m_Socket))
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	delete this;
	return;
}
