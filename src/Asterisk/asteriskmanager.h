//
// C++ Interface: asteriskmanager
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKASTERISKMANAGER_H
#define ASTERISKASTERISKMANAGER_H

#include "tokenpool.h"
#include "threadsafeobj.h"

#include <string>

namespace DCE {
	class Asterisk;
}

namespace ASTERISK {

/**
@author 
*/
class AsteriskManager : public ThreadSafeObj
{
    friend class DCE::Asterisk;
    
private:
    AsteriskManager();
    ~AsteriskManager();

    void Initialize(DCE::Asterisk* lpAsterisk);
    void Release();

public:
	static AsteriskManager* getInstance();
	

public:
	/*commands*/
	
	/*originates a call*/
	void Originate(const std::string sPhoneNumber, 
						const std::string sOriginatorNumber,
						const std::string sOriginatorType, 
						const std::string sCallerID,
						int iCommandID);
	
	/*hangs up a call*/
	void Hangup(const std::string sChannel, 
						int iCommandID);
						
	/*events - usually called by state machines*/
	void NotifyRing(const std::string sCallerID /*ringing phone id*/, 
	 					const std::string sSrcExt /*src extention*/, 
						const std::string sChannel /*used as call id*/);
	
	void NotifyResult(int iCommandID, int iResult, 
						const std::string Message);
	
private:
	DCE::Asterisk* pAsterisk;
	
// manager instance
private:
	static AsteriskManager* sinstance;
};

};

#endif
