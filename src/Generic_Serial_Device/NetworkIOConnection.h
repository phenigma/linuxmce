//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCENETWORKIOCONNECTION_H
#define DCENETWORKIOCONNECTION_H

#include <string>

#include "IOConnection.h"

namespace DCE {

/**
@author Igor Spac,,,
*/
class NetworkIOConnection : public IOConnection {
public:
    NetworkIOConnection();
    virtual ~NetworkIOConnection();

public:
	void setHost(const char* host) {
		host_ = host;
	}
	const char* getHost() {
		return host_.c_str();
	}
	
	void setPort(int port) {
		port_ = port;
	}
	int getPort() {
		return port_;
	}

public:
	virtual bool Open();
	virtual void Close();
	
	virtual int Send(const char* buff, unsigned int size);
	virtual int Recv(char* buff, unsigned int size, int timeout);

	virtual bool isOpened();
	virtual bool isDataAvailable(int timeout);
	
private:
	std::string host_;
	int port_;
	int sockfd_;
};

};

#endif
