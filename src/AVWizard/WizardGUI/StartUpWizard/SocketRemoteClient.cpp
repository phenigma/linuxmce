//
// C++ Implementation: SocketRemoteClient
//
// Description: 
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "SocketRemoteClient.h"

#include <iostream>

#include "SafetyLock.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include "Wizard.h"

SocketRemoteClient::SocketRemoteClient(SocketRemoteServer* Server, int Socket)
{
	this->Socket = Socket;
	this->Server = Server;

	std::cout<<"Create remote client! "<<std::endl;

	pthread_mutex_init(&SafeMutex, NULL);
	pthread_create(&ClientThread, NULL, SocketClientCallBack, this);
}

SocketRemoteClient::~SocketRemoteClient()
{
}

std::string SocketRemoteClient::RecieveString()
{
	SafetyLock Lock(&SafeMutex);
	std::string Message;
	int Len = 0;
	if(recv(Socket, &Len, sizeof(Len), 0) < 0)
	{
		close(Socket);
		Socket = 0;
		return Message;
	}

	char* Buffer = (char*) malloc(Len+1);
	Buffer[Len] = 0;
	if(recv(Socket, Buffer, Len, 0)<0)
	{
		close(Socket);
		Socket = 0;
		return Message;
	}
	Message = Buffer;
	free(Buffer);

	return Message;
}

void SocketRemoteClient::SendString(std::string Message)
{
	SafetyLock Lock(&SafeMutex);
	int Len = Message.length();
	write(Socket, &Len, sizeof(Len));
	write(Socket, Message.c_str(), Len);
}

void SocketRemoteClient::Disconnect()
{
	SafetyLock Lock(&SafeMutex);
	close(Socket);
}

void* SocketClientCallBack(void* SocketClientCallBackPtr)
{
	SocketRemoteClient* Client = (SocketRemoteClient*)SocketClientCallBackPtr;
	std::string Message = "OPEN";
	Client->SendString(Message);
	WM_Event Event;
	std::cout<<"Start getting information from client"<<std::endl;
	Message = Client->RecieveString();
	if(Message == "left")
		Event.LeftKey();
	if(Message == "right")
		Event.RightKey();
	if(Message == "up")
		Event.UpKey();
	if(Message == "down")
		Event.DownKey();
	if(Message == "return")
		Event.EnterKey();
	if(Message == "escape")
		Event.EscapeKey();

	Wizard::GetInstance()->GenerateCustomEvent(Event);	
	std::cout<<Message<<std::endl;

	Client->Disconnect();
	return NULL;
}

