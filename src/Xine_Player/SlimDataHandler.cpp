/*
	SlimDataHandler

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#include "SlimDataHandler.h"
#include "SlimServerClient.h"

#include "NetworkUtils.h"

#include "DCE/Logger.h"


#include <fcntl.h>

using namespace DCE;

SlimDataHandler::SlimDataHandler()
	:	SocketOperationListener("slim-data-handler"), bufferAccessMutex("buffer-access-mutex"), m_isLogBufferAccess(false), m_isPlayerCreated(false), m_isAutoStart(false)
		, STATE_CLOSED_PIPE("pipe-closed"), STATE_WAIT_TO_SEND("waiting-to-send"), STATE_READY_TO_SEND("ready-to-send")
{
    pthread_mutexattr_t      mutexAttr;
    pthread_mutexattr_init( &mutexAttr );
    pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );

	pthread_cond_init( &bufferAccessCondition, NULL );
	bufferAccessMutex.Init( &mutexAttr, &bufferAccessCondition);

	resetBuffer();

	setPlayerState(PlayerState::PLAYER_DISCONNECTED);

	gettimeofday(&epochTime, NULL);

	addTransition(STATE_START, STATE_CLOSED_PIPE, &ThreadedStateMachine::emptyTransition);

	addTransition(STATE_CLOSED_PIPE, STATE_WAIT_TO_SEND, (MachineTransitionFunction)&SlimDataHandler::openPipe);
	addTransition(STATE_WAIT_TO_SEND, STATE_CLOSED_PIPE, (MachineTransitionFunction)&SlimDataHandler::closePipe);

	addTransition(STATE_WAIT_TO_SEND, STATE_READY_TO_SEND, (MachineTransitionFunction)&SlimDataHandler::waitToSendDataThruPipe);
	addTransition(STATE_READY_TO_SEND, STATE_WAIT_TO_SEND, (MachineTransitionFunction)&SlimDataHandler::sendDataThruPipe);

	addTransition(STATE_READY_TO_SEND, STATE_CLOSED_PIPE, (MachineTransitionFunction)&SlimDataHandler::closePipe);

	addTransition(STATE_WAIT_TO_SEND, STATE_QUIT, (MachineTransitionFunction)&SlimDataHandler::quitMachine);
	addTransition(STATE_READY_TO_SEND, STATE_QUIT, (MachineTransitionFunction)&SlimDataHandler::quitMachine);
	addTransition(STATE_CLOSED_PIPE, STATE_QUIT, (MachineTransitionFunction)&SlimDataHandler::quitMachine);
	addTransition(STATE_START, STATE_QUIT, (MachineTransitionFunction)&SlimDataHandler::quitMachine);

	startMachine();
}

SlimDataHandler::~SlimDataHandler()
{

}

ThreadedStateMachine::State *SlimDataHandler::findNextState(ThreadedStateMachine::State *pCurrentState)
{
	g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::findNextState() Current: \"%s\" [ fill: %d, thres: %d, play state %s, quit: %d]!",
			pthread_self(),
			pCurrentState->getName().c_str(),
			getBufferFilledSpace(),
			getBufferThresoldLevel(),
			PlayerState::describeState(getPlayerState()).c_str(),
			getNeedToQuit());

	if ( getNeedToQuit() )
		return &STATE_QUIT;

	if ( pCurrentState == &STATE_START )
		return &STATE_CLOSED_PIPE;

	if ( pCurrentState == &STATE_CLOSED_PIPE && getBufferFilledSpace() > getBufferThresoldLevel() && getPlayerState() == PlayerState::PLAYER_BUFFERING )
		return &STATE_WAIT_TO_SEND;

	if ( pCurrentState == &STATE_WAIT_TO_SEND && getBufferFilledSpace() > 0 )
		return &STATE_READY_TO_SEND;

	if ( pCurrentState == &STATE_READY_TO_SEND )
		return &STATE_WAIT_TO_SEND;

// 	{
// 		return pCurrentState;
// 	}
// 	else if ( pCurrentState == &STATE_READY_TO_SEND )
// 	{
// 		return pCurrentState;
// 	}
// 	else if ( pCurrentState == &STATE_QUIT )
// 	{
// 		return pCurrentState;
// 	}

	return &STATE_WAITING;
}

void SlimDataHandler::setStreamFormat(unsigned char streamFormat)
{
	m_streamFormat = streamFormat;
}

unsigned char SlimDataHandler::getStreamFormat()
{
	return m_streamFormat;
}

unsigned int SlimDataHandler::getBufferThresoldLevel()
{
	return BUFFER_FULLNESS_THRESHOLD_SIZE;
}

unsigned int SlimDataHandler::getPipeBufferSize()
{
	return sizeof(pipeBuffer);
}

bool SlimDataHandler::initDataProcessing()
{
	fifoPipeName = string("/tmp/xine-slim-") + getSlimServerClient()->getFifoName();

	unlink(fifoPipeName.c_str());
	mkfifo(fifoPipeName.c_str(), S_IRWXU | S_IRWXG);

	return true;
}

bool SlimDataHandler::createPlayer()
{
	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::createPlayer() entry");
	bool bResult = getSlimServerClient()->createDataReader(fifoPipeName);
	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::createPlayer() exit");

	return bResult;
}

bool SlimDataHandler::startPlayer()
{
	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::startPlayer() entry [state: %s])", PlayerState::describeState(getPlayerState()).c_str());
	if ( getPlayerState() == PlayerState::PLAYER_PLAYING)
		return true;

	if ( ! getSlimServerClient()->startDataReader(isAutoStart()) )
		return false;

	setPlayerState(PlayerState::PLAYER_PLAYING);
	return true;
}

bool SlimDataHandler::readStreamData()
{
	int readCount;
	if ( ! readDataNonBlocking(networkBuffer, 0, min(NETWORK_BUFFER_SIZE, getBufferFreeSpace() - 1), readCount) )
		return false;

	if ( readCount != 0 )
	{
		fillBuffer((char*)networkBuffer, 0, readCount);
		return true;
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::readStreamData() Closing connection");
		getSlimServerClient()->getDataReaderThread()->removeSocket(getCommunicationSocket());
		closeCommunication();
		return false;
	}
}

unsigned int SlimDataHandler::getReadPtr()
{
	return readPtr;
}

unsigned int SlimDataHandler::getWritePtr()
{
	return writePtr;
}

unsigned int SlimDataHandler::getBufferSize()
{
	return BUFFER_SIZE;
}

unsigned int SlimDataHandler::getBufferFreeSpace()
{
	PLUTO_SAFETY_LOCK(lock, bufferAccessMutex);
	int freeSpace;

	freeSpace = readPtr - writePtr;

	if ( freeSpace <= 0 )
		freeSpace += BUFFER_SIZE;

	return (unsigned int)freeSpace;
}

unsigned int SlimDataHandler::getBufferFilledSpace()
{
	PLUTO_SAFETY_LOCK(lock, bufferAccessMutex);

	int availableData;

	availableData = writePtr - readPtr;

	if (availableData < 0)
		availableData += BUFFER_SIZE;

	return availableData;
}

bool SlimDataHandler::fillBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen)
{
	PLUTO_SAFETY_LOCK(lock, bufferAccessMutex);

	int oldWritePtr;
	oldWritePtr = writePtr;

	if ( getBufferFreeSpace() < pBufferLen )
		return false;

	if ( m_isLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) before writing %d: [ read: %d, write: %d].", BUFFER_SIZE, pBufferLen, readPtr, writePtr);

	if ( writePtr < readPtr)
	{
		memcpy(buffer + writePtr, pBuffer + pBufferOffset, pBufferLen);
		writePtr += pBufferLen;
	}
	else
	{
		if ( writePtr + pBufferLen < BUFFER_SIZE )
		{
			memcpy(buffer + writePtr, pBuffer + pBufferOffset, pBufferLen);
			writePtr += pBufferLen;
		}
		else
		{
			int currentWriteSize = writePtr;

			memcpy(buffer + writePtr, pBuffer + pBufferOffset, BUFFER_SIZE - currentWriteSize );
			memcpy(buffer, pBuffer + pBufferOffset + BUFFER_SIZE - currentWriteSize, pBufferLen - (BUFFER_SIZE - currentWriteSize) );

			writePtr = pBufferLen - (BUFFER_SIZE - currentWriteSize);
		}
	}

	bytesRx += pBufferLen;

	if ( m_isLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after writing %d: [ read: %d, write: %d].", BUFFER_SIZE, pBufferLen, readPtr, writePtr);

	return true;
}

void SlimDataHandler::resetBuffer()
{
	PLUTO_SAFETY_LOCK(lock, bufferAccessMutex);
	readPtr = writePtr = 0;
	bytesRx = 0;
	epochTime.tv_sec = epochTime.tv_usec = 0;
}

bool SlimDataHandler::peekBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen)
{
	int oldReadPtr;
	int requestedPeekSize;
	int copiedCount;

	oldReadPtr = readPtr;
	requestedPeekSize = pBufferLen;

	if ( getBufferFilledSpace() < pBufferLen )
		return false;

	if ( m_isLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) before peeking %d: [ read: %d, write: %d].", BUFFER_SIZE, requestedPeekSize, readPtr, writePtr);

	copiedCount = 0;

	if ( writePtr < readPtr )
	{
		unsigned int count = BUFFER_SIZE - readPtr;
		if ( count > pBufferLen)
			count = pBufferLen;

		memcpy(pBuffer + pBufferOffset, buffer + readPtr, count);

		pBufferLen -= count;
		copiedCount += count;
	}

	if ( pBufferLen > 0 )
	{
		unsigned int count = writePtr - readPtr;
		if (count > pBufferLen)
			count = pBufferLen;

		memcpy(pBuffer + pBufferOffset + copiedCount, buffer + readPtr, count);

		pBufferLen -= count;
		copiedCount += count;
	}

	if ( m_isLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after peeking %d: [ read: %d, write: %d] copied count: %d.", BUFFER_SIZE, requestedPeekSize, readPtr, writePtr, copiedCount);

	return copiedCount == requestedPeekSize;
}

bool SlimDataHandler::readBuffer(unsigned int readSize)
{
	PLUTO_SAFETY_LOCK(lock, bufferAccessMutex);

	int requestedReadSize;
	int copiedCount;

	if ( getBufferFilledSpace() < readSize )
		return false;

	requestedReadSize = readSize;

	if ( writePtr < readPtr )
	{
		unsigned int count = BUFFER_SIZE - readPtr;
		if ( count > readSize)
			count = readSize;

		readPtr += readSize;
		if ( readPtr >= BUFFER_SIZE )
			readPtr -= BUFFER_SIZE;

		readSize -= count;
		copiedCount += count;
	}

	if ( readSize > 0 )
	{
		unsigned int count = writePtr - readPtr;
		if (count > readSize)
			count = readSize;

		readPtr += count;
		if ( readPtr >= BUFFER_SIZE )
			readPtr -= BUFFER_SIZE;

		readSize -= count;
	}

	if ( m_isLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after reading %d: [ read: %d, write: %d].", BUFFER_SIZE, requestedReadSize, readPtr, writePtr);

	return copiedCount;
}

bool SlimDataHandler::readBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen)
{
	PLUTO_SAFETY_LOCK(lock, bufferAccessMutex);

	int requestedReadSize;
	int copiedCount;

	requestedReadSize = pBufferLen;

	if ( getBufferFilledSpace() < pBufferLen )
		return false;

	if ( m_isLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) before reading %d: [ read: %d, write: %d].", BUFFER_SIZE, requestedReadSize, readPtr, writePtr);

	copiedCount = 0;

	if ( writePtr < readPtr )
	{
		unsigned int count = BUFFER_SIZE - readPtr;
		if ( count > pBufferLen)
			count = pBufferLen;

		memcpy(pBuffer + pBufferOffset, buffer + readPtr, count);

		readPtr += pBufferLen;
		if ( readPtr >= BUFFER_SIZE )
			readPtr -= BUFFER_SIZE;

		pBufferLen -= count;
		copiedCount += count;
	}

	if ( pBufferLen > 0 )
	{
		unsigned int count = writePtr - readPtr;
		if (count > pBufferLen)
			count = pBufferLen;

		memcpy(pBuffer + pBufferOffset + copiedCount, buffer + readPtr, count);

		readPtr += count;
		if ( readPtr >= BUFFER_SIZE )
			readPtr -= BUFFER_SIZE;

		pBufferLen -= count;
		copiedCount += count;
	}

	if ( m_isLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after reading %d: [ read: %d, write: %d].", BUFFER_SIZE, requestedReadSize, readPtr, writePtr);

	return true;
}

bool SlimDataHandler::closeConnection()
{
	setPlayerState(PlayerState::PLAYER_DISCONNECTED);

	getSlimServerClient()->getDataReaderThread()->removedSocketOperationListenerForSocket(getCommunicationSocket(), this);

	return closeCommunication();
}

bool SlimDataHandler::openConnection()
{
	if ( ! openCommunication() )
		return false;

	setPlayerState(PlayerState::PLAYER_CONNECTED);

	unsigned int done,  linenum, contentlength, writeBufferSize;
	int len;

	int readCount, writeCount;

	writeBufferSize = 0;
	snprintf((char*)buffer, BUFFER_SIZE - writeBufferSize,
			"GET %s HTTP/1.0\015\012"
			"User-Agent: xine/xine\015\012"
			"Accept: */*\015\012"
			"\015\012", m_urlAddress);

	g_pPlutoLogger->Write(LV_STATUS, "Sending:\n%s", buffer);

	writeBufferSize = (unsigned int)strlen((const char*)buffer);

	if ( ! writeData(buffer, 0, writeBufferSize, writeCount) )
		return false;

	contentlength = 0;
	linenum = 0;
	done = 0;
	len = 0;


	while (!done)
	{
		if ( ! readData(buffer, len, 1, readCount) )
		{
			g_pPlutoLogger->Write(LV_STATUS, "Error reading data from server (requested %d got %d) (%s).", 1, readCount, strerror(errno));
			return false;
		}

		if (buffer[len] == '\012')
		{
			buffer[len] = '\0';
			len--;

			if (len >= 0 && buffer[len] == '\015')
			{
				buffer[len] = '\0';
				len--;
			}

			g_pPlutoLogger->Write(LV_STATUS, "-->Read: %s", buffer);

			linenum++;
			if (linenum == 1)
			{
				int httpver, httpsub, httpcode;
				char httpstatus[51];

				if (sscanf((char *)buffer, "HTTP/%d.%d %d %50[^\015\012]", &httpver, &httpsub, &httpcode, httpstatus) != 4)
				{
					if (sscanf((char *)buffer, "ICY %d %50[^\015\012]", &httpcode, httpstatus) != 2)
					{
						g_pPlutoLogger->Write(LV_STATUS, "Invalid http answer");
						closeConnection();
						return false;
					}
					else
					{
						done = 1;
					}
				}
			}
			else
			{
				if (contentlength == 0)
				{
					long parsedLength;

					if (sscanf((char *)buffer, "Content-Length: %ld", &parsedLength) == 1)
					{
						g_pPlutoLogger->Write(LV_STATUS, "bool SlimDataHandler::openConnection() Content Length = %ld bytes", parsedLength);
						contentlength = (off_t)parsedLength;
					}
				}

				if (!strncmp((char*)buffer, "Location: ", 10))
				{
				}
			}

			if (len == -1) done = 1;
			else		   len = 0;
		} else
			len ++;
	}

	if ( ! initDataProcessing() )
	{
		closeConnection();
		return false;
	}

	setPlayerState(PlayerState::PLAYER_BUFFERING);
	return true;
}

