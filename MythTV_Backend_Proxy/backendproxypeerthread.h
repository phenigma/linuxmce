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
#ifndef MYTHTVBACKENDPROXYPEERTHREAD_H
#define MYTHTVBACKENDPROXYPEERTHREAD_H

#include "proxypeerthread.h"

namespace MYTHTV {

/**
@author igor
*/
class BackendProxyPeerThread : public ProxyPeerThread, public ProxyPeerInterceptor
{
public:
    BackendProxyPeerThread(const char* proxyhost, int srcsockfd, int destsockfd);
	~BackendProxyPeerThread();

protected:
	virtual bool processData(ProxyPeerThread* thread, const char* data, bool fromsrc);
	
};

};

#endif
