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
#include "masterproxyserver.h"
#include "constants.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>

#include "masterproxypeerthread.h"

using namespace std;

namespace MYTHTV {

MasterProxyServer::MasterProxyServer(const char* host, unsigned port)
	: ProxyServer(host, port) {
	cout << "Master proxy Created." << endl;
};

MasterProxyServer::~MasterProxyServer() {
	cout << "Master proxy Destroyed." << endl;
}
void 
MasterProxyServer::handleAccept(int sockfd) {
	/*open peer connection to backend*/
	struct hostent *hent;
	if ((hent = gethostbyname(MASTER_BACKEND_HOST)) == 0) {
		cout << "Error resolving host name: " << MASTER_BACKEND_HOST << endl;
		return;
	}		
	
	cout << "Connecting to Master server." << endl;
	
	int backend_sockfd = 
		socket(AF_INET, SOCK_STREAM, 0);
	
	sockaddr_in backend_addr;
	backend_addr.sin_family = AF_INET;
	backend_addr.sin_port = htons(MASTER_BACKEND_PORT);
	backend_addr.sin_addr = *((in_addr *)hent->h_addr);
	memset(&(backend_addr.sin_zero), 0, 8); 

	if (::connect(backend_sockfd, (sockaddr *) &backend_addr, sizeof(sockaddr))) {
		cout << "Error connecting to Backend server: " << MASTER_BACKEND_HOST  << "on port: " << MASTER_BACKEND_PORT << endl;
		return;
	}		
	
	/*instantiate peer handler*/
	ProxyPeerThread *ppeerthr = 
			new MasterProxyPeerThread(getHost(), sockfd, backend_sockfd);
	ppeerthr->Run(false);
}


};
