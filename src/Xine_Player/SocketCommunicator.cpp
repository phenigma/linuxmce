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
#include "SocketCommunicator.h"
#include "NetworkUtils.h"
#include "DCE/Logger.h"

#include <fcntl.h>

using namespace DCE;

SocketCommunicator::SocketCommunicator()
{
	m_iCommunicationSocket = -1;
	m_strHostName = "";
	m_iPort = 0;
}


SocketCommunicator::~SocketCommunicator()
{
	if ( isCommunicationOpen() )
		closeCommunication();
}

void SocketCommunicator::setTargetAddress(string strHostName, int iPort)
{
	m_strHostName = strHostName;
	m_iPort	= iPort;
}

bool SocketCommunicator::openCommunication()
{
	m_iCommunicationSocket = connect_to_host(m_strHostName.c_str(), m_iPort);
	return m_iCommunicationSocket != -1;
}

bool SocketCommunicator::closeCommunication()
{
	close(m_iCommunicationSocket);
	m_iCommunicationSocket = -1;
	return true;
}

bool SocketCommunicator::isCommunicationOpen()
{
	return m_iCommunicationSocket != -1;
}

bool SocketCommunicator::readData(unsigned char *buffer, int offset, int len, int &readSize)
{
	readSize = recv (m_iCommunicationSocket, buffer + offset, len, 0);

	if ( readSize != len && readSize != 0 )
		g_pPlutoLogger->Write(LV_STATUS, "Read %d chars wanted to read %d!", readSize, len);

	return readSize == len;
}

bool SocketCommunicator::readDataNonBlocking(unsigned char *buffer, int offset, int len, int &readSize)
{
	if ( ! isCommunicationOpen() )
		return false;

	if ( fcntl(m_iCommunicationSocket, F_SETFL, O_NONBLOCK) != 0 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SocketCommunicator::readDataNonBlocking() Could not put the socket in non blocking mode %s", strerror(errno));
		return false;
	}

	readData(buffer, offset, len, readSize);

	if ( fcntl(m_iCommunicationSocket, F_SETFL, 0) != 0 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SocketCommunicator::readDataNonBlocking() Could not reset the socket from non blocking mode %s", strerror(errno));
		return false;
	}

	if  (readSize < 0 )
		return false;

	return true;
}

bool SocketCommunicator::writeData(unsigned char *buffer, int offset, int len, int &writeCount)
{
	writeCount = send(m_iCommunicationSocket, buffer + offset, len, 0);

	g_pPlutoLogger->Write(LV_STATUS, "Result from send was %d", writeCount);

	return writeCount == len;
}
