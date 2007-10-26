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
#include "communicationhandler.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "runtimeconfig.h"
#include "utils.h"

#define MARK_DIALING " DIAL/"

using namespace std;
using namespace DCE;

namespace ASTERISK {

CommunicationHandler::CommunicationHandler()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ring Detect SM created.");
};

CommunicationHandler::~CommunicationHandler() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ring Detect SM destroyed.");
};

int 
CommunicationHandler::handleToken(Token* ptoken, bool& bIsResponseToken) {

	bIsResponseToken = false;
	if(ptoken->hasKey(TOKEN_RESPONSE)) {
		bIsResponseToken = true;
		return 1;
	}
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_NEWEXTEN)
		return handleNewextenEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_HANGUP) 
		return handleHangupEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_LINK) 
		return handleLinkEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_NEWCHANNEL)
		return handleNewChannelEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_NEWSTATE)
		return handleNewStateEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_PEER_STATUS)
		return handlePeerStatusChanged(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_EXTEN_STATUS)
		return handleExtensionStatusChanged(ptoken);

	return 0;
}

int 
CommunicationHandler::handleNewextenEvent(Token* ptoken) {

	if(ptoken->getKey(TOKEN_APPLICATION) == APPLICATION_DIAL)
	{
		string party = ptoken->getKey(TOKEN_APPDATA);
		while (party != "")
		{
			string rest=party;
			string extension;
			if(!Utils::ParseParty(party, &extension,&rest)) {
				if(!extension.empty())
				{
					string channel = ptoken->getKey(TOKEN_CHANNEL);
					map_ringext[extension] += string(" ")+channel;
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Will connect channel %s to extension %s", channel.c_str(),extension.c_str());
				}
				else
				{
					string channel = ptoken->getKey(TOKEN_CHANNEL);
					string ringphoneid;
					if(!Utils::ParseChannel(channel, &ringphoneid))
					{
						if(map_ringext.find(ringphoneid) != map_ringext.end())
						{
							int pos = (int)party.rfind('/');
							if(pos>=0)
							{
								string number = party.substr(pos+1,party.length());
								map_ringext[ringphoneid] += string(MARK_DIALING)+number+string("/ ");
								LoggerWrapper::GetInstance()->Write(LV_STATUS, "Will mark channel %s as dialing to %s", channel.c_str(),number.c_str());								
							}
						}
					}
				}
			} else {
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error parsing party:%s", party.c_str());
				return 0;
			}
			party = rest;
		}
	}
	else if(ptoken->getKey(TOKEN_APPLICATION) == APPLICATION_CONF)
	{
		string channel1 = ptoken->getKey(TOKEN_CHANNEL);
		string extension = ptoken->getKey(TOKEN_EXTENSION);
		string ringphoneid;
		if(Utils::ParseChannel(channel1, &ringphoneid)) {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not extract phone id from channels");
			return 0;
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Conference event in room %s with channel %s", extension.c_str(),channel1.c_str());		
		if(map_ringext.find(extension) == map_ringext.end())
		{
			map_ringext[extension] = string("C")+extension;
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Change from %s",map_ringext[extension].c_str());
		string channel2 = StringUtils::TrimSpaces(map_ringext[extension]);
		string callerid2 = map_callerid[extension]; 
		map_ringext[extension] += string(" ")+channel1;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "         to %s",map_ringext[extension].c_str());

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ringing from channel %s (%s) to channel %s (%s) ",
			channel1.c_str(), ringphoneid.c_str(), channel2.c_str(), callerid2.c_str());
		AsteriskManager::getInstance()->NotifyRing(channel1, channel2, ringphoneid, callerid2);
	}

	return 0;
}

int 
CommunicationHandler::handleHangupEvent(Token* ptoken) 
{
	string channel = ptoken->getKey(TOKEN_CHANNEL);
	string reason = ptoken->getKey(TOKEN_REASON);

	string ringphoneid;
	if(!Utils::ParseChannel(channel, &ringphoneid))
		map_ringext.erase(ringphoneid);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hangup: %s, %d, %d, %d", channel.c_str(), reason.c_str());
	AsteriskManager::getInstance()->NotifyHangup(channel, reason);

	return 0;
}

