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
#ifndef SLIMDATAHANDLER_H
#define SLIMDATAHANDLER_H

#include <string>

#include "SocketCommunicator.h"
#include "SlimServerClientUser.h"

using std::string;

class SlimServerClient;

#define BUFFER_SIZE 262144
#define NETWORK_BUFFER_SIZE 1024*10

class SlimDataHandler: public SocketCommunicator, public SlimServerClientUser
{
	unsigned char 	*m_urlAddress;
	unsigned int 	m_urlAddressSize;

	unsigned char networkBuffer[NETWORK_BUFFER_SIZE];
	unsigned char buffer[BUFFER_SIZE];
	unsigned int  readPtr;
	unsigned int  writePtr;

	pthread_mutex_t bufferAccessMutex;
	pthread_t		fifoWriterThread;

	string 	fifoPipeName;

	long long epoch;

protected:
	bool fillBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen);
	bool readBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen);


	static void *xineFifoWriterThread(void *);
public:


    SlimDataHandler();

    ~SlimDataHandler();

	virtual bool openConnection();

	bool startProcessingData();

	bool readStreamData();

	unsigned int getReadPtr();
	unsigned int getWritePtr();
	unsigned int getBufferFreeSpace();
	unsigned int getBufferFilledSpace();

	long getBufferSize();
	long long getJiffies();

	bool setConnectionData(string strHostName, int port, unsigned char *urlAddress, unsigned int urlAddressSize);
};

#endif
