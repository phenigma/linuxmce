/*
	SocketCommunicator

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#ifndef SOCKETCOMMUNICATOR_H
#define SOCKETCOMMUNICATOR_H

#include <string>

using std::string;

/**
 * This class is handling all the low level details of socket communication.
 */
class SocketCommunicator
{
	int m_iCommunicationSocket;

	string 	m_strHostName;
	int 	m_iPort;

public:
    SocketCommunicator();

    virtual ~SocketCommunicator();


	virtual int getCommunicationSocket();

	virtual void setTargetAddress(string strHostName, int iPort);

	virtual bool openCommunication();
	virtual bool closeCommunication();

	virtual bool isCommunicationOpen();

	virtual bool readData(unsigned char *buffer, int offset, int len, int &readSize);
	virtual bool readDataNonBlocking(unsigned char *buffer, int offset, int len, int &readSize);
	virtual bool writeData(unsigned char *buffer, int offset, int len, int &writeCount);
};

#endif
