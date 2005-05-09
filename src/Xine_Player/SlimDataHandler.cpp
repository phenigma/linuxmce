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

#include <inttypes.h>
#include <fcntl.h>

using namespace DCE;

SlimDataHandler::SlimDataHandler()
{
	bLogBufferAccess = false;
}


SlimDataHandler::~SlimDataHandler()
{

}

bool SlimDataHandler::startProcessingData(bool autostart)
{
	getSlimServerClient()->commandDataThread(SlimServerClient::THREAD_START);

	fifoPipeName = string("/tmp/xine-slim-") + getSlimServerClient()->getFifoName();

	unlink(fifoPipeName.c_str());
	mkfifo(fifoPipeName.c_str(), S_IRWXU | S_IRWXG);

	if ( autostart )
	{
		pthread_create(&fifoWriterThread, NULL, &xineFifoWriterThread, this);
		getSlimServerClient()->startDataReader(fifoPipeName, autostart);
	}

	return true;
}

bool SlimDataHandler::readStreamData()
{
	int readCount;
	if ( ! readDataNonBlocking(networkBuffer, 0, NETWORK_BUFFER_SIZE, readCount) )
		return false;

	fillBuffer((char*)networkBuffer, 0, readCount);

	return true;
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
	freeSpace = readPtr - writePtr - 1;

	if ( freeSpace < 0 )
		freeSpace += BUFFER_SIZE;

	pthread_mutex_unlock(&bufferAccessMutex);
	return (unsigned int)freeSpace;
}

unsigned int SlimDataHandler::getBufferFilledSpace()
{
	int availableData;

	pthread_mutex_lock(&bufferAccessMutex);
	if (writePtr != readPtr)
	{
		availableData = writePtr - readPtr - 1;

		if (availableData < 0)
			availableData += BUFFER_SIZE;
	}
	else
		availableData = 0;

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
	if ( bLogBufferAccess )
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

	if ( bLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after writing %d: [ read: %d, write: %d].", BUFFER_SIZE, pBufferLen, readPtr, writePtr);
	pthread_mutex_unlock(&bufferAccessMutex);
	return true;
}

bool SlimDataHandler::readBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen)
{
	int oldReadPtr;
	int requestedReadSize;
	int copiedCount;

	oldReadPtr = readPtr;
	requestedReadSize = pBufferLen;

	if ( getBufferFilledSpace() <= pBufferLen )
		return false;


	pthread_mutex_lock(&bufferAccessMutex);
	if ( bLogBufferAccess )
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

	if ( bLogBufferAccess )
		g_pPlutoLogger->Write(LV_STATUS, "--->Buffer (%d) after reading %d: [ read: %d, write: %d].", BUFFER_SIZE, requestedReadSize, readPtr, writePtr);

	pthread_mutex_unlock(&bufferAccessMutex);
	return copiedCount;
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
			g_pPlutoLogger->Write(LV_STATUS, "Error reading data from server (requested %d got %d).", 1, readCount);
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
	int fifoPipeFileDescriptor;

	g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::xineFifoWriterThread() Opening the name fifo pipe %s", pSlimDataHandler->fifoPipeName.c_str());

	if ((fifoPipeFileDescriptor = open(pSlimDataHandler->fifoPipeName.c_str(), O_WRONLY)) == -1 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::xineFifoWriterThread() Fifo opening for write failed");
		return NULL;
	}

	if ( fcntl(fifoPipeFileDescriptor, F_SETFL, O_NONBLOCK) != 0 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimDataHandler::xineFifoWriterThread() Could not put the file descriptor in non blocking mode (%s)", strerror(errno));
		return NULL;
	}

	char buffer[PIPE_BUF];
	int bufferSize = sizeof(buffer);
	while ( true )
	{
		while ( ! pSlimDataHandler->readBuffer(buffer, 0, bufferSize) )
			Sleep(1);

		while ( write(fifoPipeFileDescriptor, buffer, bufferSize ) == EAGAIN )
			Sleep(1);
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


	return (currentTime.tv_sec - epochTime.tv_sec) * 1000 + currentTime.tv_usec - epochTime.tv_usec;
}

long long SlimDataHandler::getBytesRx()
{
	return bytesRx;
}
