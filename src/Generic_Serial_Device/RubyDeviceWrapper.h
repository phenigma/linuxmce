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
#ifndef DCERUBYDEVICEWRAPPER_H
#define DCERUBYDEVICEWRAPPER_H

#include <string>
#include <map>

namespace DCE {

/**
@author Igor Spac,,,
*/
class RubyDeviceWrapper {
public:
    RubyDeviceWrapper();
    ~RubyDeviceWrapper();

public:
	int getDevId() {
		return devid_;
	}
	void setDevId(int devid) {
		devid_ = devid;
	}
	
	const char* getData(const char* dataname) {
		return devdata_[dataname].c_str();
	}
	void setData(const char* dataname, const char* datavalue) {
		devdata_[dataname] = datavalue;
	}

	std::map<int, RubyDeviceWrapper>& getChildDevices() {
		return childdevices_;
	}
	void setChildDevices(std::map<int, RubyDeviceWrapper>& devices) {
		childdevices_ = devices;
	}
		
public:
	int devid_;
	std::map<std::string, std::string> devdata_;
	std::map<int, RubyDeviceWrapper> childdevices_; /*child device*/
};

};

#endif
