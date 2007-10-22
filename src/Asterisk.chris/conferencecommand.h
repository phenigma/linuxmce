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
#ifndef ASTERISKCONFERENCESTATEMACHINE_H
#define ASTERISKCONFERENCESTATEMACHINE_H

#include "asteriskcommand.h"

#include <string>

namespace ASTERISK {

/**
@author 
*/
class ConferenceCommand : public IAsteriskCommand
{
public:
	ConferenceCommand();
	~ConferenceCommand();

public:
	void setExtenNum(std::string extennum);
	void setChannel1(std::string channel);
	void setChannel2(std::string channel);	
	void setCommandID(int commandid);
	
	void handleStartup();
	void handleTerminate();
	bool handleResponse(Token &token);

private:
	std::string extennum;
	std::string channel1;
	std::string channel2;	
	int commandid;
	
private:
};

};

#endif
