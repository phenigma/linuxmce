//
// C++ Implementation: asteriskmanager
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "asteriskmanager.h"
#include "DCE/Logger.h"

#include "Asterisk.h"

#include "logincommand.h"
#include "originatecommand.h"
#include "hangupcommand.h"
#include "ringdetecthandler.h"

#define DEFAUL_LOGIN_NAME "admin"
#define DEFAUL_LOGIN_SECRET "adminsecret"

#define KEY_RESPONSE "Response"
#define KEY_MESSAGE "Message"

#define RESPONSE_SUCCESS "Success"
#define RESPONSE_ERROR "Error"

using namespace DCE;

namespace ASTERISK {

AsteriskManager* AsteriskManager::sinstance = 0;


AsteriskManager::AsteriskManager() {
}

AsteriskManager::~AsteriskManager() {
	Release();
}


void AsteriskManager::Originate(const string sPhoneNumber, const string sOriginatorNumber,
				const string sOriginatorType, const string sCallerID, int iCommandID) {
	
	/*originate*/
	TokenSMPtr<OriginateCommand> poriginate = OriginateCommand::getInstance();
	poriginate->setPhoneNum(sOriginatorNumber);
	poriginate->setPhoneType(sOriginatorType);
	poriginate->setExtenNum(sPhoneNumber);
	poriginate->setCallerID(sCallerID);
	poriginate->setCommandID(iCommandID);

	poriginate->Run(false);
}

void AsteriskManager::Hangup(const std::string sChannel, 
				int iCommandID) {
	
	/*hangup*/
	TokenSMPtr<HangupCommand> phangup = HangupCommand::getInstance();
	phangup->setChannel(sChannel);
	phangup->setCommandID(iCommandID);

	phangup->Run(false);
}
			
	
void AsteriskManager::NotifyRing(std::string sCallerID, const std::string sSrcExt, 
												const std::string sChannel) {
	if(!pAsterisk) {
	    return;
	}
	pAsterisk->EVENT_PBX_Ring(sSrcExt, sChannel, sCallerID);
}

void AsteriskManager::NotifyResult(int iCommandID, int iResult, 
						const std::string Message) 
{	
	if(!pAsterisk) {
	    return;
	}
	pAsterisk->EVENT_PBX_CommandResult(iCommandID, iResult, Message);
}


AsteriskManager* AsteriskManager::getInstance() {
	static AsteriskManager manager;
	
	LOCK_OBJ(manager);
	
	if(sinstance == 0) {
		sinstance = &manager;
	}
	
	UNLOCK_OBJ(manager);
	
	return sinstance;
}

void AsteriskManager::Initialize(DCE::Asterisk* lpAsterisk) {
	this->pAsterisk = lpAsterisk;

	/*login*/
	/* removed - login is handled automaticaly
	TokenSMPtr<LoginCommand> plogin = LoginCommand::getInstance();
	plogin->setUserName(DEFAUL_LOGIN_NAME);
	plogin->setSecret(DEFAUL_LOGIN_SECRET);
	if(plogin->Run()) {
	       g_pPlutoLogger->Write(LV_CRITICAL, "Login to asterisk manager failed");
	       return;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Login successfull.");
	*/


    /*initialize persistent SMs*/
	TokenSMPtr<RingDetectHandler> pring = RingDetectHandler::getInstance();
	pring->Run(false);
}

void AsteriskManager::Release() {
	this->pAsterisk = 0;
}
		

};
