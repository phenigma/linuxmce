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
#ifndef ASTERISKORIGINATESTATEMACHINE_H
#define ASTERISKORIGINATESTATEMACHINE_H

#include "tokenmanager.h"

#include <string>

namespace ASTERISK {

/**
@author 
*/
class OriginateCommand : public TokenManager<OriginateCommand>, public ThreadSafeObj
{
	friend class TokenManager<OriginateCommand>;
protected:
	OriginateCommand();
	virtual ~OriginateCommand();

public:
	void setPhoneNum(std::string phonenum);
	void setPhoneType(std::string phonetype);
	void setExtenNum(std::string extennum); // extention number
	void setCallerID(std::string sCallerID);
	void setCommandID(int iCommandID);
	
protected:
	virtual void handleStartup();
	virtual int handleToken(Token* ptoken);
	virtual void handleTerminate();

private:
	std::string phonenum;
	std::string phonetype;
	std::string extennum;
	std::string callerid;
	
	int commandid;
	
private:
	std::string srcchannel;
	std::string destchannel;
	
	std::string destphonetype;
	std::string destphonenum;
};

};

#endif
