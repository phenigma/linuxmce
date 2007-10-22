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
// C++ Interface: originatestatemachine
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKTRANSFERSTATEMACHINE_H
#define ASTERISKTRANSFERSTATEMACHINE_H

#include "asteriskcommand.h"

#include <string>

namespace ASTERISK {

/**
@author 
*/
class TransferCommand : public IAsteriskCommand
{
public:
	TransferCommand();
	~TransferCommand();

public:
	void setExtenNum(std::string extennum);
	void setChannel(std::string channel);
	void setCommandID(int commandid);
	
	void handleStartup();
	void handleTerminate();
	bool handleResponse(Token &token);

private:
	std::string extennum;
	std::string channel;
	
	int commandid;
	
private:
};

};

#endif
