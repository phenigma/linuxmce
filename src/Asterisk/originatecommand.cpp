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
#include "originatecommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "runtimeconfig.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

OriginateCommand::OriginateCommand()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Originate command created.");
};

OriginateCommand::~OriginateCommand() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Originate command destroyed.");
};

void 
OriginateCommand::setPhoneNum(std::string phonenum) {
	LOCKED_OP(this->phonenum = phonenum);
}

void 
OriginateCommand::setPhoneType(std::string phonetype) {
	LOCKED_OP(
		this->phonetype = phonetype;
		// SIP5061 -> SIP
		if( 3 < this->phonetype.size() &&
			"SIP" == this->phonetype.substr(0, 3) )
		{
			this->phonetype = "SIP";
		}
	);
}

void 
OriginateCommand::setExtenNum(std::string extennum) {
	LOCKED_OP(this->extennum = extennum;);
}

void 
OriginateCommand::setCallerID(std::string callerid) {
	LOCKED_OP(this->callerid = callerid;);
}

void 
OriginateCommand::setCommandID(int commandid) {
	LOCKED_OP(this->commandid = commandid;);
}

void
OriginateCommand::handleStartup() {
	Token origtok;

	/*register the channel as originating 
		*so no ring event will be issued to Telecom Plugin */
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Registered phone %s as originator of the call. ",
						phonenum .c_str());
	RuntimeConfig::getInstance()->regOriginateCall(phonenum);
	
	LOCKED_OP(
	origtok.setKey(TOKEN_ACTION, ACTION_ORIGINATE);
	origtok.setKey(TOKEN_CHANNEL, phonetype + "/" + phonenum );
	origtok.setKey(TOKEN_EXTEN, extennum);
	origtok.setKey(TOKEN_CONTEXT, "trusted");
	origtok.setKey(TOKEN_PRIORITY, "1");
//	origtok.setKey("Timeout", "45000");
	origtok.setKey(TOKEN_CALLERID, callerid);
	);

	sendToken(&origtok);
	
	AddRef(); // do not destroy until job finished
};

int 
OriginateCommand::handleToken(Token* ptoken) {
	/*first espect newchannel to get the unquieid*/
	if(ptoken->hasKey(TOKEN_EVENT) 
			&& ptoken->getKey(TOKEN_EVENT) == EVENT_NEWCHANNEL) 
	{
		string phNum, phType;
		string channel = ptoken->getKey(TOKEN_CHANNEL);
		
		Utils::ParseChannel(channel, &phNum, &phType);
		if(srcchannel.empty()) {
			/*step1*/
			if(phNum == phonenum && phType == phonetype) {
				srcchannel = channel;
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Originate source channel: %s.", channel.c_str());
			}
		} else {
			/*step3*/
			if(phNum == destphonenum && phType == destphonetype) {
				/*success*/
				destchannel = channel;
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Originate destination channel: %s.", channel.c_str());
				
				AsteriskManager::getInstance()->NotifyResult(commandid, 0, srcchannel);
				return 1;
			} else {
			}
		}
	} else 
	if(ptoken->hasKey(TOKEN_EVENT) 
			&& ptoken->getKey(TOKEN_EVENT) == EVENT_NEWEXTEN 
			&& ptoken->getKey(TOKEN_APPLICATION) == APPLICATION_DIAL) 
	{
		if(ptoken->getKey(TOKEN_CHANNEL) == srcchannel)	 {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unregistered phone %s as originator of the call. ",
						phonenum .c_str());
			RuntimeConfig::getInstance()->regOriginateCall(phonenum);
			
			/*step2*/
			string data = ptoken->getKey(TOKEN_APPDATA);
	
			/*get type and phonenum*/
			string::size_type curpos;
			destphonetype = StringUtils::Tokenize(data, "/|-", curpos);
			destphonenum = StringUtils::Tokenize(data, "/|-", curpos);
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "DestPhoneNum: %s, DestPhoneType: %s. (AppData: %s). ",
								destphonenum.c_str(), destphonetype.c_str(), data.c_str());
		}
	} else 
	if(ptoken->hasKey(TOKEN_EVENT) 
			&& ptoken->getKey(TOKEN_EVENT) == EVENT_HANGUP) 
	{
		if(ptoken->getKey(TOKEN_CHANNEL) == srcchannel)	 {
			/*error*/
			AsteriskManager::getInstance()->NotifyResult(commandid, -1, "");
			return 1;
		}
	}
	else
	if(ptoken->hasKey(TOKEN_RESPONSE)) {
		if(ptoken->getKey(TOKEN_RESPONSE) != RESPONSE_SUCCESS) {
			/*error*/
			AsteriskManager::getInstance()->NotifyResult(commandid, -1, destchannel);
			return 1;
		}
	}
	
	
	
	
		/*
		
		
		if(!Utils::ParseChannel(channel, &phonenum) && phonenum == phonenum) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Notifying successfull originate.");
			AsteriskManager* manager = AsteriskManager::getInstance();
			manager->NotifyResult(commandid, !(ptoken->getKey(TOKEN_RESPONSE) == 		RESPONSE_SUCCESS), ptoken->getKey(TOKEN_MESSAGE));
		}
			
		*/
		
		//AsteriskManager::getInstance()->NotifyResult(commandid, 0, "SIP/TEST");
	
	return 0;
}

void 
OriginateCommand::handleTerminate() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Originate completed.");
    Release();
}

};
