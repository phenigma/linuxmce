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
#include "backendproxyserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>

#include "constants.h"
#include "backendproxypeerthread.h"

using namespace std;

namespace MYTHTV {

BackendProxyServer::BackendProxyServer()
	: peerport_(0)
{
	cout << "Backend proxy Created." << endl;
};

BackendProxyServer::~BackendProxyServer() {
	cout << "Backend proxy Destroyed." << endl;
}

void 
BackendProxyServer::handleAccept(int sockfd) {
	/*open peer connection to backend*/
	struct hostent *hent;
	if ((hent = gethostbyname(peerhost_.c_str())) == 0) {
		cout << "Error resolving host name: " << peerhost_ << endl;
		return;
	}		
	
	cout << "Connecting to Backend server." << endl;
	
	int backend_sockfd = 
		socket(AF_INET, SOCK_STREAM, 0);
	
	sockaddr_in backend_addr;
	backend_addr.sin_family = AF_INET;
	backend_addr.sin_port = htons(peerport_);
	backend_addr.sin_addr = *((in_addr *)hent->h_addr);
	memset(&(backend_addr.sin_zero), 0, 8); 

	if (::connect(backend_sockfd, (sockaddr *) &backend_addr, sizeof(sockaddr))) {
		cout << "Error connecting to Backend server: " << peerhost_ << "on port: " << peerport_ << endl;
		return;
	}		
	
	/*instantiate peer handler*/
	ProxyPeerThread *ppeerthr = 
			new BackendProxyPeerThread(getHost(), sockfd, backend_sockfd);
	ppeerthr->Run(false);
}


};