int 
CommunicationHandler::handleLinkEvent(Token* ptoken) 
{
	string channel1 = ptoken->getKey(TOKEN_CHANNEL1);
	string channel2 = ptoken->getKey(TOKEN_CHANNEL2);
	string callerid1 = ptoken->getKey(TOKEN_CALLERID1);
	string callerid2 = ptoken->getKey(TOKEN_CALLERID2);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Linked channel %s (%s) to channel %s (%s) ",
        channel1.c_str(), callerid1.c_str(), channel2.c_str(), callerid2.c_str());
	AsteriskManager::getInstance()->NotifyLink(channel1, channel2, callerid1, callerid2);

	return 0;
}

int 
CommunicationHandler::handleNewStateEvent(Token* ptoken)
{
	//TODO: do we need this ?

	return 0;
}

int 
CommunicationHandler::handleNewChannelEvent(Token* ptoken)
{
	if(ptoken->getKey(TOKEN_STATE) == STATE_RING)
	{
		string channel1 = ptoken->getKey(TOKEN_CHANNEL);
		string callerid1 = ptoken->getKey(TOKEN_CALLERID);        

		string ringphoneid;
		if(!Utils::ParseChannel(channel1, &ringphoneid)) 
		{
			map_callerid[ringphoneid] = callerid1;
			map_ringext[ringphoneid] += string(" ")+channel1;
		}
	}
	else 	if(ptoken->getKey(TOKEN_STATE) == STATE_RINGING)
	{
		string channel1 = ptoken->getKey(TOKEN_CHANNEL);
		string callerid1 = ptoken->getKey(TOKEN_CALLERID);

		string ringphoneid;
		if(!Utils::ParseChannel(channel1, &ringphoneid)) 
		{
			map_callerid[ringphoneid] = callerid1;
			if(map_ringext.find(ringphoneid) == map_ringext.end())
			{
				map_ringext[ringphoneid] = "";
			}
			string channel2 = StringUtils::TrimSpaces(map_ringext[ringphoneid]);
			string callerid2;
			string ringphoneid2;
			if(!Utils::ParseChannel(channel2, &ringphoneid2))
			{
				callerid2 = map_callerid[ringphoneid2];
			}

			map_ringext[ringphoneid] += string(" ")+channel1;

			string callerid = ptoken->getKey(TOKEN_CALLERID);
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ringing from channel %s (%s) to channel %s (%s) ",
				channel1.c_str(), callerid1.c_str(), channel2.c_str(), callerid2.c_str());
			AsteriskManager::getInstance()->NotifyRing(channel1, channel2, callerid1, callerid2);
		}
		else 
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error parsing channel:%s", channel1.c_str());
		}
	}

	return 0;
}

int 
CommunicationHandler::handlePeerStatusChanged(Token* ptoken) {

	string sPeer = ptoken->getKey(TOKEN_PEER);
	string sStatus = ptoken->getKey(TOKEN_PEER_STATUS);

	AsteriskManager::getInstance()->NotifyExtensionsStatus(sPeer + ":" + sStatus);
	return 1;
}

int 
CommunicationHandler::handleExtensionStatusChanged(Token* ptoken) {

	string sExten = ptoken->getKey(TOKEN_EXTEN);
	string sRawStatus = ptoken->getKey(TOKEN_STATUS);
	string sStatus;

	switch(atoi(sRawStatus.c_str()))
	{
		case 0:
			sStatus = "Ready";
			break;

		case 1:
			sStatus = "On the phone";
			break;

		case 8:
			sStatus = "Ringing";
			break;

		default:
			sStatus = "Unknown(" + sRawStatus + ")";
			break;
	}

	AsteriskManager::getInstance()->NotifyExtensionsStatus(sExten + ":" + sStatus);
	return 1;
}

}; //namespace ASTERISK

