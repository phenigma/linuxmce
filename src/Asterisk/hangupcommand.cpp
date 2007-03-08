/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
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
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hangup command created.");
};

HangupCommand::~HangupCommand() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hangup command destroyed.");
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
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hangup completed.");	
    Release();
}

};
