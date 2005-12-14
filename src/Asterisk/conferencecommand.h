#ifndef ASTERISKCONFERENCESTATEMACHINE_H
#define ASTERISKCONFERENCESTATEMACHINE_H

#include "tokenmanager.h"

#include <string>

namespace ASTERISK {

/**
@author 
*/
class ConferenceCommand : public TokenManager<ConferenceCommand>, public ThreadSafeObj
{
	friend class TokenManager<ConferenceCommand>;
protected:
	ConferenceCommand();
	virtual ~ConferenceCommand();

public:
	void setExtenNum(std::string extennum);
	void setChannel1(std::string channel);
	void setChannel2(std::string channel);	
	void setCommandID(int commandid);
	
protected:
	virtual void handleStartup();
	virtual int handleToken(Token* ptoken);
	virtual void handleTerminate();

private:
	std::string extennum;
	std::string channel1;
	std::string channel2;	
	int commandid;
	
private:
};

};

#endif
