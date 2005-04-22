/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

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
