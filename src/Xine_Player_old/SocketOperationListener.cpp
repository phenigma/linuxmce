/*
 SocketOperationListener

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#include "SocketOperationListener.h"

#include "DCE/Logger.h"

using namespace DCE;

SocketOperationListener::SocketOperationListener(string listenerName): m_listenerName(listenerName)
{
}

SocketOperationListener::~SocketOperationListener()
{
	map<int, int>::const_iterator itRegisteredSockets;

	itRegisteredSockets = m_mapRegisteredStatus.begin();
	while ( itRegisteredSockets != m_mapRegisteredStatus.end() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "SocketOperationListener::~SocketOperationListener() in destructor this socket (%d) was still registered in %d controllers.", itRegisteredSockets->first, itRegisteredSockets->second);
		itRegisteredSockets++;
	}
}

void SocketOperationListener::registeredForSocket(int socket)
{
	if ( m_mapRegisteredStatus.find(socket) == m_mapRegisteredStatus.end() )
		m_mapRegisteredStatus[socket] = 0;

	m_mapRegisteredStatus[socket]++;
}

void SocketOperationListener::unregisteredForSocket(int socket)
{
	if ( m_mapRegisteredStatus.find(socket) == m_mapRegisteredStatus.end() )
	{
 		g_pPlutoLogger->Write(LV_WARNING, "SocketOperationListener::unregisteredForSocket() called for a socket that was registered");
		return;
	}

	m_mapRegisteredStatus[socket]--;
	if ( m_mapRegisteredStatus[socket] <= 0 )
		m_mapRegisteredStatus.erase(socket);
}

bool SocketOperationListener::isSocketObserved(int socket)
{
	return m_mapRegisteredStatus.find(socket) != m_mapRegisteredStatus.end();
}

string SocketOperationListener::getName()
{
	return m_listenerName;
}
