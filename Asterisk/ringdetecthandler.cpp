//
// C++ Implementation: ringdetectstatemachine
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ringdetecthandler.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "runtimeconfig.h"
#include "utils.h"


using namespace std;
using namespace DCE;

namespace ASTERISK {

RingDetectHandler::RingDetectHandler()
{
	g_pPlutoLogger->Write(LV_STATUS, "Ring Detect SM created.");
};

RingDetectHandler::~RingDetectHandler() {
	g_pPlutoLogger->Write(LV_STATUS, "Ring Detect SM destroyed.");
};

	
void 
RingDetectHandler::handleStartup() {
	AddRef();
}

int 
RingDetectHandler::handleToken(Token* ptoken) {
	if(ptoken->getKey(TOKEN_EVENT) == EVENT_NEWCHANNEL &&
		ptoken->getKey(TOKEN_STATE) == STATE_RINGING) 
	{
		string channel = ptoken->getKey(TOKEN_CHANNEL);

		g_pPlutoLogger->Write(LV_STATUS, "Channel Ringing: %s", channel.c_str());
		
		string ringphoneid;
		if(!Utils::ParseChannel(channel, &ringphoneid)) {
			if(RuntimeConfig::getInstance()->isCallOriginating(ringphoneid)) {
				g_pPlutoLogger->Write(LV_STATUS, "Phone %s is Origination a call. Skipping issue Ring event.",
							ringphoneid.c_str());
			} else {
				g_pPlutoLogger->Write(LV_STATUS, "Phone %s is Ringing. Fire Ring event.",
							ringphoneid.c_str());
							
				string callerid = ptoken->getKey(TOKEN_CALLERID);
				
				string srcphonenum, srccallername;
				if(!Utils::ParseCallerID(callerid, &srcphonenum, &srccallername)) {
					g_pPlutoLogger->Write(LV_STATUS, "Phone Ringing: %s call from %s", srcphonenum.c_str(), srccallername.c_str());
				} else {	
					g_pPlutoLogger->Write(LV_CRITICAL, "Error parsing callerid:%s", callerid.c_str());
				}
				AsteriskManager* manager = AsteriskManager::getInstance();
				manager->NotifyRing(srccallername, srcphonenum, channel);
			}
		} else {
			g_pPlutoLogger->Write(LV_CRITICAL, "Error parsing channel:%s", channel.c_str());
		}
		
	}
	
	return 0;
}

void 
RingDetectHandler::handleTerminate() {
	Release();
}

};
