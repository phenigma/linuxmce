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
	: STATE_CLOSED_PIPE("pipe-closed"), STATE_WAIT_TO_SEND("waiting-to-send"), STATE_READY_TO_SEND("ready-to-send")
{
	m_bLogBufferAccess = false;

	resetBuffer();

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
// 	g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::findNextState() Computing next state from state \"%s\"!", pthread_self(), pCurrentState->getName().c_str());

	if ( getNeedToQuit() )
		return &STATE_QUIT;

	if ( pCurrentState == &STATE_START )
		return &STATE_CLOSED_PIPE;

	if ( pCurrentState == &STATE_CLOSED_PIPE && getBufferFilledSpace() > getBufferThresoldLevel() )
		return &STATE_WAIT_TO_SEND;

	if ( pCurrentState == &STATE_WAIT_TO_SEND && getBufferFilledSpace() > getPipeBufferSize() )
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

bool SlimDataHandler::initDataProcessing(bool autostart)
{
	fifoPipeName = string("/tmp/xine-slim-") + getSlimServerClient()->getFifoName();

	unlink(fifoPipeName.c_str());
	mkfifo(fifoPipeName.c_str(), S_IRWXU | S_IRWXG);

	return true;
}

bool SlimDataHandler::startDataReader()
{
	return getSlimServerClient()->startDataReader(fifoPipeName, true);
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

long SlimDataHandler::getBufferSize()
{
	return BUFFER_SIZE;
}

unsigned int SlimDataHandler::getBufferFreeSpace()
{
	int freeSpace;

	pthread_mutex_lock(&bufferAccessMutex);
	freeSpace = readPtr - writePtr;

	if ( freeSpace <= 0 )
		freeSpace += BUFFER_SIZE;

	pthread_mutex_unlock(&bufferAccessMutex);
	return (unsigned int)freeSpace;
}

unsigned int SlimDataHandler::getBufferFilledSpace()
{
	int availableData;

	pthread_mutex_lock(&bufferAccessMutex);
	availableData = writePtr - readPtr;

	if (availableData < 0)
		availableData += BUFFER_SIZE;

	pthread_mutex_unlock(&bufferAccessMutex);

	return availableData;
}

bool SlimDataHandler::fillBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen)
{
	int oldWritePtr;
	oldWritePtr = writePtr;

	if ( getBufferFreeSpace() < pBufferLen )
		return false;

	pthread_mutex_lock(&bufferAccessMutex);
	if ( m_bLogBufferAccess )
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

	if ( m_bLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after writing %d: [ read: %d, write: %d].", BUFFER_SIZE, pBufferLen, readPtr, writePtr);
	pthread_mutex_unlock(&bufferAccessMutex);
	return true;
}

void SlimDataHandler::resetBuffer()
{
	pthread_mutex_lock(&bufferAccessMutex);
	readPtr = writePtr = 0;
	bytesRx = 0;
	epochTime.tv_sec = epochTime.tv_usec = 0;
	pthread_mutex_unlock(&bufferAccessMutex);
}

bool SlimDataHandler::peekBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen)
{
	int oldReadPtr;
	int requestedReadSize;
	int copiedCount;

	oldReadPtr = readPtr;
	requestedReadSize = pBufferLen;

	if ( getBufferFilledSpace() < pBufferLen )
		return false;

	if ( m_bLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) before peeking %d: [ read: %d, write: %d].", BUFFER_SIZE, requestedReadSize, readPtr, writePtr);

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

	if ( m_bLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after peeking %d: [ read: %d, write: %d].", BUFFER_SIZE, requestedReadSize, readPtr, writePtr);

	return copiedCount;
}

bool SlimDataHandler::readBuffer(unsigned int readSize)
{
	int requestedReadSize;
	int copiedCount;

	if ( getBufferFilledSpace() < readSize )
		return false;

	requestedReadSize = readSize;

	pthread_mutex_lock(&bufferAccessMutex);
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

	if ( m_bLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after reading %d: [ read: %d, write: %d].", BUFFER_SIZE, requestedReadSize, readPtr, writePtr);

	pthread_mutex_unlock(&bufferAccessMutex);

	return copiedCount;
}

bool SlimDataHandler::readBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen)
{
	int requestedReadSize;
	int copiedCount;

	requestedReadSize = pBufferLen;

	if ( getBufferFilledSpace() < pBufferLen )
		return false;

	pthread_mutex_lock(&bufferAccessMutex);
	if ( m_bLogBufferAccess )
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

	if ( m_bLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after reading %d: [ read: %d, write: %d].", BUFFER_SIZE, requestedReadSize, readPtr, writePtr);

	pthread_mutex_unlock(&bufferAccessMutex);
	return true;
}

bool SlimDataHandler::openConnection()
{
	if ( ! openCommunication() )
		return false;

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
						closeCommunication();
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

void *SlimDataHandler::xineFifoWriterThread(void *parameters)
{
	SlimDataHandler *pSlimDataHandler = static_cast<SlimDataHandler*>(parameters);
	int 			fifoPipeFileDescriptor;
// 	struct timeval 	maxWaitTime;
// 	int 			returnSelectValue;

	while ( true )
// 	{
// 		fd_set writeFileDescriptorSet;
//
// 		FD_ZERO(&writeFileDescriptorSet);
// 		FD_SET(&writeFileDescriptorSet, fifoFile);
//
// 		maxWaitTime.tv_sec = 0;
//         // maxWaitTime.tv_usec = 250000; // wait at most 1/4 of a second
// 		maxWaitTime.tv_usec = 1000000; // wait at most 1/4 of a second
//
// 		returnSelectValue = select(fifoFile + 1, NULL, &writeFileDescriptorSet, NULL, &maxWaitTime);
//
// 		switch ( returnSelectValue )
// 		{
// 			case -1:
// 				g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::xineFifoWriterThread() Error during a select call: %s", strerror(errno));
// 				break;
// 			case 0:
// 				// timeout
// 				break;
// 			default:
// 				if ( FD_ISSET(fifoFile, &writeFileDescriptorSet) )
// 				{
// 					char buffer[PIPE_BUF];
// 					while ( pSlimDataHandler->readBuffer(buffer, 0, bufferSize) )
// 						write(fifoFile, buffer, bufferSize);
// 				}
// 		}
// 	}

	Sleep(800);
	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::xineFifoWriterThread() Opening the named fifo pipe %s", pSlimDataHandler->fifoPipeName.c_str());

	if ((fifoPipeFileDescriptor = open(pSlimDataHandler->fifoPipeName.c_str(), O_WRONLY)) == -1 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::xineFifoWriterThread() Fifo opening for write failed");
		return NULL;
	}

	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::xineFifoWriterThread() named fifo pipe %s is open", pSlimDataHandler->fifoPipeName.c_str());

	if ( fcntl(fifoPipeFileDescriptor, F_SETFL, O_NONBLOCK) != 0 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::xineFifoWriterThread() Could not put the file descriptor in non blocking mode (%s)", strerror(errno));
		return NULL;
	}


	char buffer[PIPE_BUF];
	int bufferSize = sizeof(buffer);
	while ( true )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::xineFifoWriterThread() Looping 0x%x!", pthread_self() );
		while ( ! pSlimDataHandler->readBuffer(buffer, 0, bufferSize) )
			usleep(100);

		while ( write(fifoPipeFileDescriptor, buffer, bufferSize ) == EAGAIN )
			usleep(100);

		Sleep(900);
	}
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
	g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::dataIsAvailable() Data is available on socket %d", pthread_self(), socket);

	while ( readStreamData() )
	{
		// g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::dataIsAvailable() Write Ptr: %d, Read Ptr: %d", pthread_self(), getWritePtr(), getReadPtr());

		if ( getBufferFilledSpace() > getBufferThresoldLevel() && ! isReaderRunning() )
		{
			signalConditionsChange();
			setReaderRunning(startDataReader());
		}
	}

	g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::dataIsAvailable() after exiting Write Ptr: %d, Read Ptr: %d", pthread_self(), getWritePtr(), getReadPtr());

	signalConditionsChange();

	return true;
}

void SlimDataHandler::setReaderRunning(bool isReaderRunning)
{
	m_isReaderRunning = isReaderRunning;
}

bool SlimDataHandler::isReaderRunning()
{
	return m_isReaderRunning;
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

		if ( getNeedToQuit() )
			return;

 		FD_ZERO(&writeFileDescriptorSet);
 		FD_SET(fifoFileDescriptor, &writeFileDescriptorSet);

 		maxWaitTime.tv_sec = 0;
 		maxWaitTime.tv_usec = 250000; // wait at most 1/4 of a second

 		returnSelectValue = select(fifoFileDescriptor + 1, NULL, &writeFileDescriptorSet, NULL, &maxWaitTime);

		if ( getNeedToQuit() )
			return;

 		switch ( returnSelectValue )
 		{
 			case  0: break;
 			case -1: g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::sendDataThruPipe() Error during a select call: %s", strerror(errno)); break;
 			default:
// 				g_pPlutoLogger->Write(LV_STATUS, "Select call result: %d", returnSelectValue);
 				if ( FD_ISSET(fifoFileDescriptor, &writeFileDescriptorSet) )
 				{
					bufferSize = getPipeBufferSize();
					while ( (bStillSending = peekBuffer((char*)pipeBuffer, 0, bufferSize)) )
					{
						if ( (writtenData = write(fifoFileDescriptor, pipeBuffer, bufferSize )) > 0 )
							readBuffer(writtenData);
						else
							break;

// 						g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::sendDataThruPipe() After the send loop %d, %d", pthread_self(), writtenData, bStillSending);
					}

					g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::sendDataThruPipe() After the send loop %d, %d (%d)", pthread_self(), writtenData, bStillSending, getBufferFilledSpace());
					if ( ! bStillSending ) // this means no write error but buffer underrun. In this case we need to actually drain the buffer
					{
						bufferSize = min(getBufferFilledSpace(), getPipeBufferSize());
						g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::sendDataThruPipe() Draining the buffer size(%d)", pthread_self(), bufferSize);
						if ( peekBuffer((char*)pipeBuffer, 0, bufferSize) && (writtenData = write(fifoFileDescriptor, pipeBuffer, bufferSize )) > 0 )
						{
							g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::sendDataThruPipe() Doing the actual write %d, %d", pthread_self(), writtenData, bStillSending);
							readBuffer(writtenData);
						}
					}
 				}
 		}

// 		g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::sendDataThruPipe() Write Ptr: %d, Read Ptr: %d", pthread_self(), getWritePtr(), getReadPtr());
	}

	g_pPlutoLogger->Write(LV_STATUS, "0x%x SlimDataHandler::sendDataThruPipe() before returning: Write Ptr: %d, Read Ptr: %d", pthread_self(), getWritePtr(), getReadPtr());
}

void SlimDataHandler::waitToSendDataThruPipe(State &fromState, State &toState)
{
// 	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::waitToSendDataThruPipe() need to send data to the pipe %d", fifoFileDescriptor);

	// data is available
	if ( getBufferFilledSpace() > 0 )
		return;

	waitConditionsChange();
}

void SlimDataHandler::quitMachine(State &fromState, State &toState)
{
	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::quitMachine() need to send data to the pipe %d", fifoFileDescriptor);
}