bool SlimDataHandler::setConnectionData(string strHostName, int iPort, unsigned char *urlAddress, unsigned int urlAddressSize)
{
	g_pPlutoLogger->Write(LV_STATUS, "Need to open data connection to %s:%d%s", strHostName.c_str(), iPort, urlAddress);

	setTargetAddress(strHostName, iPort);
	m_urlAddress = urlAddress;
	m_urlAddressSize = urlAddressSize;

	return true;
}

long long SlimDataHandler::getJiffies()
{
	if ( epochTime.tv_sec == 0 && epochTime.tv_usec == 0 )
	{
		if ( ! gettimeofday(&epochTime, NULL) == -1 )
			g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::getJiffies() getimeofday failed %s", strerror(errno));

		return 0;
	}

	struct timeval currentTime;

	if ( ! gettimeofday(&currentTime, NULL) == -1 )
		g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::getJiffies() getimeofday failed %s", strerror(errno));

	return (currentTime.tv_sec - epochTime.tv_sec) * 1000 + (currentTime.tv_usec - epochTime.tv_usec) / 1000;
}

long long SlimDataHandler::getBytesRx()
{
	return bytesRx;
}

bool SlimDataHandler::dataIsAvailable(int socket)
{
	if ( getBufferFreeSpace() == 1 )
	{
		// when the buffer space is scarce we remove ourselves from the watcher thread. We will get readded by the state machine right after the buffer is read again.
		getSlimServerClient()->getDataReaderThread()->removedSocketOperationListenerForSocket(socket, this);
		return false;
	}

	while ( readStreamData() )
	{
		if ( getBufferFilledSpace() > getBufferThresoldLevel() && getPlayerState() == PlayerState::PLAYER_BUFFERING && ! isPlayerCreated() ) {
			signalConditionsChange();
			setPlayerCreated(createPlayer());
			if ( isPlayerCreated() )
				getSlimServerClient()->getSlimCommandHandler()->doStatus("STMt");
		}
	}

	g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::dataIsAvailable() New Write Ptr: %d, Read Ptr: %d", pthread_self(), getWritePtr(), getReadPtr());

	signalConditionsChange();

	return true;
}

