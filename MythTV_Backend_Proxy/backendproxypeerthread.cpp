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
#include "backendproxypeerthread.h"

#include <iostream>

#include "token.h"

using namespace std;

namespace MYTHTV {

BackendProxyPeerThread::BackendProxyPeerThread(const char* proxyhost, int srcsockfd, int destsockfd)
	: ProxyPeerThread(proxyhost, srcsockfd, destsockfd) 
{
	cout << "Backend peer thread Created." << endl;
	registerInterceptor(this);
};

BackendProxyPeerThread::~BackendProxyPeerThread() 
{
	unregisterInterceptor(this);
	cout << "Backend peer thread Destroyed." << endl;
}

bool 
BackendProxyPeerThread::processData(ProxyPeerThread* thread, const char* data, bool fromsrc) {
	if(!fromsrc) {
		return false;
	}

	Token tok(data);
	int index;
	if((index = tok.findValue("CHANGE_CHANNEL")) >= 0 && tok.getValuesNum() > index + 1) {
		cout << "Change channel Request received." << endl;
		sleep(5);
		int direction = atoi(tok.getValue(index + 1).c_str());
	} else 
	if((index = tok.findValue("SET_CHANNEL")) >= 0 /*&& tok.getValuesNum() > index + 1*/) {
		cout << "Set channel Request received." << endl;
		sleep(5);
	}
	
	return false;
}

};
