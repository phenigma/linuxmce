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
#include <algorithm>

#include "DCE/Logger.h"
#include "DataReaderThread.h"

using namespace DCE;

DataReaderThread::DataReaderThread()
	: m_mutexLoop( "data-access-mutex"), m_mutexThreadControl("thread-control-mutex")
{
    pthread_mutexattr_t      mutexAttr;
    pthread_mutexattr_init( &mutexAttr );
    pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );

	pthread_cond_init( &m_threadControlCondition, NULL );

	m_mutexLoop.Init( &mutexAttr );
	m_mutexThreadControl.Init( &mutexAttr, &m_threadControlCondition);

	m_bThreadQuit = false;
	pthread_create(&m_threadSelectLoop, NULL, socketWatchersThreadLoop, this);
}

DataReaderThread::~DataReaderThread()
{
	m_bThreadQuit = true;
	pthread_join(m_threadSelectLoop, NULL);
}

void DataReaderThread::addSocketOperationListenerForSocket(int socket, SocketOperationListener *pListener)
{
	PLUTO_SAFETY_LOCK(dataLock, m_mutexLoop);

	g_pPlutoLogger->Write(LV_STATUS, "DataReaderThread::addSocketOperationListenerForSocket() adding listener %s for socket %d", pListener->getName().c_str(), socket);
	m_mapSockets[socket].push_back(pListener);
	pListener->registeredForSocket(socket);

	pthread_cond_signal(&m_threadControlCondition);
}

void DataReaderThread::removedSocketOperationListenerForSocket(int socket, SocketOperationListener *pListener)
{
	PLUTO_SAFETY_LOCK(dataLock, m_mutexLoop);

	g_pPlutoLogger->Write(LV_STATUS, "DataReaderThread::removedSocketOperationListenerForSocket() remove listener %s for socket %d", pListener->getName().c_str(), socket);
	map<int, vector<SocketOperationListener *> >::iterator itSockets;
	if ( (itSockets = m_mapSockets.find(socket)) == m_mapSockets.end() )
		return;

	vector<SocketOperationListener*>::iterator itListeners = itSockets->second.begin();
	while ( itListeners != itSockets->second.end() )
	{
		if ( *itListeners == pListener )
		{
			itListeners = itSockets->second.erase(itListeners);
			pListener->unregisteredForSocket(socket);
		}
		else
			itListeners++;
	}

	if ( m_mapSockets[socket].size() == 0 )
		m_mapSockets.erase(socket);
}

void DataReaderThread::removeSocket(int socket)
{
	PLUTO_SAFETY_LOCK(dataLock, m_mutexLoop);

	map<int, vector<SocketOperationListener *> >::iterator itSockets;
	if ( (itSockets = m_mapSockets.find(socket)) == m_mapSockets.end() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "DataReaderThread::removeSocket() remove socket called for invalid socket: %d", socket);
		return;
	}

	vector<SocketOperationListener*>::iterator itListeners = itSockets->second.begin();
	while ( itListeners != itSockets->second.end() )
	{
		(*itListeners)->unregisteredForSocket(socket);
		itListeners = itSockets->second.erase(itListeners);
	}

	m_mapSockets.erase(socket);
}

int DataReaderThread::setup_FD_SET(fd_set *fileDescriptorSet)
{
	PLUTO_SAFETY_LOCK(dataLock, m_mutexLoop);

	int maxFileDescriptor;
	map<int, vector<SocketOperationListener*> >::const_iterator itSockets;

	maxFileDescriptor = 0;
	itSockets = m_mapSockets.begin();

	FD_ZERO(fileDescriptorSet);
	while( itSockets != m_mapSockets.end() )
	{
		FD_SET(itSockets->first, fileDescriptorSet);
		maxFileDescriptor = max(maxFileDescriptor, itSockets->first);
		itSockets++;
	}

	return maxFileDescriptor;
}

void DataReaderThread::processDataAvailable(fd_set *fileDescriptorSet)
{
	map<int, vector<SocketOperationListener*> > m_mapActiveListeners;

	PLUTO_SAFETY_LOCK(dataLock, m_mutexLoop);

	map<int, vector<SocketOperationListener*> >::const_iterator itSockets;

	itSockets = m_mapSockets.begin();
	while( itSockets != m_mapSockets.end() )
	{
		if ( FD_ISSET(itSockets->first, fileDescriptorSet) )
			m_mapActiveListeners[itSockets->first] = itSockets->second;
		itSockets++;
	}

	dataLock.Release();

	itSockets = m_mapActiveListeners.begin();
	while( itSockets != m_mapActiveListeners.end() )
	{
// 		g_pPlutoLogger->Write(LV_STATUS, "DataReaderThread::processDataAvailable() Data is available on socket %d", itSockets->first);
		vector<SocketOperationListener *>::const_iterator itListeners = itSockets->second.begin();

		while ( itListeners != itSockets->second.end() )
		{
			(*itListeners)->dataIsAvailable(itSockets->first);
			itListeners++;
		}
		itSockets++;
	}
}

void *DataReaderThread::socketWatchersThreadLoop(void *object)
{
	DataReaderThread *thisObject = static_cast<DataReaderThread*>(object);

	PLUTO_SAFETY_LOCK(dataLock, thisObject->m_mutexLoop);
	dataLock.Release();

	while ( true )
	{
		fd_set 			readFileDescriptors;
		int 			maxFileDescriptor;
        struct timeval 	maxWaitTime;
        int 			returnSelectValue;

		PLUTO_SAFETY_LOCK(nonBusyWaitLock, thisObject->m_mutexThreadControl);
		while ( thisObject->m_mapSockets.size() == 0 && ! thisObject->m_bThreadQuit )
		{
			g_pPlutoLogger->Write(LV_STATUS, "DataReaderThread::socketWatchersThreadLoop() Waiting");
			nonBusyWaitLock.CondWait();
		}
		nonBusyWaitLock.Release();

		if ( thisObject->m_bThreadQuit )
		{
			g_pPlutoLogger->Write(LV_STATUS, "DataReaderThread::socketWatchersThreadLoop() Quiting");
			return NULL;
		}

		dataLock.Relock();
		maxFileDescriptor = thisObject->setup_FD_SET(&readFileDescriptors);

        maxWaitTime.tv_sec = 0;
        // maxWaitTime.tv_usec = 250000; // wait at most 1/4 of a second
		maxWaitTime.tv_usec = 1000000; // wait at most 1/4 of a second

		dataLock.Release();
		returnSelectValue = select(maxFileDescriptor + 1, &readFileDescriptors, NULL, NULL, &maxWaitTime);

		switch ( returnSelectValue )
		{
			case -1:
				g_pPlutoLogger->Write(LV_STATUS, "DataReaderThread::socketWatchersThreadLoop() Error during a select call: %s", strerror(errno));
				break;
			case 0:
				// timeout
				break;
			default:
				thisObject->processDataAvailable(&readFileDescriptors);
		}
	}
}
