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
#include "TransferCommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

TransferCommand::TransferCommand()
{
	g_pPlutoLogger->Write(LV_STATUS, "Transfer command created.");
};

TransferCommand::~TransferCommand() {
	g_pPlutoLogger->Write(LV_STATUS, "Transfer command destroyed.");
};

void 
TransferCommand::setExtenNum(std::string extennum) {
	LOCKED_OP(this->extennum = extennum);
}

void 
TransferCommand::setChannel(std::string channel) {
	LOCKED_OP(this->channel = channel;);
}

void 
TransferCommand::setCommandID(int commandid) {
	LOCKED_OP(this->commandid = commandid;);
}

void
TransferCommand::handleStartup() {
	Token transtok;

	LOCKED_OP(
	transtok.setKey(TOKEN_ACTION, ACTION_REDIRECT);
	transtok.setKey(TOKEN_CHANNEL, channel );
	transtok.setKey(TOKEN_EXTENSION, extennum);
	transtok.setKey("Priority", "1");
	);

	sendToken(&transtok);
	
	AddRef(); // do not destroy until job finished
};

int 
TransferCommand::handleToken(Token* ptoken) {
	return 0;
}

void 
TransferCommand::handleTerminate() {
	g_pPlutoLogger->Write(LV_STATUS, "Transfer completed.");	
    Release();
}

};
