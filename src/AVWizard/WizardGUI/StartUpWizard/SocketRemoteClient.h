//
// C++ Interface: SocketRemoteClient
//
// Description: 
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SOCKETREMOTECLIENT_H
#define SOCKETREMOTECLIENT_H

/**
	Class that manages one client 
	@author Ciprian Mustiata <ciprian.m@plutohome.com>
*/
#include <pthread.h>
#include <string>

class SocketRemoteServer;
class SocketRemoteClient
{
	int Socket;
	pthread_t ClientThread;
	SocketRemoteServer* Server;
public:
	pthread_mutex_t SafeMutex;
	bool Quit;

    SocketRemoteClient(SocketRemoteServer* Server, int Socket);

    ~SocketRemoteClient();
	std::string RecieveString();
	void SendString(std::string Message);
	void Disconnect();
};

void* SocketClientCallBack(void* SocketClientCallBackPtr);

#endif
