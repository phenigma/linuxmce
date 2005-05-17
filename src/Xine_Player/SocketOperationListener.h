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
#ifndef SOCKETOPERATIONLISTENER_H
#define SOCKETOPERATIONLISTENER_H

#include <map>
#include <string>

using std::map;
using std::string;

class SocketOperationListener
{
	map<int, int> m_mapRegisteredStatus;

	string 		  m_listenerName;

public:
	SocketOperationListener(string name);

	virtual ~SocketOperationListener();

	virtual void registeredForSocket(int socket);

	virtual void unregisteredForSocket(int socket);

	virtual bool isSocketObserved(int socket);

	virtual bool dataIsAvailable(int socket) = 0;

	virtual string getName();
};

#endif
