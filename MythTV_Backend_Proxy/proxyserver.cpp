/***************************************************************************
 *   Copyright (C) 2005 by igor                                            *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "proxyserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>

#define SOCK_WAITQUEUE_LENGTH 	10

using namespace std;

namespace MYTHTV {

ProxyServer::ProxyServer()
	: port_(0) {
}

ProxyServer::ProxyServer(const char* host, unsigned port) {
	setHost(host);
	setPort(port);
}

ProxyServer::~ProxyServer() {
}

bool 
ProxyServer::handleStartup() {
	if((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cout << "Can't open stream socket.\n" << endl;
		return false;
	}
	
	struct hostent *hent;
	if ((hent = gethostbyname(host_.c_str())) == 0) {
		cout << "Error resolving host name: " << host_ << endl;
		return false;
	}		
	
	struct sockaddr_in serv_addr;
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr = *((in_addr *)hent->h_addr);
	serv_addr.sin_port = htons(port_);
	
	if(bind(sockfd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		cout << "Can't bind local address." << endl;
		return 0;
	}

	cout << "Listening on interface: " <<  host_ << " port: " << port_ << endl;
	
	listen(sockfd_, SOCK_WAITQUEUE_LENGTH);
	return true;
}

void 
ProxyServer::handleTerminate() {
	close(sockfd_);
}

void* 
ProxyServer::_Run() {
	fd_set rfds;
	
	struct sockaddr_in client_addr;
	int len_client_addr = sizeof(client_addr);
	
	while(!isStopRequested()) {
		FD_ZERO(&rfds); 
		FD_SET(sockfd_, &rfds);
		
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
	
		int retval = select(sockfd_ + 1, &rfds, NULL, NULL, &tv);
					
		if(retval <= 0) {
			continue;
		}
		
		if(FD_ISSET(sockfd_, &rfds)) {
			/*accept connection*/
			int client_sockfd = 
				accept(sockfd_, (struct sockaddr *) &client_addr, (socklen_t*)&len_client_addr);
		
			cout << "Connection accepted." << endl;
			handleAccept(client_sockfd);
		} 
	}
	
	return 0;
}

};
