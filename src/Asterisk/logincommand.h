//
// C++ Interface: loginstatemachine
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LOGINSTATEMACHINE_H
#define LOGINSTATEMACHINE_H

#include "tokenmanager.h"
#include "condition.h"

#include <string>

namespace ASTERISK {

/**
@author 
*/
class LoginCommand : public TokenManager<LoginCommand>, public ThreadSafeObj {
	friend class TokenManager<LoginCommand>;
protected:
	LoginCommand();
	~LoginCommand();
public:
	void setUserName(std::string username);
	void setSecret(std::string secret);

public:
	virtual int handleToken(Token* ptoken);
	
	virtual void handleStartup();
	virtual void handleTerminate();

private:
	std::string username;
	std::string secret;
};

}

#endif
