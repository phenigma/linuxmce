//
// C++ Interface: runtimeconfig
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKRUNTIMECONFIG_H
#define ASTERISKRUNTIMECONFIG_H

#include "mutex.h"
#include <map>
#include <string>

#include "threadsafeobj.h"

namespace ASTERISK {

/**
@author 
*/
class RuntimeConfig : public ThreadSafeObj {
public:
    RuntimeConfig();
    ~RuntimeConfig();

public:
	static RuntimeConfig* getInstance();

public:
	void regOriginateCall(std::string phoneid);
	void unregOriginateCall(std::string phoneid);
	bool isCallOriginating(std::string phoneid);
	
private:
	std::map<std::string, std::string> origsrcs;

// runtime config instance
private:
	static RuntimeConfig* sinstance;
};

};

#endif
