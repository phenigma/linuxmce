//
// C++ Implementation: utils
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "utils.h"

namespace ASTERISK {

int 
Utils::ParseChannel(const std::string channel,
		std::string* psphone, std::string* psphonetype,	std::string* psid) 
{
	int pos, oldpos = 0;
	
	pos = channel.find('/');
	if(pos < 0) {
		return -1;
	}
	if(psphonetype) {
		*psphonetype = channel.substr(0, pos);
	}
	
	oldpos = pos + 1;
	pos = channel.find('-');
	if(pos < 0) {
		return -1;
	}
	
	if(psphone) {
		*psphone = channel.substr(oldpos, pos - oldpos);
	}
	
	if(psid) {
		*psid = channel.substr(pos, channel.length() - pos);
	}
	
	return 0;
}

int 
Utils::ParseCallerID(const std::string callerid,
		std::string* psphonenum, std::string* pscallername)
{
	int pos, oldpos = 0;
	
	pos = callerid.find('<');
	if(pos < 0) {
		return -1;
	}
	if(pscallername) {
		*pscallername = callerid.substr(0, pos - 1);
	}
	
	oldpos = pos;
	pos = callerid.find('>', oldpos);
	if(pos < 0) {
		return -1;
	}

	if(psphonenum) {
		*psphonenum = callerid.substr(oldpos + 1, pos - oldpos - 1);
	}
	
	return 0;
}

};
