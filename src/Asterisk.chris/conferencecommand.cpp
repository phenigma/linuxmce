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
#include "conferencecommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

ConferenceCommand::ConferenceCommand()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Conference command created.");
};

ConferenceCommand::~ConferenceCommand() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Conference command destroyed.");
};

void 
ConferenceCommand::setExtenNum(std::string extennum) {
	this->extennum = extennum;
}

void 
ConferenceCommand::setChannel1(std::string channel) {
	this->channel1 = channel;
}

void 
ConferenceCommand::setChannel2(std::string channel) {
	this->channel2 = channel;
}

void 
ConferenceCommand::setCommandID(int commandid) {
	this->commandid = commandid;
}

void
ConferenceCommand::handleStartup() {
	m_token.setKey(TOKEN_ACTION, ACTION_REDIRECT);
	m_token.setKey(TOKEN_CHANNEL, channel1);
	m_token.setKey(TOKEN_EXTRACHANNEL, channel2);
	m_token.setKey(TOKEN_EXTEN, extennum);
	
	m_token.setKey(TOKEN_CONTEXT, "trusted");
	m_token.setKey("Priority", "1");
};

bool 
ConferenceCommand::handleResponse(Token &token){
	//TODO: handle response
	return true;
}

void 
ConferenceCommand::handleTerminate() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Conference completed.");	
}

};
