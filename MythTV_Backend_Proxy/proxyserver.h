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
#ifndef MYTHTVPROXYSERVER_H
#define MYTHTVPROXYSERVER_H

#include <string>

#include "thread.h"

namespace MYTHTV {

/**
@author igor
*/

class ProxyServer : public Thread
{
public:
    ProxyServer();
    ProxyServer(const char* host, unsigned port);
    ~ProxyServer();

	void setHost(const char* host) {
		host_ = host;
	}
	const char* getHost() {
		return host_.c_str();
	}
	
	void setPort(unsigned port) {
		port_ = port;
	}
	unsigned getPort() {
		return port_;
	}
	
protected:
	virtual void* _Run();

protected:
	virtual bool handleStartup();
	virtual void handleTerminate();
	
	virtual void handleAccept(int sockfd) = 0;

private:
	std::string host_;
	unsigned port_;

private:
	int sockfd_;
};

};

#endif
