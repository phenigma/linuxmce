#include "BDCommandProcessor_Smartphone_Bluetooth.h"

#include "PlutoUtils/MultiThreadIncludes.h"
#include "DCE/Logger.h"
#include "BD/BDCommandProcessor.h"
#include "BD/BDCommand.h"

#include "VIPShared/BD_PC_ReportMyVersion.h"
#include "PlutoUtils/CommonIncludes.h"
using namespace DCE;
//------------------------------------------------------------------------------------------------------------
#include <Ws2bth.h>
#include <bthapi.h>
//------------------------------------------------------------------------------------------------------------
#define RECV_TIMEDOUT 15 //seconds
#define BLUETOOTH_PORT 19
#define VERSION "bubu_Version"
//---------------------------------------------------------------------------------------------------------
void *ProcessCommandsThread(void *p)
{
	BDCommandProcessor_Smartphone_Bluetooth* pBDCommandProcessor = (BDCommandProcessor_Smartphone_Bluetooth* )p;

	bool bImmediateCallback = false;
	while(!pBDCommandProcessor->m_bDead && pBDCommandProcessor->SendCommand(bImmediateCallback))
	{
		if(!bImmediateCallback) //nothing to process right now
		{
			timespec abstime;
			abstime.tv_sec = (long) (time(NULL) + 1);
			abstime.tv_nsec = 0;
			
			PLUTO_SAFETY_LOCK(csm, pBDCommandProcessor->m_ClientSocketMutex);

#ifdef DEBUG_BLUETOOTH
			g_pPlutoLogger->Write(LV_STATUS, "Nothing to process right now, sleeping 1 sec");
#endif
			csm.TimedCondWait(abstime);
		}
	}

	pBDCommandProcessor->m_bClientConnected = false;

	g_pPlutoLogger->Write(LV_STATUS, "Client disconnected. Waking up server thread");
	pthread_cond_broadcast(&pBDCommandProcessor->m_ServerSocketCond);

	return NULL;
}
//------------------------------------------------------------------------------------------------------------
void *ServerThread(void *p)
{
	BDCommandProcessor_Smartphone_Bluetooth *pBDCommandProcessor = (BDCommandProcessor_Smartphone_Bluetooth *)p;
	pBDCommandProcessor->m_bRunning = true;

	while(!pBDCommandProcessor->m_bDead) 
	{
		SOCKADDR_BTH sab;
		int ilen = sizeof(sab);

		pBDCommandProcessor->m_ClientSocket = accept(pBDCommandProcessor->m_ServerSocket, (SOCKADDR *)&sab, &ilen);
		if (pBDCommandProcessor->m_ClientSocket == INVALID_SOCKET) 
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Failed to accept connection, error %d", WSAGetLastError());
			continue;
		}

		pBDCommandProcessor->m_bClientConnected = true;
		g_pPlutoLogger->Write(LV_WARNING, "Client successfully connected. Connection came from %04x%08x to channel %d",
			GET_NAP(sab.btAddr), GET_SAP(sab.btAddr), sab.port);


		pBDCommandProcessor->AddCommand(new BD_PC_ReportMyVersion(string(VERSION)));
		pthread_create(&pBDCommandProcessor->m_ProcessCommandsThreadID, NULL, ProcessCommandsThread, 
			(void*)pBDCommandProcessor); 

		PLUTO_SAFETY_LOCK(ssm, pBDCommandProcessor->m_ServerSocketMutex);
		g_pPlutoLogger->Write(LV_STATUS, "Client connected. Sleeping until no client is connected.");
		ssm.CondWait();
	}

	pBDCommandProcessor->m_bRunning = false;
	return NULL;
}
//------------------------------------------------------------------------------------------------------------
BDCommandProcessor_Smartphone_Bluetooth::BDCommandProcessor_Smartphone_Bluetooth()
	: BDCommandProcessor("dummy"), m_ClientSocketMutex("client socket mutex"), 
	m_ServerSocketMutex("sever socket mutex")
{
	pthread_cond_init(&m_ClientSocketCond, NULL);
	m_ClientSocketMutex.Init(NULL,&m_ClientSocketCond);

	pthread_cond_init(&m_ServerSocketCond, NULL);
	m_ServerSocketMutex.Init(NULL,&m_ServerSocketCond);

	m_ServerSocket = 0;
	m_ClientSocket = 0;

	m_bDead = false;
	m_bClientConnected = false;
	m_bRunning = false;

	//setting the service
	BT_SetService();

	//creating the socket
	m_ServerSocket = socket (AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if(m_ServerSocket == INVALID_SOCKET) 
	{
		m_bDead = true;
		g_pPlutoLogger->Write(LV_CRITICAL, "Socket creation failed, error %d", WSAGetLastError());
		return;
	}

	//preparing bind structures
	SOCKADDR_BTH sab;
	memset (&sab, 0, sizeof(sab));
	sab.addressFamily  = AF_BTH;
	sab.port = BLUETOOTH_PORT;

	//binding to the port
	if(0 != bind(m_ServerSocket, (SOCKADDR *) &sab, sizeof(sab))) 
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Socket bind, error %d", WSAGetLastError());
		closesocket (m_ServerSocket);
		return;
	}

	//listening
	listen (m_ServerSocket, SOMAXCONN);

	g_pPlutoLogger->Write(LV_WARNING, "Ready to accept connections!");
	pthread_create(&m_ServerThreadID, NULL, ServerThread, (void*)this); 
}
//------------------------------------------------------------------------------------------------------------
BDCommandProcessor_Smartphone_Bluetooth::~BDCommandProcessor_Smartphone_Bluetooth()
{
	m_bDead = false;

	if(m_ServerSocket)
	{
		closesocket(m_ServerSocket);
		CloseHandle ((LPVOID)m_ServerSocket);
	}

	if(m_ClientSocket)
	{
		closesocket(m_ClientSocket);
		CloseHandle ((LPVOID)m_ClientSocket);
	}

	pthread_mutex_destroy(&m_ClientSocketMutex.mutex);
	pthread_mutex_destroy(&m_ServerSocketMutex.mutex);
}
//------------------------------------------------------------------------------------------------------------
bool BDCommandProcessor_Smartphone_Bluetooth::SendData(int size,const char *data)
{
	PROF_START();

	if(!m_bClientConnected)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"The client is not connected!");
		return false;
	}

