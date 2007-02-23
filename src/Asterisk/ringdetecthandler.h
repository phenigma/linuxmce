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
// C++ Interface: ringdetectstatemachine
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKRINGDETECTSTATEMACHINE_H
#define ASTERISKRINGDETECTSTATEMACHINE_H

#include "tokenmanager.h"

#include <string>
#include <map>

namespace ASTERISK {

/**
@author 
*/
class RingDetectHandler : public TokenManager<RingDetectHandler> {
	friend class TokenManager<RingDetectHandler>;
protected:
	RingDetectHandler();
	virtual ~RingDetectHandler();

protected:
	virtual void handleStartup();
	virtual int handleToken(Token* ptoken);
	virtual void handleTerminate();
	
private:
	std::map<std::string,std::string> map_ringext;
};

};

#endif
