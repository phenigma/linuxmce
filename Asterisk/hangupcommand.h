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
#ifndef ASTERISKHANGUPSTATEMACHINE_H
#define ASTERISKHANGUPSTATEMACHINE_H

#include "tokenmanager.h"

#include <string>

namespace ASTERISK {

/**
@author 
*/
class HangupCommand : public TokenManager<HangupCommand>, public ThreadSafeObj
{
	friend class TokenManager<HangupCommand>;
protected:
	HangupCommand();
	virtual ~HangupCommand();

public:
	void setChannel(std::string channel);
	void setCommandID(int commandid);
	
protected:
	virtual void handleStartup();
	virtual int handleToken(Token* ptoken);
	virtual void handleTerminate();

private:
	std::string channel;
	
	int commandid;

private:
};

};

#endif
