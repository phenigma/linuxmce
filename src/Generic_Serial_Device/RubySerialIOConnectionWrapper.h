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
#ifndef DCERUBYSERIALIOCONNECTIONWRAPPER_H
#define DCERUBYSERIALIOCONNECTIONWRAPPER_H

#include "BufferedIOConnection.h"

namespace DCE {

/**
@author Igor Spac,,,
*/
class RubySerialIOConnectionWrapper : protected BufferedIOConnection {
public:
    RubySerialIOConnectionWrapper(IOConnection* pconn = NULL);
    virtual ~RubySerialIOConnectionWrapper();

public:
	/*limit access to only these members*/
	int Recv(char* buff, unsigned int size, int timeout) {
		return BufferedIOConnection::Recv(buff, size, timeout);
	}
	std::string 
	RecvDelimited(const char* delimbuff, unsigned int delimsize, int timeout) {
		return BufferedIOConnection::Recv(delimbuff, delimsize, timeout);
	}
	int Send(const char* buff, unsigned int size) {
		return BufferedIOConnection::Send(buff, size);
	}
	bool isDataAvailable(int timeout) {
		return BufferedIOConnection::isDataAvailable(timeout);
	}
};

};

#endif
