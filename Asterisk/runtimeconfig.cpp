//
// C++ Implementation: runtimeconfig
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "runtimeconfig.h"

using namespace std;

namespace ASTERISK {

RuntimeConfig* RuntimeConfig::sinstance = 0;

RuntimeConfig::RuntimeConfig() {}
RuntimeConfig::~RuntimeConfig() {}

RuntimeConfig* RuntimeConfig::getInstance() {
	static RuntimeConfig config;
	
	LOCK_OBJ(config); /* perform lock on config as it is static */
	
	if(sinstance == 0) {
		sinstance = &config;
	}

	UNLOCK_OBJ(config);
			
	return sinstance;
}

void RuntimeConfig::regOriginateCall(string phoneid) {
	LOCKED_OP(
		origsrcs[phoneid] = phoneid;
	);
}

void RuntimeConfig::unregOriginateCall(string phoneid) {
	map<string, string>::iterator it;
	LOCKED_OP(	
		it = origsrcs.find(phoneid);
		if(it != origsrcs.end()) {
			origsrcs.erase(it);
		}
	);
}

bool RuntimeConfig::isCallOriginating(string phoneid) {
	bool ret = false;
	map<string, string>::iterator it;
	
	LOCKED_OP(
		it = origsrcs.find(phoneid);
		if(it != origsrcs.end()) {
			ret = true;
		}
	);
	return ret;
}


};
