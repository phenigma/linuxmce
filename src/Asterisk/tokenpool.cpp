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
// C++ Implementation: TokenPool 
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tokenpool.h"
#include "DCE/Logger.h"
#include "asteriskconsts.h"
#include "PlutoUtils/CommonIncludes.h"

using namespace std;
using namespace DCE;

#define POOL_SLEEP_PERIOD		1000
#define POOL_RECONNECT_PERIOD	5000


#define DEFAUL_LOGIN_NAME "admin"
#define DEFAUL_LOGIN_SECRET "adminsecret"

namespace ASTERISK {

TokenPool::TokenPool() {
}


TokenPool::~TokenPool() {
}

int 
TokenPool::handleConnect(Socket* psock) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Login to asterisk manager.");
			
	Token logintok;
	logintok.setKey("Action", "login");
	logintok.setKey("Username", DEFAUL_LOGIN_NAME);
    logintok.setKey("Secret", DEFAUL_LOGIN_SECRET);

	Token recvtok;
	if(!psock->sendToken(&logintok) && !psock->recvToken(&recvtok)) {
		if(recvtok.hasKey(TOKEN_RESPONSE)) {
			if(recvtok.getKey(TOKEN_RESPONSE) == RESPONSE_SUCCESS) {
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Login to asterisk manager successfull.");
				return 0;
			}
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed login to asterisk manager.");
	return -1;
}

void
TokenPool::handleDisconnect() {
	sqm.Lock();
	sendqueue.clear();
	sqm.Unlock();
}

void 
TokenPool::handleError(int errcode) {
}

int 
TokenPool::handleToken(Token *ptoken) {
    return 0;
}

void* 
TokenPool::_Run() {
	Socket sock;

	bool bFirstConnect = true;
	int errcode;

	while(1) {
		if(!sock.isConnected()) {
			if(bFirstConnect) {
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Waiting %d seconds before reconnect.", POOL_RECONNECT_PERIOD);
				Sleep(POOL_RECONNECT_PERIOD);
			} else {
				bFirstConnect = false;
			}
			/*connect to asterisk manager*/
			if(!sock.Connect() && !handleConnect(&sock)) {
			
			} else {
				continue;
			}
		}
		else
		{
			/*first try process a send item*/
			bool dosend = false;


			/*get the req from queue*/
			_sendrequest req;
			sqm.Lock();
			if(sendqueue.size() > 0) {
				req = *sendqueue.begin();
				sendqueue.pop_front();
				dosend = true;
			}
			sqm.Unlock();

			if(dosend) {
				errcode = sock.sendToken(&req.token);
            	if(errcode != 0) {
			    	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error occured while sending token. Disconnecting.");
					break;
				} else {
				}
			}	

			/*receive*/
			 bool doreceive = sock.isReceivable();
        	 if(doreceive) {
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Receiving token.");
				Token token;
            	errcode = sock.recvToken(&token);
	        	if(errcode != 0) {
		        	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error occured while receiving token. Disconnecting."); 
					break;
		    	}

				/*process received token*/
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Notification of new token.");
				if(handleToken(&token)) {
					break;
				}
			 } else {
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "No traffic in/to asterisk manager. Sleeping for %d seconds", POOL_SLEEP_PERIOD);
				Sleep(POOL_SLEEP_PERIOD);
			 }
		 }
	}

	sock.Disconnect();
	if(errcode != 0) {
		handleError(errcode);
	}
	handleDisconnect();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Token pool thread ended.");

	return 0;
}

int 
TokenPool::sendToken(const Token* ptoken) {
	sqm.Lock();
	
	_sendrequest req = {*ptoken};
	sendqueue.push_back(req);

	sqm.Unlock();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Token pushed into send queue.");
	return 0;
}

}
