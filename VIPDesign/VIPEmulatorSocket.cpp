
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

#include "DCE/MyStl.h"
#include "VIPEmulator.h"
#include "VIPEmulatorSocket.h"
#include "VIPShared/RequestProcessor.h"
#include "VIPShared/VIPRequest.h"
#include "VIPShared/VR_UnknownRequest.h"
#include "VIPShared/VR_RequestPayment.h"
#include "VIPShared/VIPAction.h"
#include "VIPShared/BTCommandProcessor_Windows_Socket.h"
#include "VIPShared/PlutoConfig.h"
#include <iostream>

#include "VIPShared/BT_PC_KeyWasPressed.h"

static const char *HTTPHeader = "HTTP/1.1 200 OK\nMax-Age: 0\nExpires: -1\n"
	    	"Cache-control: no-cache\nCache-control: private\nPragma: no-cache\n";
	
void *BeginClientThread(void *param)
{	
	VIPEmulatorSocket *pCS = (VIPEmulatorSocket *)param;
	pCS->Run();
	return NULL;
}

VIPEmulatorSocket::VIPEmulatorSocket(VIPEmulator *pListener, SOCKET Sock) : 
	DCESocket("VIPEmulator")
{
	m_DeviceID = -1;
	m_Socket = Sock;
	m_pListener = pListener;

	pthread_create(&m_ClientThreadID, NULL, BeginClientThread, (void *)this);
}

VIPEmulatorSocket::~VIPEmulatorSocket()
{
}

void VIPEmulatorSocket::Run()
{
	BTCommandProcessor_Windows_Socket *pProcessor = new BTCommandProcessor_Windows_Socket("",this);

	pthread_mutex_lock(&pProcessor->m_PollingMutex.mutex);
	while(true)
	{
		if( pProcessor->MYSTL_SIZEOF_LIST(m_listCommands)==0 && pProcessor->m_pCommand_Sent==NULL )
		{
int a=EINVAL;
int b=EFAULT;
int c=ETIMEDOUT;

			timespec timeout;
			timeout.tv_sec = 500000; 
	g_pPlutoConfig->m_iPollingInterval = 2000000000;
			timeout.tv_nsec = 0; g_pPlutoConfig->m_iPollingInterval;
	//			pthread_cond_wait(&pProcessor->m_PollingCond,&pProcessor->m_PollingMutex.mutex);
			int ret = pthread_cond_timedwait(&pProcessor->m_PollingCond,&pProcessor->m_PollingMutex.mutex,&timeout);
Sleep(500); // This is screwed up, it doesn't matter if it's seconds, ms, nanoseconds, the cond_timedwait always returns immediately
int k=2;
		}

		// We are holding the mutex
		pProcessor->SendCommand();
Sleep(25); // Somehow designer is still tying up all the CPU time????
//		pthread_mutex_lock(&pProcessor->m_PollingMutex.mutex);
	}

   	if (!SOCKFAIL(m_Socket))
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	delete this;
	return;
}


string VIPEmulatorSocket::MakeUrl(map<long,string>::iterator &i, string Page)
{
	return "<a href=\"/"+Page+"_"+StringUtils::itos((*i).first)+".htm\">"+(*i).second+"</a>";
}