void SlimDataHandler::openPipe(State &fromState, State &toState)
{
	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::openPipe() Opening the named fifo pipe %s", fifoPipeName.c_str());

	if ((fifoFileDescriptor = open(fifoPipeName.c_str(), O_WRONLY)) == -1 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::openPipe() Fifo opening for write failed");
		return;
	}

	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::openPipe() named fifo pipe %s is open", fifoPipeName.c_str());

	if ( fcntl(fifoFileDescriptor , F_SETFL, O_NONBLOCK) != 0 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::openPipe() Could not put the file descriptor in non blocking mode (%s)", strerror(errno));
		return;
	}

	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::openPipe() the file descriptor %d is set to non blocking", fifoFileDescriptor);
}

void SlimDataHandler::closePipe(State &fromState, State &toState)
{
	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::closePipe() need to send data to the pipe %d", fifoFileDescriptor);
}

void SlimDataHandler::sendDataThruPipe(State &fromState, State &toState)
{
	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::sendDataThruPipe() need to send data to the pipe %d", fifoFileDescriptor);

	int  writtenData;
	int  bufferSize;

	bool bStillSending = true;
	while ( bStillSending )
	{
		fd_set 			writeFileDescriptorSet;
		int				returnSelectValue;
		struct timeval  maxWaitTime;

		if ( getNeedToQuit() || fifoFileDescriptor == -1)
			return;

 		FD_ZERO(&writeFileDescriptorSet);
 		FD_SET(fifoFileDescriptor, &writeFileDescriptorSet);

 		maxWaitTime.tv_sec = 0; // wait at most 1 second
 		maxWaitTime.tv_usec = 100;

 		returnSelectValue = select(fifoFileDescriptor + 1, NULL, &writeFileDescriptorSet, NULL, &maxWaitTime);

		if ( getNeedToQuit() )
			return;

 		switch ( returnSelectValue )
 		{
 			case  0: break;
 			case -1: g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::sendDataThruPipe() Error during a select call: %s", strerror(errno)); break;
 			default:
 				// g_pPlutoLogger->Write(LV_STATUS, "Select call result: %d", returnSelectValue);
 				if ( FD_ISSET(fifoFileDescriptor, &writeFileDescriptorSet) )
 				{
					if ( (bufferSize = min(getBufferFilledSpace(), getPipeBufferSize())) == 0 )
					{
						bStillSending = false;
						break;
					}

					while ( (bStillSending = peekBuffer((char*)pipeBuffer, 0, bufferSize)) )
					{
						if ( (writtenData = write(fifoFileDescriptor, pipeBuffer, bufferSize )) > 0 )
							readBuffer(writtenData);

						if ( writtenData == 0 || (writtenData == -1 && errno != EAGAIN) )
						{
							bStillSending = false; // the socket was just closed.
							close(fifoFileDescriptor);
							fifoFileDescriptor = -1;
						}

						if ( writtenData <= 0 )
							break;

// 						g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::sendDataThruPipe() After the send loop %d, %d, bufferSize %d", pthread_self(), writtenData, bStillSending, bufferSize);
						if ( (bufferSize = min(getBufferFilledSpace(), getPipeBufferSize())) == 0 )
						{
							bStillSending = false;
							break;
						}
					}
 				}
 		}

		if ( isCommunicationOpen() && ! isSocketObserved(getCommunicationSocket()) && getBufferFreeSpace() > getBufferSize() / 2  )
			getSlimServerClient()->getDataReaderThread()->addSocketOperationListenerForSocket(getCommunicationSocket(), this);

//  		g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::sendDataThruPipe() Write Ptr: %d, Read Ptr: %d", pthread_self(), getWritePtr(), getReadPtr());
	}

	g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::sendDataThruPipe() before returning: Write Ptr: %d, Read Ptr: %d", pthread_self(), getWritePtr(), getReadPtr());
}

