/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#ifndef DCEBUFFEREDIOCONNECTION_H
#define DCEBUFFEREDIOCONNECTION_H

#include <sstream>
#include <string>

#include "IOConnection.h"

namespace DCE {

/**
@author Igor Spac,,,
*/
class BufferedIOConnection : public IOConnection {
public:
    BufferedIOConnection(IOConnection* pconn = NULL);
    BufferedIOConnection(const BufferedIOConnection& conn) {
		operator =(conn);
	}
    virtual ~BufferedIOConnection();
	
public:
	/*proxied members*/
	virtual bool Open() {
		return (pconn_ ? pconn_->Open() : false);
	}
	virtual void Close() {
		if(pconn_ != NULL) {
			pconn_->Close();
		}
	}
	virtual int Send(const char* buff, unsigned int size) {
		return (pconn_ ? pconn_->Send(buff, size) : -1);
	}

	virtual bool isOpened() {
		return (pconn_ ? pconn_->isOpened() : -1);
	}
	virtual bool isDataAvailable(int timeout) {
		return (!buff_.in_avail()) || (pconn_ ? pconn_->isDataAvailable(timeout) : false);
	}

	/*particular members members*/
	virtual int Recv(char* buff, unsigned int size, int timeout);
	virtual std::string Recv(const char* delimbuff, unsigned int delimsize, int timeout);
	virtual void UndoRecv(const char* buff, unsigned int size);

protected:
	int RecvRaw(char* buff, unsigned int size, int timeout);
	int RecvCache(unsigned int size, int timeout);
	unsigned int CacheSize();

	IOConnection* getConnection() {
		return pconn_;
	}
	
public:
	BufferedIOConnection& operator =(const BufferedIOConnection& conn) {
		pconn_ = conn.pconn_;
		/*cache buff is lost*/
		return *this;
	}
	
private:
	IOConnection* pconn_;
	std::stringbuf buff_;
};

};

#endif