#ifdef DEBUG_BLUETOOTH
	g_pPlutoLogger->Write(LV_STATUS,"Sending %d bytes of data", size);
#endif
	
	int br;
	int bytes_to_send = size;
	int bytes_sent = 0;

	while(bytes_to_send > 0 && !m_bDead)
	{
		//hehes
		br = send(m_ClientSocket, data + bytes_sent, bytes_to_send, 0);

		if(br == -1) 
		{
			g_pPlutoLogger->Write(LV_STATUS, "Failed to send %d bytes of data", size);
			m_bClientConnected = false;
			return false;
		}
		else 
		{
			bytes_sent += br;
			bytes_to_send -= br;

#ifdef DEBUG_BLUETOOTH
			g_pPlutoLogger->Write(LV_STATUS, "Sent %d bytes [total %d/%d]", br, bytes_sent, size);
#endif
		}

		if ( br == 0 || WSAGetLastError() == WSAECONNRESET ) 
		{
			g_pPlutoLogger->Write(LV_WARNING, "Connection closed");
  		    return false;
		}
	}

	if(m_bDead)
	{
		m_bClientConnected = false;
		return false;
	}

#ifdef DEBUG_BLUETOOTH
	g_pPlutoLogger->Write(LV_STATUS,"Sent %d bytes of data", size);
#endif

	PROF_STOP("Send data");

	return true;
}
//------------------------------------------------------------------------------------------------------------
char *BDCommandProcessor_Smartphone_Bluetooth::ReceiveData(int size)
{
	PROF_START();

	if(!m_bClientConnected)
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"The client is not connected!");
		return NULL;
	}

	char *buffer = (char *)malloc(size);

#ifdef DEBUG_BLUETOOTH
	g_pPlutoLogger->Write(LV_STATUS, "Ready to receive %d bytes of data", size);
