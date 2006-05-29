#ifndef SOCKETREMOTESERVER_H
#define SOCKETREMOTESERVER_H

#include <unistd.h>

/**
	That class is used to get commands remote (using a socket interface)
	@author Ciprian Mustiata <ciprian.m@plutohome.com>
*/

#include "SocketRemoteClient.h"
#include <vector>
#include <pthread.h>

class SocketRemoteServer{
	int SocketDescriptor;
	bool Closed;

	std::vector <SocketRemoteClient*> Clients;
	pthread_t ServerThreadID;
public:
	bool Finish;

    SocketRemoteServer();
    ~SocketRemoteServer();

	int StartServer(int Port);
	void Terminate();
	
	void MainLoop();

	void CreateClient(int SocketClient);

};

void Sleep(int Miliseconds);
void* ServerThreadCallBack(void* SocketClientCallBackPtr);

#endif
