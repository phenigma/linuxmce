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

#include "RubyDeviceWrapper.h"
#include "RubyCommandWrapper.h"
#include "RubyDCEConnector.h"
#include "RubySerialIOConnectionWrapper.h"

#include <map>

namespace DCE {

class Event_Impl;

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
	void setConn(const RubySerialIOConnectionWrapper& conn) {
		conn_ = conn;
	}
	RubySerialIOConnectionWrapper& getConn() {
		return conn_;
	}
	
	void setDevice(const RubyDeviceWrapper& device) {
		device_ = device;
	}
	RubyDeviceWrapper& getDevice() {
		return device_;
	}
	
	void setDCEConnector(RubyDCEConnector* pdce) {
		pdce_ = pdce;
	}
	RubyDCEConnector* getDCEConnector() {
		return pdce_;
	}
	
public:
	void SendCommand(RubyCommandWrapper* pcmd);

	/*accessed from ruby code*/
public:
	RubySerialIOConnectionWrapper conn_; /*connection*/
	RubyDeviceWrapper device_; /*our device*/
	
private:
	RubyDCEConnector* pdce_;
};

};

#endif