#endif		

	int br = 0;
	int bytes_to_receive = size;
	int bytes_received = 0;

	while(bytes_to_receive > 0 && !m_bDead)
	{
		br = recv(m_ClientSocket, buffer + bytes_received, bytes_to_receive, 0);

		if (br==-1) 
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Failed to receive %d bytes of data", size);
			m_bClientConnected = false;
			return NULL;
		}
		else 
		{
			bytes_received += br;
			bytes_to_receive -= br;

#ifdef DEBUG_BLUETOOTH			
			g_pPlutoLogger->Write(LV_STATUS, "Received %d bytes [total %d/%d]", br, bytes_received, size);
#endif
		}

		if ( br == 0 || WSAGetLastError() == WSAECONNRESET ) 
		{
			g_pPlutoLogger->Write(LV_WARNING, "Connection closed");
  		    return NULL;
		}
	}

	if(m_bDead)
	{
		m_bClientConnected = false;
		return NULL;
	}

#ifdef DEBUG_BLUETOOTH
	g_pPlutoLogger->Write(LV_STATUS, "Received %d bytes of data", size);
#endif		


	PROF_STOP("Receive data");

	return buffer; 
}
//------------------------------------------------------------------------------------------------------------
bool BDCommandProcessor_Smartphone_Bluetooth::BT_SetService()
{
	/*
   ULONG recordHandle = 0;
   #define SDP_RECORD_SIZE   0x0000004f

   BYTE rgbSdpRecord[] = {
         0x35, 0x4d, 0x09, 0x00, 0x01, 0x35, 0x11, 0x1c,
         0x29, 0xf9, 0xc0, 0xfd, 0xbb, 0x6e, 0x47, 0x97,
         0x9f, 0xa9, 0x3e, 0xc9, 0xa8, 0x54, 0x29, 0x0c,
         0x09, 0x00, 0x04, 0x35, 0x0c, 0x35, 0x03, 0x19,
         0x01, 0x00, 0x35, 0x05, 0x19, 0x00, 0x03, 0x08,
         0x1a, 0x09, 0x00, 0x06, 0x35, 0x09, 0x09, 0x65,
         0x6e, 0x09, 0x00, 0x6a, 0x09, 0x01, 0x00, 0x09,
         0x00, 0x09, 0x35, 0x08, 0x35, 0x06, 0x19, 0x11,
         0x05, 0x09, 0x01, 0x00, 0x09, 0x01, 0x00, 0x25,
         0x06, 0x53, 0x65, 0x72, 0x69, 0x61, 0x6c
   };
   struct 
   {
      BTHNS_SETBLOB   b;
      unsigned char   uca[SDP_RECORD_SIZE];
   } bigBlob;
   ULONG ulSdpVersion = BTH_SDP_VERSION;
   bigBlob.b.pRecordHandle   = &recordHandle;
   bigBlob.b.pSdpVersion     = &ulSdpVersion;
   bigBlob.b.fSecurity       = 0;
   bigBlob.b.fOptions        = 0;
   bigBlob.b.ulRecordLength  = SDP_RECORD_SIZE;
   memcpy (bigBlob.b.pRecord, rgbSdpRecord, SDP_RECORD_SIZE);
   
   BLOB blob;
   blob.cbSize    = sizeof(BTHNS_SETBLOB) + SDP_RECORD_SIZE - 1;
   blob.pBlobData = (PBYTE) &bigBlob;

   WSAQUERYSET Service;
   memset (&Service, 0, sizeof(Service));
   Service.dwSize = sizeof(Service);
   Service.lpBlob = &blob;
   Service.dwNameSpace = NS_BTH;

   return WSASetService(&Service,RNRSERVICE_REGISTER,0) != SOCKET_ERROR;
   */
	return true;
}
//------------------------------------------------------------------------------------------------------------
void BDCommandProcessor_Smartphone_Bluetooth::AddCommand(BDCommand *pCommand)
{
	BDCommandProcessor::AddCommand(pCommand);

#ifdef DEBUG_BLUETOOTH
	g_pPlutoLogger->Write(LV_STATUS, "You got mail! Wake up and process the command, dear Mr. Socket.");
#endif

	pthread_cond_broadcast(&m_ClientSocketCond);
}
//------------------------------------------------------------------------------------------------------------