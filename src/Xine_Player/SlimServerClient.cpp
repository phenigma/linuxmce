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
#include "SlimCommandHandler.h"
#include "SlimDataHandler.h"
#include "XineSlaveWrapper.h"

#include <assert.h>

using namespace std;
using namespace DCE;

SlimServerClient::SlimServerClient()
{
	commandConnectionState = CommandConnState::NOT_CONNECTED;
	dataConnectionState = DataConnState::NOT_CONNECTED;

	m_pSlimCommandHandler = NULL;
	m_pSlimDataHandler = NULL;
	m_pDataReaderThread = NULL;
}

SlimServerClient::~SlimServerClient()
{
	delete m_pSlimCommandHandler;
	delete m_pSlimDataHandler;
	delete m_pDataReaderThread;

	// TODO: When taking in consideration the streamIDs make sure that we loop thru them.
	disconnectFromServer(0);
}

bool SlimServerClient::connectToServer(string serverUrl, int controlledStreamID)
{
	if ( ! parse_server_address(serverUrl) )
		return false;

	SlimCommandHandler *pCommandHandler = getSlimCommandHandler();

	// the command handler should exist all the time. If not this is a programming error
	assert(pCommandHandler);

	commandConnectionState = CommandConnState::CONNECTING;

	pCommandHandler->setTargetAddress(m_strHostname, m_iPort);

	if ( ! pCommandHandler->openCommunication() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Command socket connect was not succesfull");
		commandConnectionState = CommandConnState::NOT_CONNECTED;
	}
	else if ( ! pCommandHandler->doHello() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Hello was not succesfull");
		commandConnectionState = CommandConnState::NOT_CONNECTED;
	}
	else
	{
		pCommandHandler->doIR(1, 1, 0x0000f701);
		commandConnectionState = CommandConnState::CONNECTED;
	}

	getDataReaderThread()->addSocketOperationListenerForSocket(pCommandHandler->getCommunicationSocket(), pCommandHandler);
	return true;
}

bool SlimServerClient::isConnected(int iStreamID)
{
	return commandConnectionState == CommandConnState::CONNECTED;
}

void SlimServerClient::disconnectFromServer(int iStreamID)
{
	g_pPlutoLogger->Write(LV_STATUS, "SlimServerClient::disconnectFromServer() Called for stream %d", iStreamID);

	if ( ! isConnected(iStreamID) )
		return;

	if ( getSlimDataHandler()->isCommunicationOpen() )
		getSlimDataHandler()->closeConnection();

	if  (getSlimCommandHandler()->isCommunicationOpen() )
		getSlimCommandHandler()->closeConnection();
}

void SlimServerClient::setXineSlaveMutex(pluto_pthread_mutex_t *pXineSlaveMutex)
{
	m_pXineSlaveMutex = pXineSlaveMutex;
}

void SlimServerClient::setXineSlaveObject(class XineSlaveWrapper *pXineSlaveControl)

{
	m_pXineSlave = pXineSlaveControl;
}

