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
#ifndef MYTHTVPROXYPEERTHREAD_H
#define MYTHTVPROXYPEERTHREAD_H

#include <list>
#include <string>

#include "proxypeer.h"
#include "thread.h"

#include "proxypeerinterceptor.h"

namespace MYTHTV {


/**
@author igor
*/
class ProxyPeerThread : public ProxyPeer, public Thread
{
public:
    ProxyPeerThread(const char* proxyhost, int srcsockfd, int destsockfd);
    virtual ~ProxyPeerThread();

protected:
	virtual void* _Run();

public:
	inline int writeDataToSrc(const std::string& data) {
		return writeData(getSrcSock(), data);
	}
	inline int readDataFromSrc(std::string& data) {
		return readData(getSrcSock(), data);
	}
	inline int writeDataToDest(const std::string& data) {
		return writeData(getDestSock(), data);
	}
	inline int readDataFromDest(std::string& data) {
		return readData(getDestSock(), data);
	}

	void registerInterceptor(ProxyPeerInterceptor* pintor);
	void unregisterInterceptor(ProxyPeerInterceptor* pintor);

protected:
	int readData(int sockfd, std::string& data);
	int writeData(int sockfd, const std::string& data);

private:
	int replData(bool fromsrc);
	virtual void handleTerminate();

private:
	typedef std::list<ProxyPeerInterceptor*> PEERINTERCEPTORLIST;
	PEERINTERCEPTORLIST interceptors_;
};

};

#endif
