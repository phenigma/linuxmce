//
// C++ Interface: TokenPool 
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef ASTERISKTOKENPOOL_H
#define ASTERISKTOKENPOOL_H

#include "socket.h"
#include "thread.h"
#include "mutex.h"
#include "token.h"

#include <list>

namespace ASTERISK { 

/**
@author 
*/
class TokenPool : public Thread {
public:
    TokenPool();
    virtual ~TokenPool();
	
protected:
    virtual void* _Run();
	
	int sendToken(const Token* ptoken);
	virtual int handleToken(Token* ptoken);
	
	virtual int handleConnect(Socket *psock);
	virtual void handleDisconnect();
	virtual void handleError(int errcode);

private:
    Mutex sm; /*socket access mutex*/
    Socket sock;

    Mutex sqm; /*send queue access mutex*/
    typedef struct __sendrequest {
		Token token;
    } _sendrequest;
    std::list<_sendrequest> sendqueue;
};

};

#endif
