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

#include "tokenmanager.h"

#include <string>

namespace ASTERISK {

/**
@author 
*/
class TransferCommand : public TokenManager<TransferCommand>, public ThreadSafeObj
{
	friend class TokenManager<TransferCommand>;
protected:
	TransferCommand();
	virtual ~TransferCommand();

public:
	void setExtenNum(std::strin extennum);
	void setChannel(std::string channel);
	void setCommandID(int commandid);
	
protected:
	virtual void handleStartup();
	virtual int handleToken(Token* ptoken);
	virtual void handleTerminate();

private:
	std::string extennum;
	std::string channel;
	
	int commandid;
	
private:
};

};

#endif
