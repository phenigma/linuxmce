//
// C++ Implementation: originatestatemachine
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "hangupcommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

HangupCommand::HangupCommand()
{
	g_pPlutoLogger->Write(LV_STATUS, "Hangup command created.");
};

HangupCommand::~HangupCommand() {
	g_pPlutoLogger->Write(LV_STATUS, "Hangup command destroyed.");
};

void 
HangupCommand::setChannel(std::string channel) {
	LOCKED_OP(this->channel = channel;);
}

void 
HangupCommand::setCommandID(int commandid) {
	LOCKED_OP(this->commandid = commandid;);
}

void
HangupCommand::handleStartup() {
	Token transtok;

	LOCKED_OP(
	transtok.setKey(TOKEN_ACTION, ACTION_HANGUP);
	transtok.setKey(TOKEN_CHANNEL, channel );
	);

	sendToken(&transtok);
	
	AddRef(); // do not destroy until job finished
};

int 
HangupCommand::handleToken(Token* ptoken) {
	return 1;
}

void 
HangupCommand::handleTerminate() {
	g_pPlutoLogger->Write(LV_STATUS, "Hangup completed.");	
    Release();
}

};