void SlimDataHandler::waitToSendDataThruPipe(State &fromState, State &toState)
{
// 	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::waitToSendDataThruPipe() need to send data to the pipe %d", fifoFileDescriptor);

	// data is available
	if ( getBufferFilledSpace() > 0 && fifoFileDescriptor != -1 )
		return;

	waitConditionsChange();
}

void SlimDataHandler::quitMachine(State &fromState, State &toState)
{
	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::quitMachine() need to send data to the pipe %d", fifoFileDescriptor);
}

SlimDataHandler::PlayerState::STATE SlimDataHandler::getPlayerState()
{
	return m_playerState;
}

void SlimDataHandler::setPlayerState(PlayerState::STATE playerState)
{
	g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::setPlayerState() Setting state to %s", pthread_self(), PlayerState::describeState(playerState).c_str());
	m_playerState = playerState;
	signalConditionsChange();
}

bool SlimDataHandler::isAutoStart()
{
	return m_isAutoStart;
}

void SlimDataHandler::setAutoStart(bool isAutoStart)
{
	m_isAutoStart = isAutoStart;
}

bool SlimDataHandler::isPlayerCreated()
{
	return m_isPlayerCreated;
}

void SlimDataHandler::setPlayerCreated(bool playerCreated)
{
	m_isPlayerCreated = playerCreated;
}
