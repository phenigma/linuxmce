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

#define MARK_DIALING " DIAL/"

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
		while (party != "")
		{
			string rest=party;
			string extension;
			if(!Utils::ParseParty(party, &extension,&rest)) {
				if(atoi(extension.c_str())>0)
				{
					string channel = ptoken->getKey(TOKEN_CHANNEL);
					map_ringext[extension] += string(" ")+channel;
					g_pPlutoLogger->Write(LV_STATUS, "Will connect channel %s to extension %s", channel.c_str(),extension.c_str());
				}
				else
				{
					string channel = ptoken->getKey(TOKEN_CHANNEL);
					string ringphoneid;
					if(!Utils::ParseChannel(channel, &ringphoneid))
					{
						if(map_ringext.find(ringphoneid) != map_ringext.end())
						{
							int pos = party.rfind('/');
							if(pos>=0)
							{
								string number = party.substr(pos+1,party.length());
								map_ringext[ringphoneid] += string(MARK_DIALING)+number+string("/ ");
								g_pPlutoLogger->Write(LV_STATUS, "Will mark channel %s as dialing to %s", channel.c_str(),number.c_str());								
							}
						}
					}
				}
			} else {
				g_pPlutoLogger->Write(LV_CRITICAL, "Error parsing party:%s", party.c_str());
				return 0;
			}
			party = rest;
		}
	}
	if(ptoken->getKey(TOKEN_EVENT) == EVENT_HANGUP)
	{
		string channel = ptoken->getKey(TOKEN_CHANNEL);
		bool success=false;
		list<map<string,string>::iterator> delete_list;
		for(map<string,string>::iterator it=map_ringext.begin();it!=map_ringext.end();it++)
		{
			string exten=(*it).first;
			string newchan=(*it).second;
			int pos = newchan.find(channel);
			if( pos >=0)
			{
				g_pPlutoLogger->Write(LV_STATUS, "Will delete channel %s from extension %s", channel.c_str(),(*it).first.c_str());
				newchan = newchan.substr(0,pos)+newchan.substr(pos+channel.length(),newchan.length());
				g_pPlutoLogger->Write(LV_STATUS, "Change from [%s] to [%s]",(*it).second.c_str(),newchan.c_str());
				(*it).second = newchan;
				
				if ((*it).second.find_first_not_of(' ')<0)
				{
					delete_list.push_back(it);
					g_pPlutoLogger->Write(LV_STATUS, "Will delete as empty");
				}
				else
				{
					pos=newchan.find(MARK_DIALING);
					if(pos>=0)
					{
						int newpos = newchan.find(' ',pos+1);
						newchan=newchan.substr(0,pos)+newchan.substr(newpos,newchan.length());
						if((newpos=newchan.find_first_not_of(' '))<0)
						{
							delete_list.push_back(it);
							g_pPlutoLogger->Write(LV_STATUS, "Will delete empty DIAL");
						}
						else
						{
							g_pPlutoLogger->Write(LV_STATUS, "DIAL is not empty '%s' %d",newchan.c_str(),newpos);
						}
					}
				}
				
				success=true;
			}
			AsteriskManager* manager = AsteriskManager::getInstance();
			g_pPlutoLogger->Write(LV_STATUS, "Will notify hangup on %s",exten.c_str());
			manager->NotifyHangup(exten);
		}
		for(list<map<string,string>::iterator>::iterator it=delete_list.begin();it!=delete_list.end();it++)
		{
			map_ringext.erase(*it);
		}
		
		if(!success)
		{
			if(channel.find("Local")<0)
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "Hangup on unknown channel %s", channel.c_str());
			}
			return 0;
		}
	}

	if((ptoken->getKey(TOKEN_EVENT) == EVENT_NEWCHANNEL || ptoken->getKey(TOKEN_EVENT) == EVENT_NEWSTATE ) && (ptoken->getKey(TOKEN_STATE) == STATE_RINGING || ptoken->getKey(TOKEN_STATE) == STATE_RING)) 
	{
		string channel = ptoken->getKey(TOKEN_CHANNEL);

		g_pPlutoLogger->Write(LV_STATUS, "Channel Ringing: %s", channel.c_str());
		int pos=channel.find("Local");
		if(pos>=0)
		{
			g_pPlutoLogger->Write(LV_STATUS, "Ignore call on Local channel, wait for real one %d in %s ",pos,channel.c_str());
			return 0;
		}
		string ringphoneid;
		if(!Utils::ParseChannel(channel, &ringphoneid)) {
			if(map_ringext.find(ringphoneid) == map_ringext.end())
			{
//					g_pPlutoLogger->Write(LV_WARNING, "No previos ring to this channel !!!");
				map_ringext[ringphoneid]="";
//					return 0;
			}
			map_ringext[ringphoneid] += string(" ")+channel;
			if(ptoken->getKey(TOKEN_STATE) == STATE_RINGING)
			{
				string callerid = ptoken->getKey(TOKEN_CALLERID);
				if(callerid.find_first_of("0123456789") < 0)
				{
					callerid = "";	
					channel = map_ringext[ringphoneid];
					int oldpos=0;				

					string tmp1, tmp2;
					do
					{
						pos = channel.find(' ',oldpos);
						if(pos <0)
						{
							tmp1 = channel.substr(oldpos, channel.length());
						}
						else
						{
							tmp1= channel.substr(oldpos, pos - oldpos);
						}
						if(!Utils::ParseChannel(tmp1, &tmp2))
						{
							if(tmp2 != ringphoneid)
							{
								callerid += tmp2+" ";
							}
						}
						oldpos = pos+1;

					}
					while(pos>=0);
				}
				if(RuntimeConfig::getInstance()->isCallOriginating(ringphoneid)) {
					g_pPlutoLogger->Write(LV_STATUS, "Phone %s is originating a call. Skipping issue Ring event.",
							ringphoneid.c_str());
				} else {
					AsteriskManager* manager = AsteriskManager::getInstance();
					manager->NotifyRing(callerid, ringphoneid, map_ringext[ringphoneid]);
					g_pPlutoLogger->Write(LV_STATUS, "Phone %s is Ringing. Fire Ring event. with callerid %s",ringphoneid.c_str(),callerid.c_str());
				}
			}

			/* as idea 	:  we need both  map_ringext[ringphoneid] and channel, and use one or another or both depending on situation */
		}
		else {
			g_pPlutoLogger->Write(LV_CRITICAL, "Error parsing channel:%s", channel.c_str());
		}
	}
	if(ptoken->getKey(TOKEN_EVENT) == EVENT_LINK) 
	{
		string channel1 = ptoken->getKey(TOKEN_CHANNEL1);
		string channel2 = ptoken->getKey(TOKEN_CHANNEL2);
		

		g_pPlutoLogger->Write(LV_STATUS, "Linking channels %s and %s", channel1.c_str(),channel2.c_str());
		
		string ringphoneid;
		int chan=1;
		if(Utils::ParseChannel(channel1, &ringphoneid)) {
			chan=2;
			if(Utils::ParseChannel(channel2, &ringphoneid)) {
				g_pPlutoLogger->Write(LV_CRITICAL, "Could not extract phone id from channels");
				return 0;
			}
		}
		if(map_ringext.find(ringphoneid) == map_ringext.end())
		{
			g_pPlutoLogger->Write(LV_WARNING, "No previos ring to this channel !!!");
			return 0;
		}
		if(chan==1)
		{
			map_ringext[ringphoneid] += string(" ")+channel2;
			for(map<string,string>::iterator it=map_ringext.begin();it!=map_ringext.end();it++)
			{
				string thechan=(*it).second;
				if(thechan == channel2)
				map_ringext.erase(it);
				break;
			}
		}
		else
		{
			map_ringext[ringphoneid] += string(" ")+channel1;
			for(map<string,string>::iterator it=map_ringext.begin();it!=map_ringext.end();it++)
			{
				string thechan=(*it).second;
				if(thechan == channel1)
				map_ringext.erase(it);
				break;
			}
		}
		g_pPlutoLogger->Write(LV_STATUS, "Extension %s has channels %s",ringphoneid.c_str(), map_ringext[ringphoneid].c_str());
		AsteriskManager* manager = AsteriskManager::getInstance();
		manager->NotifyRing("", ringphoneid, map_ringext[ringphoneid]);
	}
	
	return 0;
}

void 
RingDetectHandler::handleTerminate() {
	Release();
}

};
