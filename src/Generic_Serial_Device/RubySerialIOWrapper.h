//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCERUBYSERIALIOWRAPPER_H
#define DCERUBYSERIALIOWRAPPER_H

#include "IOConnection.h"
#include "RubyDeviceWrapper.h"

#include <map>

namespace DCE {

/**
@author Igor Spac,,,
*/
class RubySerialIOWrapper {
	friend class RubySerialIOPool;
public:
    RubySerialIOWrapper();
    ~RubySerialIOWrapper();

	/*accessed from our code*/
public:
	void setConn(IOConnection *pc) {
		conn_ = pc;
	}
	IOConnection * getConn() {
		return conn_;
	}
	
	RubyDeviceWrapper& getDevice() {
		return device_;
	}
	void setDevice(const RubyDeviceWrapper& device) {
		device_ = device;
	}

	/*accessed from ruby code*/
public:
	IOConnection* conn_; /*connection*/
	RubyDeviceWrapper device_; /*our device*/
};

};

#endif
