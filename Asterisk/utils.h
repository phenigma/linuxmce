//
// C++ Interface: utils
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKUTILS_H
#define ASTERISKUTILS_H

#include <string>

namespace ASTERISK {

/**
@author 
*/
class Utils{
public:
	static int ParseChannel(const std::string channel,
		std::string* psphone, std::string* psphonetype = 0,	std::string* psid = 0);
	static int ParseCallerID(const std::string callerid,
		std::string* psphonenume, std::string* pscallername = 0);
};

};

#endif
