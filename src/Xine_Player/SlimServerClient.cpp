//
// C++ Implementation: SlimServerClient implementation for the Xine Player DCE Wrapper.
//
// Description:
//
//
// Author: Toader Mihai Claudiu mtoader@gmail.com
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "DCE/Logger.h"
#include "SlimServerClient.h"

#include "NetworkUtils.h"

using namespace std;
using namespace DCE;

SlimServerClient::SlimServerClient()
{
	dataThreadCommand = THREAD_STOP;
	commandThreadCommand = THREAD_STOP;

	pthread_create(&commandThread, NULL, controlConnectionThread, this);
	pthread_create(&dataThread, NULL, dataConnectionThread, this);
}

SlimServerClient::~SlimServerClient()
{
	disconnectFromServer(0); // TODO: When taking in consideration the streamIDs make sure that we loop thru them.
}

bool SlimServerClient::connectToServer(string serverUrl, int controlledStreamID)
{
	// ignore the controlledStreamID for now
	// TODO: use this in the future if we need it. Or remove it alltogether


	if ( ! parse_server_address(serverUrl) )
		return false;

	commandThreadCommand = THREAD_START; // signal the thread to start processing

	return true;
}

bool SlimServerClient::isConnecting(int iStreamID)
{
	return commandThreadCommand == THREAD_START && commandThreadState != THREAD_CONNECTED;
}

bool SlimServerClient::isConnected(int iStreamID)
{
	return commandThreadState == THREAD_CONNECTED;
}

void SlimServerClient::disconnectFromServer(int iStreamID)
{
	if ( ! isConnected(iStreamID) )
		return;

	pthread_join(commandThread, NULL);
}

void SlimServerClient::setXineSlaveObject(class XineSlaveWrapper *pXineSlaveControl)
{
	m_pXine = pXineSlaveControl;
}

void *SlimServerClient::controlConnectionThread(void *arguments)
{
	SlimServerClient::ThreadCommandType lastCommand;
	SlimServerClient *pSlimServerClient;
	int controlSocket;

	pSlimServerClient = static_cast<SlimServerClient *>(arguments);
	controlSocket = -1;

	while ( pSlimServerClient->commandThreadCommand != THREAD_EXIT )
	{
		if ( pSlimServerClient->commandThreadCommand != lastCommand )
		{
			switch( pSlimServerClient->commandThreadCommand )
			{
				case THREAD_STOP:
					g_pPlutoLogger->Write(LV_STATUS, "Pausing the command thread!");
					pSlimServerClient->commandThreadState = THREAD_STOPPED;
					break;
				case THREAD_START:
					g_pPlutoLogger->Write(LV_STATUS, "Starting the command thread!");

					// assume connection failure. Only reset it when we know for sure that we are connected
					pSlimServerClient->commandThreadState = THREAD_STOPPED;
					if ( controlSocket == -1 )
						controlSocket = connect_to_host(pSlimServerClient->m_strHostname.c_str(), pSlimServerClient->m_iPort);

					if ( controlSocket <= 0 )
						break;

					if ( pSlimServerClient->slim_do_hello(controlSocket) )
						pSlimServerClient->commandThreadState = THREAD_CONNECTED;

					break;
				case THREAD_EXIT:
					g_pPlutoLogger->Write(LV_STATUS, "Starting the command thread!");
					pSlimServerClient->commandThreadState = THREAD_EXITING;
					break;
			}

			lastCommand = pSlimServerClient->commandThreadCommand;
		}

// 		if ( pSlimServerClient->commandThreadState == THREAD_CONNECTED )
// 			g_pPlutoLogger->Write(LV_STATUS, "Processing commands from the slim server");

		Sleep(100); // 10 commands per ssecond. It should be enough.
	}

	return NULL;
}

void *SlimServerClient::dataConnectionThread(void *arguments)
{
	return NULL;
}

bool SlimServerClient::parse_server_address(string strConnectionString)
{
	// helpers in parsing the URL
	const char *pChar, *pStartPortNumber;
	char *pPortNumberAsString; // this is used only when we have a port number specified.


	const char *pConnectionString = strConnectionString.c_str();

	pChar = pConnectionString;

	while ( *pChar && *pChar != ':' && *pChar != '/' ) pChar++;

	if ( pChar == pConnectionString )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Could not parse the hostname from URL: %s!", pConnectionString);
		return false;
	}

	m_strHostname = string(pConnectionString, pChar - pConnectionString);
	m_iPort = 3483; // the default slim protocol port.

	if ( *pChar == ':' )
	{
		pStartPortNumber = ++pChar;
		while ( *pChar && ('0' <= *pChar && *pChar <= '9') ) pChar++;

		if ( pChar != pStartPortNumber )
		{
			pPortNumberAsString = new char[pChar - pStartPortNumber + 1];

			strncpy(pPortNumberAsString, pStartPortNumber, pChar - pStartPortNumber);
			pPortNumberAsString[pChar - pStartPortNumber] = 0;

			m_iPort = atoi(pPortNumberAsString);

			delete pPortNumberAsString;
		}
	}

	return true;
}

char commandBuffer[1024];

bool SlimServerClient::slim_do_hello(int connection)
{
	strncpy(commandBuffer, "HELO", 4);
	commandBuffer[4] = 0;
	commandBuffer[5] = 0;
	commandBuffer[6] = 0;
	commandBuffer[7] = 10;

	commandBuffer[8] = 3;
	commandBuffer[9] = 0;

	// g_pPlutoLogger->Write(LV_STATUS, "Got this mac address: %s", m_pXine->GetMacAddress().c_str());

	commandBuffer[10] = m_macAddress[0];
	commandBuffer[11] = m_macAddress[1];
	commandBuffer[12] = m_macAddress[2];
	commandBuffer[13] = m_macAddress[3];
	commandBuffer[14] = m_macAddress[4];
	commandBuffer[15] = m_macAddress[5];
	commandBuffer[16] = 0x00;
	commandBuffer[17] = 0x00;

// 	*len  = 4 /*HELO*/ + 4 /* payload len */ + 1 /* device ID */ +
// 			1 /* device ID */ + 6 + /* mac data */ + 2 /* flags */;
// 	*outBuffer = (char*)xine_xmalloc(*len);
// 	buffer = *outBuffer;
//
// 	strncpy(buffer, "HELO", 4);

	return send (connection, commandBuffer, 18, 0) == 18;
}

void SlimServerClient::setMacAddress(string strMacAddress)
{
	const char *startChar;
	char *endChar;

	unsigned int index, value;

	index = 0;
	startChar = strMacAddress.c_str();
	while ( *endChar != 0 )
	{
		value = strtol(startChar, &endChar, 16);
		m_macAddress[index++] = value & 0xFF;
		if ( *endChar )
			startChar = ++endChar;
	}
}