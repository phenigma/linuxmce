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
	if(ptoken->getKey(TOKEN_EVENT) == EVENT_NEWEXTEN &&
		ptoken->getKey(TOKEN_APPLICATION) == APPLICATION_DIAL)
	{
		string party = ptoken->getKey(TOKEN_APPDATA);
		string extension;
		if(!Utils::ParseParty(party, &extension)) {	
			string channel = ptoken->getKey(TOKEN_CHANNEL);
			map_ringext[extension] = channel;
			g_pPlutoLogger->Write(LV_STATUS, "Will connect channel %s to extension %s", channel.c_str(),extension.c_str());
		} else {
			g_pPlutoLogger->Write(LV_CRITICAL, "Error parsing party:%s", party.c_str());
		}
		
	}
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
				if(map_ringext.find(ringphoneid) == map_ringext.end())
				{
					g_pPlutoLogger->Write(LV_CRITICAL, "No previos ring to this channel !!!");
					return 0;
				}
				g_pPlutoLogger->Write(LV_STATUS, "Phone %s is Ringing. Fire Ring event.",
							ringphoneid.c_str());
							
				string callerid = ptoken->getKey(TOKEN_CALLERID);
				
				AsteriskManager* manager = AsteriskManager::getInstance();
				manager->NotifyRing(callerid, ringphoneid, map_ringext[ringphoneid]);
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
