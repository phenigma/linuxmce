/*
	DataReaderThread

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#ifndef DATAREADERTHREAD_H
#define DATAREADERTHREAD_H

#include <map>
#include <vector>

#include "SocketOperationListener.h"
#include "PlutoUtils/MultiThreadIncludes.h"

using std::map;
using std::vector;

class DataReaderThread
{
	map<int, vector<SocketOperationListener *> > m_mapSockets;

	pluto_pthread_mutex_t m_mutexLoop;

	pthread_t			  m_threadSelectLoop;
	pthread_cond_t		  m_threadControlCondition;
	pluto_pthread_mutex_t m_mutexThreadControl;

	bool			m_bThreadQuit;

	static void *socketWatchersThreadLoop(void *object);

	int setup_FD_SET(fd_set *fileDescriptorSet);
	void processDataAvailable(fd_set *fileDescriptorSet);

public:
    DataReaderThread();

    ~DataReaderThread();

	void addSocketOperationListenerForSocket(int socket, SocketOperationListener *pListener);

	void removedSocketOperationListenerForSocket(int socket, SocketOperationListener *pListener);

	void removeSocket(int socket);
};

#endif
