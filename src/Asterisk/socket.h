//
// C++ Interface: socket
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKSOCKET_H
#define ASTERISKSOCKET_H


#include <string>
#include "token.h"

namespace ASTERISK {

	
/**
@author 
*/
class Socket{
public:
    Socket();
    ~Socket();

public:
	int Connect();
	int Disconnect();
	bool isConnected();


	int sendToken(const Token* ptoken);
	int recvToken(Token *ptoken);
	bool isReceivable();

private:
	int recvGreeting();
	std::string recvLine();

private:
	int socket_fd;
	std::string recvtail;
};

};

#endif
