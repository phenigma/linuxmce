//
// C++ Implementation: LoginCommand
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "logincommand.h"
#include "DCE/Logger.h"
#include "asteriskconsts.h"

using namespace DCE;

namespace ASTERISK {

LoginCommand::LoginCommand() 
{
	g_pPlutoLogger->Write(LV_STATUS, "Login commmand created.");
}

LoginCommand::~LoginCommand() {
	g_pPlutoLogger->Write(LV_STATUS, "Login command destroyed.");
}

void 
LoginCommand::setUserName(std::string username) {
	LOCKED_OP(this->username = username);
}

void 
LoginCommand::setSecret(std::string secret) {
	LOCKED_OP(this->secret = secret);
}

void 
LoginCommand::handleStartup() {
	Token logintok;

	g_pPlutoLogger->Write(LV_STATUS, "Login SM created.!!!!!");
	
	LOCKED_OP(
		logintok.setKey("Action", "login");
		logintok.setKey("Username", username);
		logintok.setKey("Secret", secret);
	);
	
	sendToken(&logintok);

	AddRef();
}


int 
LoginCommand::handleToken(Token* ptoken) {
	int ret = 0;
	if(ptoken->hasKey(TOKEN_RESPONSE)) {
	    ret = 1;
	}
	
	return ret;
}

void
LoginCommand::handleTerminate() {
	g_pPlutoLogger->Write(LV_STATUS, "Login completed.");	
	Release();
}

};
