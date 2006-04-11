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
#include "conferencecommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

ConferenceCommand::ConferenceCommand()
{
	g_pPlutoLogger->Write(LV_STATUS, "Conference command created.");
};

ConferenceCommand::~ConferenceCommand() {
	g_pPlutoLogger->Write(LV_STATUS, "Conference command destroyed.");
};

void 
ConferenceCommand::setExtenNum(std::string extennum) {
	LOCKED_OP(this->extennum = extennum);
}

void 
ConferenceCommand::setChannel1(std::string channel) {
	LOCKED_OP(this->channel1 = channel;);
}

void 
ConferenceCommand::setChannel2(std::string channel) {
	LOCKED_OP(this->channel2 = channel;);
}

void 
ConferenceCommand::setCommandID(int commandid) {
	LOCKED_OP(this->commandid = commandid;);
}

void
ConferenceCommand::handleStartup() {
	Token transtok;
	
	LOCKED_OP(
	transtok.setKey(TOKEN_ACTION, ACTION_REDIRECT);
	transtok.setKey(TOKEN_CHANNEL, channel1);
	transtok.setKey(TOKEN_EXTRACHANNEL, channel2);
	transtok.setKey(TOKEN_EXTEN, extennum);
	
	transtok.setKey(TOKEN_CONTEXT, "trusted");
	transtok.setKey("Priority", "1");
	);

	sendToken(&transtok);
	
	AddRef(); // do not destroy until job finished
};

int 
ConferenceCommand::handleToken(Token* ptoken) {
	return 0;
}

void 
ConferenceCommand::handleTerminate() {
	g_pPlutoLogger->Write(LV_STATUS, "Conference completed.");	
    Release();
}

};
