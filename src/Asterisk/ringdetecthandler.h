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
	
public:
};

};

#endif
