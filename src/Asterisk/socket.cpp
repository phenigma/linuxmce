//
// C++ Implementation: socket
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "socket.h"
#include "DCE/Logger.h"

#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;
using namespace DCE;


#define ASMANAGER_PORT	5038
#define ASMANAGER_HOST	"127.0.0.1"

namespace ASTERISK {

Socket::Socket() : socket_fd(0) {
}


Socket::~Socket()
{
}

int Socket::Connect() {
	int ret = 0;
	
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd > 0) {
		g_pPlutoLogger->Write(LV_STATUS, "socket created.");
	} else {
		socket_fd = 0;
		g_pPlutoLogger->Write(LV_CRITICAL, "socket creation failed.");
		return -1;
	}
	
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
		
	address.sin_family = AF_INET;
	address.sin_port = htons(ASMANAGER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
	
	
	
	if((ret = connect(socket_fd, (struct sockaddr*)&address, sizeof(address)))) {
		g_pPlutoLogger->Write(LV_CRITICAL, "failed connecting to asterisk manager");
		socket_fd = 0;
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "connected to asterisk manager");
		recvGreeting();
	}
	
	return ret;	
}

int Socket::Disconnect() {
	if(!socket_fd) {
		return -1;
	}
	close(socket_fd);
	socket_fd = 0;
	return 0;
};

bool Socket::isConnected() {
	return socket_fd != 0;
}

int Socket::sendToken(const Token* ptoken) {
	if(!socket_fd) {
		return -1;
	}
	string str = ptoken->serialize();
	if(!str.empty()) {
		g_pPlutoLogger->Write(LV_STATUS, "sending command:\n%s", str.c_str());
		if(send(socket_fd, str.c_str(), str.length(), 0) > 0) {
			g_pPlutoLogger->Write(LV_STATUS, "command sent successfully.");
			return 0;
		} else {
			g_pPlutoLogger->Write(LV_CRITICAL, "failed sending command.");
			return -1;
		}
	} else {
		g_pPlutoLogger->Write(LV_CRITICAL, "Ignored sending empty token.");		
	}
	return -1;
}

bool Socket::isReceivable() {
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(socket_fd, &rfds);
	
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10*1000;
	
	if(select((int)(socket_fd + 1), &rfds, 0, 0, &tv) > 0)  {
		return true;
	} else {
		return false;
	}
}

#define RECV_BUFFER_SIZE  128

int Socket::recvGreeting() {
	char recvbuff[RECV_BUFFER_SIZE];

	string recvstr;
	while(1) {
		int recvlen = recv(socket_fd, recvbuff, RECV_BUFFER_SIZE - 1, 0);	
		if(recvlen > 0) {
			recvbuff[recvlen] = 0;
			recvstr += recvbuff;
			if(recvlen < RECV_BUFFER_SIZE - 1) {
				break;
			}
		} else {
			break;
		}
	}

	if(recvstr.empty()) {
		g_pPlutoLogger->Write(LV_CRITICAL, "failed receiving greeting");
		return -1;
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "received greeting:\n%s", recvstr.c_str());
	}

	return 0;
}

string Socket::recvLine() {
	string recvline;
	char recvbuff[RECV_BUFFER_SIZE];
	
	while(1) {
		if(!recvtail.empty()) {
			int lineend = recvtail.find('\x0A', 0);
			if(lineend != -1) {
				recvline.append(recvtail.substr(0, lineend + 1));
				recvtail.erase(0, lineend + 1);
				break;
			} else {
				recvline.append(recvtail);
				recvtail.clear();
			}
		} else {
			int recvlen = recv(socket_fd, recvbuff, RECV_BUFFER_SIZE - 1, 0);
			if(recvlen > 0) {
				recvbuff[recvlen] = 0;
				recvtail.append(recvbuff);
			} else {
				break;
			}
		}
	}
	
	return recvline;
}

int Socket::recvToken(Token* ptoken) {
	string recvreply, recvline;
	while(1) {
		recvline = recvLine();
		//g_pPlutoLogger->Write(LV_STATUS, "received line:\n%s", recvline.c_str());
		if(recvline.empty() || recvline == "\x0D\x0A") {
			break;
		}
		recvreply += recvline;
	}

	if(!recvreply.empty()) {
		g_pPlutoLogger->Write(LV_STATUS, "received message:\n%s", recvreply.c_str());
		if(!ptoken->unserialize(recvreply)) {
			return 0;
		} else {
			g_pPlutoLogger->Write(LV_CRITICAL, "failed unserializing message.");
		}
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "failed receiving message:\n%s", recvreply.c_str());
	}
	
	return -1;	
}

}