/*void *SlimServerClient::controlConnectionThread(void *arguments)
{
	SlimServerClient::ThreadCommandType lastCommand;
	SlimServerClient *pSlimServerClient;
	int controlSocket;

	pSlimServerClient = static_cast<SlimServerClient *>(arguments);
	controlSocket = -1;
	lastCommand = THREAD_UNKNOWN;

	SlimCommandHandler *slimCommandHandler = pSlimServerClient->getSlimCommandHandler();

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

					slimCommandHandler->setTargetAddress(pSlimServerClient->m_strHostname, pSlimServerClient->m_iPort);
					if ( ! slimCommandHandler->openCommunication() )
					{
						g_pPlutoLogger->Write(LV_STATUS, "Command socket connect was not succesfull");
						pSlimServerClient->commandThreadState = THREAD_STOPPED;
					}
					else if ( ! slimCommandHandler->doHello() )
					{
						g_pPlutoLogger->Write(LV_STATUS, "Hello was not succesfull");
						pSlimServerClient->commandThreadState = THREAD_STOPPED;
					}
					else
					{
						slimCommandHandler->doIR(1, 1, 0x0000f701);
						pSlimServerClient->commandThreadState = THREAD_CONNECTED;
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
			slimCommandHandler->doOneCommand();
	}

	return NULL;
}
*/
// void *SlimServerClient::dataConnectionThread(void *arguments)
// {
// 	SlimServerClient::ThreadCommandType lastCommand;
// 	SlimServerClient *pSlimServerClient;
// 	int controlSocket;
//
// 	pSlimServerClient = static_cast<SlimServerClient *>(arguments);
// 	controlSocket = -1;
// 	lastCommand = THREAD_UNKNOWN;
//
// 	SlimDataHandler *slimDataHandler = pSlimServerClient->getSlimDataHandler();
// 	SlimCommandHandler *slimCommandHandler = pSlimServerClient->getSlimCommandHandler();
//
// 	while ( pSlimServerClient->dataThreadCommand != THREAD_EXIT )
// 	{
// 		if ( pSlimServerClient->dataThreadCommand != lastCommand )
// 		{
// 			g_pPlutoLogger->Write(LV_STATUS, "SlimServerClient::dataConnectionThread() Processing command %d!", pSlimServerClient->dataThreadCommand);
// 			switch( pSlimServerClient->dataThreadCommand )
// 			{
// 				case THREAD_STOP:
// 					g_pPlutoLogger->Write(LV_STATUS, "SlimServerClient::dataConnectionThread() Pausing the data thread!");
// 					pSlimServerClient->dataThreadState = THREAD_STOPPED;
// 					break;
//
// 				case THREAD_START:
// 					if ( pSlimServerClient->dataThreadState == THREAD_CONNECTED )
// 						break;
//
// 					g_pPlutoLogger->Write(LV_STATUS, "SlimServerClient::dataConnectionThread()  Starting the data thread!");
//
// 					// assume connection failure. Only reset it when we know for sure that we are connected
// 					pSlimServerClient->dataThreadState = THREAD_CONNECTING;
// 					if ( slimDataHandler->openConnection() )
// 					{
// 						g_pPlutoLogger->Write(LV_STATUS, "SlimServerClient::dataConnectionThread() Data connection thread was succesfull!");
// 						pSlimServerClient->dataThreadState= THREAD_CONNECTED;
// 					}
// 					else
// 					{
// 						g_pPlutoLogger->Write(LV_STATUS, "SlimServerClient::dataConnectionThread() Data connection thread was not succesfull!");
// 						pSlimServerClient->dataThreadState = THREAD_STOPPED;;
// 					}
//
// 					break;
//
// 				case THREAD_PLAY:
// 					if ( pSlimServerClient->dataThreadState != THREAD_CONNECTED )
// 						break;
//
// 					pSlimServerClient->dataThreadState = THREAD_PLAYING;
// 					break;
//
// 				case THREAD_PAUSE:
// 					if ( pSlimServerClient->dataThreadState != THREAD_PLAYING )
// 						break;
// 					pSlimServerClient->dataThreadState = THREAD_PAUSED;
// 					break;
//
// 				case THREAD_EXIT:
// 					g_pPlutoLogger->Write(LV_STATUS, "SlimServerClient::dataConnectionThread() Ending the data thread!");
// 					pSlimServerClient->dataThreadState = THREAD_EXITING;
// 					break;
//
// 				default:
// 					g_pPlutoLogger->Write(LV_STATUS, "SlimServerClient::dataConnectionThread() Command not handled %d", pSlimServerClient->dataThreadCommand);
// 			}
//
// 			lastCommand = pSlimServerClient->dataThreadCommand;
// 		}
//
// 		if ( pSlimServerClient->dataThreadState == THREAD_CONNECTED )
// 			slimDataHandler->readStreamData();
//
// 		if ( pSlimServerClient->dataThreadState == THREAD_PLAYING && slimCommandHandler->needToSendStatus() )
// 			slimCommandHandler->doStatus("STMt");
//
// 		usleep(100);
// 	}
//
// 	return NULL;
// }

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

std::string SlimServerClient::getHostName()
{
	return m_strHostname;
}

DataReaderThread *SlimServerClient::getDataReaderThread()
{
	if ( m_pDataReaderThread == NULL )
		m_pDataReaderThread = new DataReaderThread();

	return m_pDataReaderThread;
}

SlimDataHandler *SlimServerClient::getSlimDataHandler()
{
	if ( m_pSlimDataHandler == NULL )
	{
		m_pSlimDataHandler = new SlimDataHandler();
		m_pSlimDataHandler->setSlimServerClient(this);
	}

	return m_pSlimDataHandler;
}

SlimCommandHandler *SlimServerClient::getSlimCommandHandler()
{
	if ( m_pSlimCommandHandler == NULL )
	{
		m_pSlimCommandHandler = new SlimCommandHandler();
		m_pSlimCommandHandler->setSlimServerClient(this);
	}

	return m_pSlimCommandHandler;
}

void SlimServerClient::setMacAddress(std::string strMacAddress)
{
	getSlimCommandHandler()->setMacAddress(strMacAddress);
}

std::string SlimServerClient::getFifoName()
{
	return StringUtils::Format("%d", m_pXineSlave->getDeviceId());
}

bool SlimServerClient::createDataReader(string fifoFileName, bool autostart)
{
	PLUTO_SAFETY_LOCK(xineSlaveMutex, *m_pXineSlaveMutex);
	g_pPlutoLogger->Write(LV_STATUS, "SlimServerClient::createDataReader() called");

	return
		   m_pXineSlave->createStream(string("fifo://") + fifoFileName, m_iStreamID, m_iRequestingObjectID)
		&& m_pXineSlave->playStream(m_iStreamID, 0, autostart);
}

bool SlimServerClient::pauseDataReader()
{
	PLUTO_SAFETY_LOCK(xineSlaveMutex, *m_pXineSlaveMutex);
	m_pXineSlave->pauseMediaStream(m_iStreamID);
	return true;
}

bool SlimServerClient::unpauseDataReader()
{
	PLUTO_SAFETY_LOCK(xineSlaveMutex, *m_pXineSlaveMutex);
	m_pXineSlave->changePlaybackSpeed(m_iStreamID, XineSlaveWrapper::PLAYBACK_FF_1);
	return true;
}

void SlimServerClient::setMediaStreamID(int iStreamID)
{
	m_iStreamID = iStreamID;
}

void SlimServerClient::setRequestingObjectID(int iRequestingObjectID)
{
	m_iRequestingObjectID = iRequestingObjectID;
}

int SlimServerClient::getPlaybackSeconds()
{
	PLUTO_SAFETY_LOCK(xineSlaveMutex, *m_pXineSlaveMutex);

	int positionTime = 0;
	int totalTime = 0;

	if ( m_pXineSlave != NULL)
		m_pXineSlave->getStreamPlaybackPosition(m_iStreamID, positionTime, totalTime);

	return positionTime / 1000;
}

