/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#include "BufferedIOConnection.h"

using namespace std;

#define CACHE_READBLOCK_SIZE	128

namespace DCE {

BufferedIOConnection::BufferedIOConnection(IOConnection* pconn)
	: pconn_(pconn)
{
}


BufferedIOConnection::~BufferedIOConnection()
{
}

int 
BufferedIOConnection::RecvRaw(char* buff, unsigned int size, int timeout) {
	return (pconn_ ? pconn_->Recv(buff, size, timeout) : -1);
}

int 
BufferedIOConnection::Recv(char* buff, unsigned int size, int timeout) {
	streamsize sz = buff_.sgetn(buff, size);
	if(sz == 0) {
		return RecvRaw(buff, size, timeout);
	} else {
		if(sz == (streamsize)size) {
			return sz;
		} else {
			return sz + RecvRaw(buff + sz, size - sz, timeout);
		}
	}
}

void 
BufferedIOConnection::UndoRecv(const char* buff, unsigned int size) {
	buff_.sputn(buff, size);
}

int 
BufferedIOConnection::RecvCache(unsigned int size, int timeout) {
	char buff[size];
	int recvsize = RecvRaw(buff, size, timeout);
	if(recvsize > 0) {
		UndoRecv(buff, recvsize);
	}
	return recvsize;
}

unsigned int 
BufferedIOConnection::CacheSize() {
	return buff_.in_avail();
}


std::string
BufferedIOConnection::Recv(const char* delimbuff, unsigned int delimsize, int timeout) 
{
	string ret;
	if(delimsize <= 0 || !isDataAvailable(timeout)) {
		return ret;
	}
	
	unsigned delimindex = 0;
	while(delimindex < delimsize) {
		if(CacheSize() == 0) {
			RecvCache(CACHE_READBLOCK_SIZE, 0);
		}
		
		char c;
		int rsize = Recv(&c, sizeof(char), timeout);
		if(rsize <= 0) {
			if(delimindex > 0) {
				UndoRecv(ret.c_str(), delimindex);
			}
			return ret;
		}

		if(c == delimbuff[delimindex]) {
			delimindex++;
		} else {
			if(delimindex > 0) {
				delimindex = 0;
				ret.append(delimbuff, delimindex);
				UndoRecv(&c, 1);
			} else {
				ret += c;
			}
		}
	}
	return ret;
}

};
