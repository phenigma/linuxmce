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
#include "SlimHandler.h"

using namespace std;
using namespace DCE;

SlimServerClient::SlimServerClient()
{
	dataThreadCommand = THREAD_STOP;
	dataThreadState = THREAD_STOPPED;

	commandThreadCommand = THREAD_STOP;
	commandThreadState = THREAD_STOPPED;

	pthread_create(&commandThread, NULL, controlConnectionThread, this);
	pthread_create(&dataThread, NULL, dataConnectionThread, this);

	m_pSlimHandler = NULL;
}

SlimServerClient::~SlimServerClient()
{
	delete m_pSlimHandler;
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
	m_pXineSlave = pXineSlaveControl;
}

void *SlimServerClient::controlConnectionThread(void *arguments)
{
	SlimServerClient::ThreadCommandType lastCommand;
	SlimServerClient *pSlimServerClient;
	int controlSocket;

	pSlimServerClient = static_cast<SlimServerClient *>(arguments);
	controlSocket = -1;
	lastCommand = THREAD_UNKNOWN;

	SlimHandler *slimProtocolHandler;

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
					pSlimServerClient->commandThreadState = THREAD_CONNECTING;
					if ( controlSocket == -1 )
						controlSocket = connect_to_host(pSlimServerClient->m_strHostname.c_str(), pSlimServerClient->m_iPort);

					if ( controlSocket <= 0 )
						break;

					slimProtocolHandler = pSlimServerClient->getSlimHandler();
					slimProtocolHandler->setConnectionSocket(controlSocket);
					if ( slimProtocolHandler->doHello() )
						pSlimServerClient->commandThreadState = THREAD_CONNECTED;
					else
					{
						g_pPlutoLogger->Write(LV_STATUS, "Hello was not succesfull");
						pSlimServerClient->commandThreadState = THREAD_STOPPED;
					}

					break;
				case THREAD_EXIT:
					g_pPlutoLogger->Write(LV_STATUS, "Ending the the command thread!");
					pSlimServerClient->commandThreadState = THREAD_EXITING;
					break;
				default:
					g_pPlutoLogger->Write(LV_STATUS, "Command not handled %d", pSlimServerClient->commandThreadCommand);
			}

			lastCommand = pSlimServerClient->commandThreadCommand;
		}

		if ( pSlimServerClient->commandThreadState == THREAD_CONNECTED )
			slimProtocolHandler->doOneCommand();

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

SlimHandler *SlimServerClient::getSlimHandler()
{
	if ( m_pSlimHandler == NULL )
		m_pSlimHandler = new SlimHandler();

	return m_pSlimHandler;
}

void SlimServerClient::setMacAddress(std::string strMacAddress)
{
	getSlimHandler()->setMacAddress(strMacAddress);
}
