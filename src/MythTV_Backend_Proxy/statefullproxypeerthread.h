//
// C++ Interface: statefullproxypeerthread
//
// Description: 
//
//
// Author: igor <igor@dexx>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MYTHTVSTATEFULLPROXYPEERTHREAD_H
#define MYTHTVSTATEFULLPROXYPEERTHREAD_H

#include "proxypeerthread.h"

namespace MYTHTV {

#define PEER_STATE_NONE		0

/**
@author igor
*/
class StatefullProxyPeerThread : public ProxyPeerThread {
public:
    StatefullProxyPeerThread(ProxyServer *pserver, int srcsockfd, int destsockfd);
    ~StatefullProxyPeerThread();

public:
	inline void setState(int state) {
		state_ = state;
	}
	inline int getState() {
		return state_;
	}

private:
	int state_;
};

};

#endif
